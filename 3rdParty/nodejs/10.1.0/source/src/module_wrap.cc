#include <algorithm>
#include <limits.h>  // PATH_MAX
#include <sys/stat.h>  // S_IFDIR
#include "module_wrap.h"

#include "env.h"
#include "node_errors.h"
#include "node_url.h"
#include "util-inl.h"
#include "node_internals.h"
#include "node_contextify.h"
#include "node_watchdog.h"

namespace node {
namespace loader {

using node::contextify::ContextifyContext;
using node::url::URL;
using node::url::URL_FLAGS_FAILED;
using v8::Array;
using v8::Context;
using v8::Function;
using v8::FunctionCallbackInfo;
using v8::FunctionTemplate;
using v8::HandleScope;
using v8::Integer;
using v8::IntegrityLevel;
using v8::Isolate;
using v8::JSON;
using v8::Just;
using v8::Local;
using v8::Maybe;
using v8::MaybeLocal;
using v8::Module;
using v8::Nothing;
using v8::Object;
using v8::Promise;
using v8::ScriptCompiler;
using v8::ScriptOrigin;
using v8::String;
using v8::TryCatch;
using v8::Undefined;
using v8::Value;

static const char* const EXTENSIONS[] = {".mjs", ".js", ".json", ".node"};

ModuleWrap::ModuleWrap(Environment* env,
                       Local<Object> object,
                       Local<Module> module,
                       Local<String> url) : BaseObject(env, object) {
  module_.Reset(env->isolate(), module);
  url_.Reset(env->isolate(), url);
}

ModuleWrap::~ModuleWrap() {
  HandleScope scope(env()->isolate());
  Local<Module> module = module_.Get(env()->isolate());
  auto range = env()->module_map.equal_range(module->GetIdentityHash());
  for (auto it = range.first; it != range.second; ++it) {
    if (it->second == this) {
      env()->module_map.erase(it);
      break;
    }
  }
}

ModuleWrap* ModuleWrap::GetFromModule(Environment* env,
                                      Local<Module> module) {
  ModuleWrap* ret = nullptr;
  auto range = env->module_map.equal_range(module->GetIdentityHash());
  for (auto it = range.first; it != range.second; ++it) {
    if (it->second->module_ == module) {
      ret = it->second;
      break;
    }
  }
  return ret;
}

void ModuleWrap::New(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);
  Isolate* isolate = env->isolate();

  CHECK(args.IsConstructCall());
  Local<Object> that = args.This();

  const int argc = args.Length();
  CHECK_GE(argc, 2);

  CHECK(args[0]->IsString());
  Local<String> source_text = args[0].As<String>();

  CHECK(args[1]->IsString());
  Local<String> url = args[1].As<String>();

  Local<Context> context;
  Local<Integer> line_offset;
  Local<Integer> column_offset;

  if (argc == 5) {
    // new ModuleWrap(source, url, context?, lineOffset, columnOffset)
    if (args[2]->IsUndefined()) {
      context = that->CreationContext();
    } else {
      CHECK(args[2]->IsObject());
      ContextifyContext* sandbox =
          ContextifyContext::ContextFromContextifiedSandbox(
              env, args[2].As<Object>());
      CHECK_NE(sandbox, nullptr);
      context = sandbox->context();
    }

    CHECK(args[3]->IsNumber());
    line_offset = args[3].As<Integer>();

    CHECK(args[4]->IsNumber());
    column_offset = args[4].As<Integer>();
  } else {
    // new ModuleWrap(source, url)
    context = that->CreationContext();
    line_offset = Integer::New(isolate, 0);
    column_offset = Integer::New(isolate, 0);
  }

  Environment::ShouldNotAbortOnUncaughtScope no_abort_scope(env);
  TryCatch try_catch(isolate);
  Local<Module> module;

  // compile
  {
    ScriptOrigin origin(url,
                        line_offset,                          // line offset
                        column_offset,                        // column offset
                        False(isolate),                       // is cross origin
                        Local<Integer>(),                     // script id
                        Local<Value>(),                       // source map URL
                        False(isolate),                       // is opaque (?)
                        False(isolate),                       // is WASM
                        True(isolate));                       // is ES6 module
    Context::Scope context_scope(context);
    ScriptCompiler::Source source(source_text, origin);
    if (!ScriptCompiler::CompileModule(isolate, &source).ToLocal(&module)) {
      CHECK(try_catch.HasCaught());
      CHECK(!try_catch.Message().IsEmpty());
      CHECK(!try_catch.Exception().IsEmpty());
      AppendExceptionLine(env, try_catch.Exception(), try_catch.Message(),
                          ErrorHandlingMode::MODULE_ERROR);
      try_catch.ReThrow();
      return;
    }
  }

  if (!that->Set(context, env->url_string(), url).FromMaybe(false)) {
    return;
  }

  ModuleWrap* obj = new ModuleWrap(env, that, module, url);
  obj->context_.Reset(isolate, context);

  env->module_map.emplace(module->GetIdentityHash(), obj);

  that->SetIntegrityLevel(context, IntegrityLevel::kFrozen);
  args.GetReturnValue().Set(that);
}

void ModuleWrap::Link(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);
  Isolate* isolate = args.GetIsolate();

  CHECK_EQ(args.Length(), 1);
  CHECK(args[0]->IsFunction());

  Local<Object> that = args.This();

  ModuleWrap* obj;
  ASSIGN_OR_RETURN_UNWRAP(&obj, that);

  if (obj->linked_)
    return;
  obj->linked_ = true;

  Local<Function> resolver_arg = args[0].As<Function>();

  Local<Context> mod_context = obj->context_.Get(isolate);
  Local<Module> module = obj->module_.Get(isolate);

  Local<Array> promises = Array::New(isolate,
                                     module->GetModuleRequestsLength());

  // call the dependency resolve callbacks
  for (int i = 0; i < module->GetModuleRequestsLength(); i++) {
    Local<String> specifier = module->GetModuleRequest(i);
    Utf8Value specifier_utf8(env->isolate(), specifier);
    std::string specifier_std(*specifier_utf8, specifier_utf8.length());

    Local<Value> argv[] = {
      specifier
    };

    MaybeLocal<Value> maybe_resolve_return_value =
        resolver_arg->Call(mod_context, that, 1, argv);
    if (maybe_resolve_return_value.IsEmpty()) {
      return;
    }
    Local<Value> resolve_return_value =
        maybe_resolve_return_value.ToLocalChecked();
    if (!resolve_return_value->IsPromise()) {
      env->ThrowError("linking error, expected resolver to return a promise");
    }
    Local<Promise> resolve_promise = resolve_return_value.As<Promise>();
    obj->resolve_cache_[specifier_std].Reset(env->isolate(), resolve_promise);

    promises->Set(mod_context, i, resolve_promise).FromJust();
  }

  args.GetReturnValue().Set(promises);
}

void ModuleWrap::Instantiate(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);
  Isolate* isolate = args.GetIsolate();
  ModuleWrap* obj;
  ASSIGN_OR_RETURN_UNWRAP(&obj, args.This());
  Local<Context> context = obj->context_.Get(isolate);
  Local<Module> module = obj->module_.Get(isolate);
  TryCatch try_catch(isolate);
  Maybe<bool> ok =
      module->InstantiateModule(context, ModuleWrap::ResolveCallback);

  // clear resolve cache on instantiate
  obj->resolve_cache_.clear();

  if (!ok.FromMaybe(false)) {
    CHECK(try_catch.HasCaught());
    CHECK(!try_catch.Message().IsEmpty());
    CHECK(!try_catch.Exception().IsEmpty());
    AppendExceptionLine(env, try_catch.Exception(), try_catch.Message(),
                        ErrorHandlingMode::MODULE_ERROR);
    try_catch.ReThrow();
    return;
  }
}

void ModuleWrap::Evaluate(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);
  Isolate* isolate = env->isolate();
  ModuleWrap* obj;
  ASSIGN_OR_RETURN_UNWRAP(&obj, args.This());
  Local<Context> context = obj->context_.Get(isolate);
  Local<Module> module = obj->module_.Get(isolate);

  // module.evaluate(timeout, breakOnSigint)
  CHECK_EQ(args.Length(), 2);

  CHECK(args[0]->IsNumber());
  int64_t timeout = args[0]->IntegerValue(env->context()).FromJust();

  CHECK(args[1]->IsBoolean());
  bool break_on_sigint = args[1]->IsTrue();

  Environment::ShouldNotAbortOnUncaughtScope no_abort_scope(env);
  TryCatch try_catch(isolate);

  bool timed_out = false;
  bool received_signal = false;
  MaybeLocal<Value> result;
  if (break_on_sigint && timeout != -1) {
    Watchdog wd(isolate, timeout, &timed_out);
    SigintWatchdog swd(isolate, &received_signal);
    result = module->Evaluate(context);
  } else if (break_on_sigint) {
    SigintWatchdog swd(isolate, &received_signal);
    result = module->Evaluate(context);
  } else if (timeout != -1) {
    Watchdog wd(isolate, timeout, &timed_out);
    result = module->Evaluate(context);
  } else {
    result = module->Evaluate(context);
  }

  // Convert the termination exception into a regular exception.
  if (timed_out || received_signal) {
    env->isolate()->CancelTerminateExecution();
    // It is possible that execution was terminated by another timeout in
    // which this timeout is nested, so check whether one of the watchdogs
    // from this invocation is responsible for termination.
    if (timed_out) {
      env->ThrowError("Script execution timed out.");
    } else if (received_signal) {
      env->ThrowError("Script execution interrupted.");
    }
  }

  if (try_catch.HasCaught()) {
    try_catch.ReThrow();
    return;
  }

  args.GetReturnValue().Set(result.ToLocalChecked());
}

void ModuleWrap::Namespace(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);
  Isolate* isolate = args.GetIsolate();
  ModuleWrap* obj;
  ASSIGN_OR_RETURN_UNWRAP(&obj, args.This());

  Local<Module> module = obj->module_.Get(isolate);

  switch (module->GetStatus()) {
    default:
      return env->ThrowError(
          "cannot get namespace, Module has not been instantiated");
    case v8::Module::Status::kInstantiated:
    case v8::Module::Status::kEvaluating:
    case v8::Module::Status::kEvaluated:
      break;
  }

  Local<Value> result = module->GetModuleNamespace();
  args.GetReturnValue().Set(result);
}

void ModuleWrap::GetStatus(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  ModuleWrap* obj;
  ASSIGN_OR_RETURN_UNWRAP(&obj, args.This());

  Local<Module> module = obj->module_.Get(isolate);

  args.GetReturnValue().Set(module->GetStatus());
}

void ModuleWrap::GetStaticDependencySpecifiers(
    const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);
  ModuleWrap* obj;
  ASSIGN_OR_RETURN_UNWRAP(&obj, args.This());

  Local<Module> module = obj->module_.Get(env->isolate());

  int count = module->GetModuleRequestsLength();

  Local<Array> specifiers = Array::New(env->isolate(), count);

  for (int i = 0; i < count; i++)
    specifiers->Set(env->context(), i, module->GetModuleRequest(i)).FromJust();

  args.GetReturnValue().Set(specifiers);
}

void ModuleWrap::GetError(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  ModuleWrap* obj;
  ASSIGN_OR_RETURN_UNWRAP(&obj, args.This());

  Local<Module> module = obj->module_.Get(isolate);

  args.GetReturnValue().Set(module->GetException());
}

MaybeLocal<Module> ModuleWrap::ResolveCallback(Local<Context> context,
                                               Local<String> specifier,
                                               Local<Module> referrer) {
  Environment* env = Environment::GetCurrent(context);
  Isolate* isolate = env->isolate();
  if (env->module_map.count(referrer->GetIdentityHash()) == 0) {
    env->ThrowError("linking error, unknown module");
    return MaybeLocal<Module>();
  }

  ModuleWrap* dependent = ModuleWrap::GetFromModule(env, referrer);

  if (dependent == nullptr) {
    env->ThrowError("linking error, null dep");
    return MaybeLocal<Module>();
  }

  Utf8Value specifier_utf8(env->isolate(), specifier);
  std::string specifier_std(*specifier_utf8, specifier_utf8.length());

  if (dependent->resolve_cache_.count(specifier_std) != 1) {
    env->ThrowError("linking error, not in local cache");
    return MaybeLocal<Module>();
  }

  Local<Promise> resolve_promise =
      dependent->resolve_cache_[specifier_std].Get(isolate);

  if (resolve_promise->State() != Promise::kFulfilled) {
    env->ThrowError("linking error, dependency promises must be resolved on "
                    "instantiate");
    return MaybeLocal<Module>();
  }

  Local<Object> module_object = resolve_promise->Result().As<Object>();
  if (module_object.IsEmpty() || !module_object->IsObject()) {
    env->ThrowError("linking error, expected a valid module object from "
                    "resolver");
    return MaybeLocal<Module>();
  }

  ModuleWrap* module;
  ASSIGN_OR_RETURN_UNWRAP(&module, module_object, MaybeLocal<Module>());
  return module->module_.Get(env->isolate());
}

namespace {

// Tests whether a path starts with /, ./ or ../
// In WhatWG terminology, the alternative case is called a "bare" specifier
// (e.g. in `import "jquery"`).
inline bool ShouldBeTreatedAsRelativeOrAbsolutePath(
    const std::string& specifier) {
  size_t len = specifier.length();
  if (len == 0)
    return false;
  if (specifier[0] == '/') {
    return true;
  } else if (specifier[0] == '.') {
    if (len == 1 || specifier[1] == '/')
      return true;
    if (specifier[1] == '.') {
      if (len == 2 || specifier[2] == '/')
        return true;
    }
  }
  return false;
}

std::string ReadFile(uv_file file) {
  std::string contents;
  uv_fs_t req;
  char buffer_memory[4096];
  uv_buf_t buf = uv_buf_init(buffer_memory, sizeof(buffer_memory));
  int r;

  do {
    r = uv_fs_read(uv_default_loop(),
                   &req,
                   file,
                   &buf,
                   1,
                   contents.length(),  // offset
                   nullptr);
    uv_fs_req_cleanup(&req);

    if (r <= 0)
      break;
    contents.append(buf.base, r);
  } while (true);
  return contents;
}

enum CheckFileOptions {
  LEAVE_OPEN_AFTER_CHECK,
  CLOSE_AFTER_CHECK
};

Maybe<uv_file> CheckFile(const std::string& path,
                         CheckFileOptions opt = CLOSE_AFTER_CHECK) {
  uv_fs_t fs_req;
  if (path.empty()) {
    return Nothing<uv_file>();
  }

  uv_file fd = uv_fs_open(nullptr, &fs_req, path.c_str(), O_RDONLY, 0, nullptr);
  uv_fs_req_cleanup(&fs_req);

  if (fd < 0) {
    return Nothing<uv_file>();
  }

  uv_fs_fstat(nullptr, &fs_req, fd, nullptr);
  uint64_t is_directory = fs_req.statbuf.st_mode & S_IFDIR;
  uv_fs_req_cleanup(&fs_req);

  if (is_directory) {
    CHECK_EQ(0, uv_fs_close(nullptr, &fs_req, fd, nullptr));
    uv_fs_req_cleanup(&fs_req);
    return Nothing<uv_file>();
  }

  if (opt == CLOSE_AFTER_CHECK) {
    CHECK_EQ(0, uv_fs_close(nullptr, &fs_req, fd, nullptr));
    uv_fs_req_cleanup(&fs_req);
  }

  return Just(fd);
}

const PackageConfig& GetPackageConfig(Environment* env,
                                                   const std::string path) {
  auto existing = env->package_json_cache.find(path);
  if (existing != env->package_json_cache.end()) {
    return existing->second;
  }
  Maybe<uv_file> check = CheckFile(path, LEAVE_OPEN_AFTER_CHECK);
  if (check.IsNothing()) {
    auto entry = env->package_json_cache.emplace(path,
        PackageConfig { Exists::No, IsValid::Yes, HasMain::No, "" });
    return entry.first->second;
  }

  Isolate* isolate = env->isolate();
  v8::HandleScope handle_scope(isolate);

  std::string pkg_src = ReadFile(check.FromJust());
  uv_fs_t fs_req;
  CHECK_EQ(0, uv_fs_close(nullptr, &fs_req, check.FromJust(), nullptr));
  uv_fs_req_cleanup(&fs_req);

  Local<String> src;
  if (!String::NewFromUtf8(isolate,
                           pkg_src.c_str(),
                           v8::NewStringType::kNormal,
                           pkg_src.length()).ToLocal(&src)) {
    auto entry = env->package_json_cache.emplace(path,
        PackageConfig { Exists::No, IsValid::Yes, HasMain::No, "" });
    return entry.first->second;
  }

  Local<Value> pkg_json_v;
  Local<Object> pkg_json;

  if (!JSON::Parse(env->context(), src).ToLocal(&pkg_json_v) ||
      !pkg_json_v->ToObject(env->context()).ToLocal(&pkg_json)) {
    auto entry = env->package_json_cache.emplace(path,
        PackageConfig { Exists::Yes, IsValid::No, HasMain::No, "" });
    return entry.first->second;
  }

  Local<Value> pkg_main;
  HasMain::Bool has_main = HasMain::No;
  std::string main_std;
  if (pkg_json->Get(env->context(), env->main_string()).ToLocal(&pkg_main)) {
    has_main = HasMain::Yes;
    Utf8Value main_utf8(isolate, pkg_main);
    main_std.assign(std::string(*main_utf8, main_utf8.length()));
  }

  auto entry = env->package_json_cache.emplace(path,
      PackageConfig { Exists::Yes, IsValid::Yes, has_main, main_std });
  return entry.first->second;
}

enum ResolveExtensionsOptions {
  TRY_EXACT_NAME,
  ONLY_VIA_EXTENSIONS
};

template<ResolveExtensionsOptions options>
Maybe<URL> ResolveExtensions(const URL& search) {
  if (options == TRY_EXACT_NAME) {
    std::string filePath = search.ToFilePath();
    Maybe<uv_file> check = CheckFile(filePath);
    if (!check.IsNothing()) {
      return Just(search);
    }
  }

  for (const char* extension : EXTENSIONS) {
    URL guess(search.path() + extension, &search);
    Maybe<uv_file> check = CheckFile(guess.ToFilePath());
    if (!check.IsNothing()) {
      return Just(guess);
    }
  }

  return Nothing<URL>();
}

inline Maybe<URL> ResolveIndex(const URL& search) {
  return ResolveExtensions<ONLY_VIA_EXTENSIONS>(URL("index", search));
}

Maybe<URL> ResolveMain(Environment* env, const URL& search) {
  URL pkg("package.json", &search);

  const PackageConfig& pjson =
      GetPackageConfig(env, pkg.ToFilePath());
  // Note invalid package.json should throw in resolver
  // currently we silently ignore which is incorrect
  if (pjson.exists == Exists::No ||
      pjson.is_valid == IsValid::No ||
      pjson.has_main == HasMain::No) {
    return Nothing<URL>();
  }
  if (!ShouldBeTreatedAsRelativeOrAbsolutePath(pjson.main)) {
    return Resolve(env, "./" + pjson.main, search, IgnoreMain);
  }
  return Resolve(env, pjson.main, search, IgnoreMain);
}

Maybe<URL> ResolveModule(Environment* env,
                         const std::string& specifier,
                         const URL& base) {
  URL parent(".", base);
  URL dir("");
  do {
    dir = parent;
    Maybe<URL> check =
        Resolve(env, "./node_modules/" + specifier, dir, CheckMain);
    if (!check.IsNothing()) {
      const size_t limit = specifier.find('/');
      const size_t spec_len =
          limit == std::string::npos ? specifier.length() :
                                       limit + 1;
      std::string chroot =
          dir.path() + "node_modules/" + specifier.substr(0, spec_len);
      if (check.FromJust().path().substr(0, chroot.length()) != chroot) {
        return Nothing<URL>();
      }
      return check;
    } else {
      // TODO(bmeck) PREVENT FALLTHROUGH
    }
    parent = URL("..", &dir);
  } while (parent.path() != dir.path());
  return Nothing<URL>();
}

Maybe<URL> ResolveDirectory(Environment* env,
                            const URL& search,
                            PackageMainCheck check_pjson_main) {
  if (check_pjson_main) {
    Maybe<URL> main = ResolveMain(env, search);
    if (!main.IsNothing())
      return main;
  }
  return ResolveIndex(search);
}

}  // anonymous namespace

Maybe<URL> Resolve(Environment* env,
                   const std::string& specifier,
                   const URL& base,
                   PackageMainCheck check_pjson_main) {
  URL pure_url(specifier);
  if (!(pure_url.flags() & URL_FLAGS_FAILED)) {
    // just check existence, without altering
    Maybe<uv_file> check = CheckFile(pure_url.ToFilePath());
    if (check.IsNothing()) {
      return Nothing<URL>();
    }
    return Just(pure_url);
  }
  if (specifier.length() == 0) {
    return Nothing<URL>();
  }
  if (ShouldBeTreatedAsRelativeOrAbsolutePath(specifier)) {
    URL resolved(specifier, base);
    Maybe<URL> file = ResolveExtensions<TRY_EXACT_NAME>(resolved);
    if (!file.IsNothing())
      return file;
    if (specifier.back() != '/') {
      resolved = URL(specifier + "/", base);
    }
    return ResolveDirectory(env, resolved, check_pjson_main);
  } else {
    return ResolveModule(env, specifier, base);
  }
}

void ModuleWrap::Resolve(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);

  // module.resolve(specifier, url)
  CHECK_EQ(args.Length(), 2);

  CHECK(args[0]->IsString());
  Utf8Value specifier_utf8(env->isolate(), args[0]);
  std::string specifier_std(*specifier_utf8, specifier_utf8.length());

  CHECK(args[1]->IsString());
  Utf8Value url_utf8(env->isolate(), args[1]);
  URL url(*url_utf8, url_utf8.length());

  if (url.flags() & URL_FLAGS_FAILED) {
    return node::THROW_ERR_INVALID_ARG_TYPE(
        env, "second argument is not a URL string");
  }

  Maybe<URL> result = node::loader::Resolve(env, specifier_std, url);
  if (result.IsNothing() || (result.FromJust().flags() & URL_FLAGS_FAILED)) {
    std::string msg = "Cannot find module " + specifier_std;
    return node::THROW_ERR_MISSING_MODULE(env, msg.c_str());
  }

  args.GetReturnValue().Set(result.FromJust().ToObject(env));
}

static MaybeLocal<Promise> ImportModuleDynamically(
    Local<Context> context,
    Local<v8::ScriptOrModule> referrer,
    Local<String> specifier) {
  Isolate* iso = context->GetIsolate();
  Environment* env = Environment::GetCurrent(context);
  v8::EscapableHandleScope handle_scope(iso);

  if (env->context() != context) {
    auto maybe_resolver = Promise::Resolver::New(context);
    Local<Promise::Resolver> resolver;
    if (maybe_resolver.ToLocal(&resolver)) {
      // TODO(jkrems): Turn into proper error object w/ code
      Local<Value> error = v8::Exception::Error(
        OneByteString(iso, "import() called outside of main context"));
      if (resolver->Reject(context, error).IsJust()) {
        return handle_scope.Escape(resolver.As<Promise>());
      }
    }
    return MaybeLocal<Promise>();
  }

  Local<Function> import_callback =
    env->host_import_module_dynamically_callback();
  Local<Value> import_args[] = {
    referrer->GetResourceName(),
    Local<Value>(specifier)
  };
  MaybeLocal<Value> maybe_result = import_callback->Call(context,
                                                         v8::Undefined(iso),
                                                         2,
                                                         import_args);

  Local<Value> result;
  if (maybe_result.ToLocal(&result)) {
    return handle_scope.Escape(result.As<Promise>());
  }
  return MaybeLocal<Promise>();
}

void ModuleWrap::SetImportModuleDynamicallyCallback(
    const FunctionCallbackInfo<Value>& args) {
  Isolate* iso = args.GetIsolate();
  Environment* env = Environment::GetCurrent(args);
  HandleScope handle_scope(iso);

  CHECK_EQ(args.Length(), 1);
  CHECK(args[0]->IsFunction());
  Local<Function> import_callback = args[0].As<Function>();
  env->set_host_import_module_dynamically_callback(import_callback);

  iso->SetHostImportModuleDynamicallyCallback(ImportModuleDynamically);
}

void ModuleWrap::HostInitializeImportMetaObjectCallback(
    Local<Context> context, Local<Module> module, Local<Object> meta) {
  Isolate* isolate = context->GetIsolate();
  Environment* env = Environment::GetCurrent(context);
  ModuleWrap* module_wrap = ModuleWrap::GetFromModule(env, module);

  if (module_wrap == nullptr) {
    return;
  }

  Local<Object> wrap = module_wrap->object();
  Local<Function> callback =
      env->host_initialize_import_meta_object_callback();
  Local<Value> args[] = { wrap, meta };
  callback->Call(context, Undefined(isolate), arraysize(args), args)
      .ToLocalChecked();
}

void ModuleWrap::SetInitializeImportMetaObjectCallback(
    const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);
  Isolate* isolate = env->isolate();

  CHECK_EQ(args.Length(), 1);
  CHECK(args[0]->IsFunction());
  Local<Function> import_meta_callback = args[0].As<Function>();
  env->set_host_initialize_import_meta_object_callback(import_meta_callback);

  isolate->SetHostInitializeImportMetaObjectCallback(
      HostInitializeImportMetaObjectCallback);
}

void ModuleWrap::Initialize(Local<Object> target,
                            Local<Value> unused,
                            Local<Context> context) {
  Environment* env = Environment::GetCurrent(context);
  Isolate* isolate = env->isolate();

  Local<FunctionTemplate> tpl = env->NewFunctionTemplate(New);
  tpl->SetClassName(FIXED_ONE_BYTE_STRING(isolate, "ModuleWrap"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  env->SetProtoMethod(tpl, "link", Link);
  env->SetProtoMethod(tpl, "instantiate", Instantiate);
  env->SetProtoMethod(tpl, "evaluate", Evaluate);
  env->SetProtoMethod(tpl, "namespace", Namespace);
  env->SetProtoMethod(tpl, "getStatus", GetStatus);
  env->SetProtoMethod(tpl, "getError", GetError);
  env->SetProtoMethod(tpl, "getStaticDependencySpecifiers",
                      GetStaticDependencySpecifiers);

  target->Set(FIXED_ONE_BYTE_STRING(isolate, "ModuleWrap"), tpl->GetFunction());
  env->SetMethod(target, "resolve", node::loader::ModuleWrap::Resolve);
  env->SetMethod(target,
                 "setImportModuleDynamicallyCallback",
                 node::loader::ModuleWrap::SetImportModuleDynamicallyCallback);
  env->SetMethod(target,
                 "setInitializeImportMetaObjectCallback",
                 ModuleWrap::SetInitializeImportMetaObjectCallback);

#define V(name)                                                                \
    target->Set(context,                                                       \
      FIXED_ONE_BYTE_STRING(env->isolate(), #name),                            \
      Integer::New(env->isolate(), Module::Status::name))                      \
        .FromJust()
    V(kUninstantiated);
    V(kInstantiating);
    V(kInstantiated);
    V(kEvaluating);
    V(kEvaluated);
    V(kErrored);
#undef V
}

}  // namespace loader
}  // namespace node

NODE_MODULE_CONTEXT_AWARE_INTERNAL(module_wrap,
                                   node::loader::ModuleWrap::Initialize)
