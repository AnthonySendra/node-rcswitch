#include "RCSwitchNode.h"
#include <unistd.h>

Nan::Persistent<v8::Function> RCSwitchNode::constructor;

void RCSwitchNode::Init(v8::Local<v8::Object> exports) {
  Nan::HandleScope scope;

  if(getuid() > 0) {
    if(wiringPiSetupSys() == -1) {
      Nan::ThrowTypeError("rcswitch: GPIO initialization failed. Expose pins in /sys/class/gpio or re-run as root.");
      return;
    }
  } else if(wiringPiSetup() == -1 ) {
      Nan::ThrowTypeError("rcswitch: GPIO initialization failed. Try exposeing pins in /sys/class/gpio and re-running as standard user.");
      return;
  }

  // Prepare constructor template
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
  tpl->SetClassName(Nan::New("RCSwitch").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1); // 1 since this is a constructor function

  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("protocol").ToLocalChecked(), GetProtocol); //, SetProtocol); TODO <- Error at compile-time... ?
  Nan::SetPrototypeMethod(tpl, "send", Send);

  // Prototype
  Nan::SetPrototypeMethod(tpl, "send", Send);
  Nan::SetPrototypeMethod(tpl, "enableTransmit", EnableTransmit);
  Nan::SetPrototypeMethod(tpl, "disableTransmit", DisableTransmit);
  Nan::SetPrototypeMethod(tpl, "enableReceive", EnableReceive);
  Nan::SetPrototypeMethod(tpl, "disableReceive", DisableReceive);
  Nan::SetPrototypeMethod(tpl, "switchOn", SwitchOn);
  Nan::SetPrototypeMethod(tpl, "switchOff", SwitchOff);
  Nan::SetPrototypeMethod(tpl, "available", Available);
  Nan::SetPrototypeMethod(tpl, "resetAvailable", ResetAvailable);
  Nan::SetPrototypeMethod(tpl, "getReceivedValue", GetReceivedValue);

  constructor.Reset(tpl->GetFunction());
  exports->Set(Nan::New("RCSwitch").ToLocalChecked(), tpl->GetFunction());
}

 RCSwitchNode::RCSwitchNode() {}
 RCSwitchNode::~RCSwitchNode() {}

void RCSwitchNode::SwitchOp(const Nan::FunctionCallbackInfo<v8::Value>& info, bool switchOn) {
  Nan::HandleScope scope;
  RCSwitchNode* thiz = ObjectWrap::Unwrap<RCSwitchNode>(info.Holder());

  info.GetReturnValue().Set(false);
  if(info.Length() == 2) {
    v8::Handle<v8::Value> group = info[0];
    v8::Handle<v8::Value> swtch = info[1];

    if(group->IsInt32() && swtch->IsInt32()) {
      switchOp2(group->Int32Value(), swtch->Int32Value());
      info.GetReturnValue().Set(true);
    } else if(group->IsString() && swtch->IsInt32()) {
      Nan::Utf8String sGroup(group);

      if(sGroup.length() >= 5) {
        switchOp2(*sGroup, swtch->Int32Value());
        info.GetReturnValue().Set(true);
      }
    }
  } else if(info.Length() == 3) {
    v8::Handle<v8::Value> famly = info[0];
    v8::Handle<v8::Value> group = info[1];
    v8::Handle<v8::Value> devce = info[2];

    if(famly->IsString() && group->IsInt32() && devce->IsInt32()) {
      Nan::Utf8String v8str(famly);

      if(v8str.length() > 0) {
        switchOp3(*(*v8str), group->Int32Value(), devce->Int32Value());
        info.GetReturnValue().Set(true);
      }
    }
  }
}

void RCSwitchNode::New(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  if (info.IsConstructCall()) {
    // Invoked as constructor: `new MyObject(...)`
    RCSwitchNode* obj = new RCSwitchNode();
    obj->Wrap(info.This());
    info.GetReturnValue().Set(info.This());
  } else {
    // Invoked as plain function `MyObject(...)`, turn into construct call.
    const int argc = 0;
    v8::Local<v8::Value> argv[argc];
    v8::Local<v8::Function> cons = Nan::New<v8::Function>(constructor);
    info.GetReturnValue().Set(cons->NewInstance(argc, argv));
  }
}

void RCSwitchNode::Send(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  Nan::HandleScope scope;

  RCSwitchNode* obj = ObjectWrap::Unwrap<RCSwitchNode>(info.Holder());

  Nan::Utf8String v8str(info[0]);
  obj->rcswitch.send(*v8str);

  info.GetReturnValue().Set(true);
}

// notification.enableTransmit();
void RCSwitchNode::EnableTransmit(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  Nan::HandleScope scope;
  RCSwitchNode* obj = ObjectWrap::Unwrap<RCSwitchNode>(info.Holder());

  v8::Local<v8::Value> pinNr = info[0];
  if(pinNr->IsInt32()) {
    obj->rcswitch.enableTransmit(pinNr->Int32Value());
    info.GetReturnValue().Set(true);
  } else {
    info.GetReturnValue().Set(false);
  }
}

// notification.disableTransmit();
void RCSwitchNode::DisableTransmit(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  Nan::HandleScope scope;

  RCSwitchNode* obj = ObjectWrap::Unwrap<RCSwitchNode>(info.Holder());
  obj->rcswitch.disableTransmit();
  info.GetReturnValue().Set(true);
}

// notification.EnableReceive();
void RCSwitchNode::EnableReceive(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  Nan::HandleScope scope;
  RCSwitchNode* obj = ObjectWrap::Unwrap<RCSwitchNode>(info.Holder());

  v8::Local<v8::Value> pinNr = info[0];
  if(pinNr->IsInt32()) {
    obj->rcswitch.enableReceive(pinNr->Int32Value());
    info.GetReturnValue().Set(true);
  } else {
    info.GetReturnValue().Set(false);
  }
}

// notification.disableReceive();
void RCSwitchNode::DisableReceive(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  Nan::HandleScope scope;

  RCSwitchNode* obj = ObjectWrap::Unwrap<RCSwitchNode>(info.Holder());
  obj->rcswitch.disableReceive();
  info.GetReturnValue().Set(true);
}

void RCSwitchNode::SwitchOn(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  SwitchOp(info, true);
}

void RCSwitchNode::SwitchOff(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  SwitchOp(info, false);
}

// notification.available();
void RCSwitchNode::Available(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  Nan::HandleScope scope;

  RCSwitchNode* obj = ObjectWrap::Unwrap<RCSwitchNode>(info.Holder());
  info.GetReturnValue().Set(Nan::New<v8::Boolean>(obj->rcswitch.available()));
}

// notification.resetAvailable();
void RCSwitchNode::ResetAvailable(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  Nan::HandleScope scope;

  RCSwitchNode* obj = ObjectWrap::Unwrap<RCSwitchNode>(info.Holder());
  obj->rcswitch.resetAvailable();
  info.GetReturnValue().Set(true);
}

// notification.getReceivedValue();
void RCSwitchNode::GetReceivedValue(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  Nan::HandleScope scope;

  RCSwitchNode* obj = ObjectWrap::Unwrap<RCSwitchNode>(info.Holder());
  info.GetReturnValue().Set(Nan::New<v8::Uint32>((uint32_t)obj->rcswitch.getReceivedValue()));
}

// notification.protocol=
void RCSwitchNode::SetProtocol(v8::Local<v8::String> property, v8::Local<v8::Value> value, const Nan::PropertyCallbackInfo<v8::Value>& info) {
  RCSwitchNode* obj = ObjectWrap::Unwrap<RCSwitchNode>(info.Holder());

  if(value->IsInt32())
    obj->rcswitch.setProtocol(value->Int32Value());
}

// notification.protocol
void RCSwitchNode::GetProtocol(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value>& info) {
  RCSwitchNode* obj = ObjectWrap::Unwrap<RCSwitchNode>(info.Holder());
  info.GetReturnValue().Set(Nan::New<v8::Uint32>(obj->rcswitch.getReceivedProtocol()));
}
