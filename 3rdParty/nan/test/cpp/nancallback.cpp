/*********************************************************************
 * NAN - Native Abstractions for Node.js
 *
 * Copyright (c) 2018 NAN contributors
 *
 * MIT License <https://github.com/nodejs/nan/blob/master/LICENSE.md>
 ********************************************************************/

#include <nan.h>

using namespace Nan;  // NOLINT(build/namespaces)

NAN_METHOD(GlobalContext) {
  AsyncResource resource("nan:test.nancallback");
  Callback(To<v8::Function>(info[0]).ToLocalChecked()).Call(0, NULL, &resource);
}

NAN_METHOD(SpecificContext) {
  AsyncResource resource("nan:test.nancallback");
  Callback cb(To<v8::Function>(info[0]).ToLocalChecked());
  cb.Call(GetCurrentContext()->Global(), 0, NULL, &resource);
}

NAN_METHOD(CustomReceiver) {
  AsyncResource resource("nan:test.nancallback");
  Callback cb(To<v8::Function>(info[0]).ToLocalChecked());
  cb.Call(To<v8::Object>(info[1]).ToLocalChecked(), 0, NULL, &resource);
}

NAN_METHOD(CompareCallbacks) {
  Callback cb1(To<v8::Function>(info[0]).ToLocalChecked());
  Callback cb2(To<v8::Function>(info[1]).ToLocalChecked());
  Callback cb3(To<v8::Function>(info[2]).ToLocalChecked());

  info.GetReturnValue().Set(New<v8::Boolean>(cb1 == cb2 && cb1 != cb3));
}

NAN_METHOD(CallDirect) {
  Callback cb(To<v8::Function>(info[0]).ToLocalChecked());
  (*cb)->Call(GetCurrentContext()->Global(), 0, NULL);
}

NAN_METHOD(CallAsFunction) {
  AsyncResource resource("nan:test.nancallback");
  Callback(To<v8::Function>(info[0]).ToLocalChecked())(&resource);
}

NAN_METHOD(ResetUnset) {
  Callback callback;
  callback.Reset();
  info.GetReturnValue().Set(callback.IsEmpty());
}

NAN_METHOD(ResetSet) {
  Callback callback(To<v8::Function>(info[0]).ToLocalChecked());
  callback.Reset();
  info.GetReturnValue().Set(callback.IsEmpty());
}

NAN_MODULE_INIT(Init) {
  Set(target
    , New<v8::String>("globalContext").ToLocalChecked()
    , New<v8::FunctionTemplate>(GlobalContext)->GetFunction()
  );
  Set(target
    , New<v8::String>("specificContext").ToLocalChecked()
    , New<v8::FunctionTemplate>(SpecificContext)->GetFunction()
  );
  Set(target
    , New<v8::String>("customReceiver").ToLocalChecked()
    , New<v8::FunctionTemplate>(CustomReceiver)->GetFunction()
  );
  Set(target
    , New<v8::String>("compareCallbacks").ToLocalChecked()
    , New<v8::FunctionTemplate>(CompareCallbacks)->GetFunction()
  );
  Set(target
    , New<v8::String>("callDirect").ToLocalChecked()
    , New<v8::FunctionTemplate>(CallDirect)->GetFunction()
  );
  Set(target
    , New<v8::String>("callAsFunction").ToLocalChecked()
    , New<v8::FunctionTemplate>(CallAsFunction)->GetFunction()
  );
  Set(target
    , New<v8::String>("resetUnset").ToLocalChecked()
    , New<v8::FunctionTemplate>(ResetUnset)->GetFunction()
  );
  Set(target
    , New<v8::String>("resetSet").ToLocalChecked()
    , New<v8::FunctionTemplate>(ResetSet)->GetFunction()
  );
}

NODE_MODULE(nancallback, Init)
