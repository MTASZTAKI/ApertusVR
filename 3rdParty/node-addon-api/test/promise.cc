#include "napi.h"

using namespace Napi;

Value IsPromise(const CallbackInfo& info) {
  return Boolean::New(info.Env(), info[0].IsPromise());
}

Value ResolvePromise(const CallbackInfo& info) {
  auto deferred = Promise::Deferred::New(info.Env());
  deferred.Resolve(info[0]);
  return deferred.Promise();
}

Value RejectPromise(const CallbackInfo& info) {
  auto deferred = Promise::Deferred::New(info.Env());
  deferred.Reject(info[0]);
  return deferred.Promise();
}

Object InitPromise(Env env) {
  Object exports = Object::New(env);

  exports["isPromise"] = Function::New(env, IsPromise);
  exports["resolvePromise"] = Function::New(env, ResolvePromise);
  exports["rejectPromise"] = Function::New(env, RejectPromise);

  return exports;
}
