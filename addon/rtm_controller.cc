// Copyright (c) 2017-2017 Agora.io, Inc.
// a module that communition with Lbes and Lbec with Singnaling

#include "rtm_controller.h"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <utility>
#include <sstream>
#include "node_napi_api.h"
#include "node_async_queue.h"
#include "rtm_channel_event_handler.h"

using std::cout;
using std::endl;
using std::string;
using std::vector;

uv_async_t async;

static Nan::Persistent<v8::Function> constructor;

using namespace v8;

#define MAKE_JS_CALL_0(ev)                                               \
  auto it = m_callbacks.find(ev);                                        \
  if (it != m_callbacks.end())                                           \
  {                                                                      \
    Isolate *isolate = Isolate::GetCurrent();                            \
    HandleScope scope(isolate);                                          \
    NodeEventCallback &cb = *it->second;                                 \
    cb.callback.Get(isolate)->Call(cb.js_this.Get(isolate), 0, nullptr); \
  }

#define MAKE_JS_CALL_1(ev, type, param)                               \
  auto it = m_callbacks.find(ev);                                     \
  if (it != m_callbacks.end())                                        \
  {                                                                   \
    Isolate *isolate = Isolate::GetCurrent();                         \
    HandleScope scope(isolate);                                       \
    Local<Value> argv[1]{napi_create_##type##_(isolate, param)};      \
    NodeEventCallback &cb = *it->second;                              \
    cb.callback.Get(isolate)->Call(cb.js_this.Get(isolate), 1, argv); \
  }

#define MAKE_JS_CALL_2(ev, type1, param1, type2, param2)              \
  auto it = m_callbacks.find(ev);                                     \
  if (it != m_callbacks.end())                                        \
  {                                                                   \
    Isolate *isolate = Isolate::GetCurrent();                         \
    HandleScope scope(isolate);                                       \
    Local<Value> argv[2]{napi_create_##type1##_(isolate, param1),     \
                         napi_create_##type2##_(isolate, param2)};    \
    NodeEventCallback &cb = *it->second;                              \
    cb.callback.Get(isolate)->Call(cb.js_this.Get(isolate), 2, argv); \
  }

#define MAKE_JS_CALL_3(ev, type1, param1, type2, param2, type3, param3) \
  auto it = m_callbacks.find(ev);                                       \
  if (it != m_callbacks.end())                                          \
  {                                                                     \
    Isolate *isolate = Isolate::GetCurrent();                           \
    HandleScope scope(isolate);                                         \
    Local<Value> argv[3]{napi_create_##type1##_(isolate, param1),       \
                         napi_create_##type2##_(isolate, param2),       \
                         napi_create_##type3##_(isolate, param3)};      \
    NodeEventCallback &cb = *it->second;                                \
    cb.callback.Get(isolate)->Call(cb.js_this.Get(isolate), 3, argv);   \
  }

#define MAKE_JS_CALL_4(ev, type1, param1, type2, param2, type3, param3, type4, param4) \
  auto it = m_callbacks.find(ev);                                                      \
  if (it != m_callbacks.end())                                                         \
  {                                                                                    \
    Isolate *isolate = Isolate::GetCurrent();                                          \
    HandleScope scope(isolate);                                                        \
    Local<Value> argv[4]{                                                              \
        napi_create_##type1##_(isolate, param1),                                       \
        napi_create_##type2##_(isolate, param2),                                       \
        napi_create_##type3##_(isolate, param3),                                       \
        napi_create_##type4##_(isolate, param4),                                       \
    };                                                                                 \
    NodeEventCallback &cb = *it->second;                                               \
    cb.callback.Get(isolate)->Call(cb.js_this.Get(isolate), 4, argv);                  \
  }

#define MAKE_JS_CALL_5(ev, type1, param1, type2, param2, type3, param3, type4, param4, type5, param5) \
  auto it = m_callbacks.find(ev);                                                                     \
  if (it != m_callbacks.end())                                                                        \
  {                                                                                                   \
    Isolate *isolate = Isolate::GetCurrent();                                                         \
    HandleScope scope(isolate);                                                                       \
    Local<Value> argv[5]{                                                                             \
        napi_create_##type1##_(isolate, param1),                                                      \
        napi_create_##type2##_(isolate, param2),                                                      \
        napi_create_##type3##_(isolate, param3),                                                      \
        napi_create_##type4##_(isolate, param4),                                                      \
        napi_create_##type5##_(isolate, param5),                                                      \
    };                                                                                                \
    NodeEventCallback &cb = *it->second;                                                              \
    cb.callback.Get(isolate)->Call(cb.js_this.Get(isolate), 5, argv);                                 \
  }

#define MAKE_JS_CALL_6(ev, type1, param1, type2, param2, type3, param3, type4, param4, type5, param5, type6, param6) \
  auto it = m_callbacks.find(ev);                                                                                    \
  if (it != m_callbacks.end())                                                                                       \
  {                                                                                                                  \
    Isolate *isolate = Isolate::GetCurrent();                                                                        \
    HandleScope scope(isolate);                                                                                      \
    Local<Value> argv[6]{                                                                                            \
        napi_create_##type1##_(isolate, param1),                                                                     \
        napi_create_##type2##_(isolate, param2),                                                                     \
        napi_create_##type3##_(isolate, param3),                                                                     \
        napi_create_##type4##_(isolate, param4),                                                                     \
        napi_create_##type5##_(isolate, param5),                                                                     \
        napi_create_##type6##_(isolate, param6),                                                                     \
    };                                                                                                               \
    NodeEventCallback &cb = *it->second;                                                                             \
    cb.callback.Get(isolate)->Call(cb.js_this.Get(isolate), 6, argv);                                                \
  }

#define COMMAND_JS_CALLBACK_SUCCESS(cid)                                   \
  auto it = m_commands.find(cid);                                          \
  if (it != m_commands.end())                                              \
  {                                                                        \
    Isolate *isolate = Isolate::GetCurrent();                              \
    HandleScope scope(isolate);                                            \
    CommandPrivData &pdata = *it->second;                                  \
    NodeEventCallback *cb = pdata.success;                                 \
    cb->callback.Get(isolate)->Call(cb->js_this.Get(isolate), 0, nullptr); \
  }

#define COMMAND_JS_CALLBACK_FAIL(cid, type1, param1, type2, param2)     \
  auto it = m_commands.find(cid);                                       \
  if (it != m_commands.end())                                           \
  {                                                                     \
    Isolate *isolate = Isolate::GetCurrent();                           \
    HandleScope scope(isolate);                                         \
    Local<Value> argv[2]{napi_create_##type1##_(isolate, param1),       \
                         napi_create_##type2##_(isolate, param2)};      \
    CommandPrivData &pdata = *it->second;                               \
    NodeEventCallback *cb = pdata.fail;                                 \
    cb->callback.Get(isolate)->Call(cb->js_this.Get(isolate), 2, argv); \
  }

namespace agora
{
namespace lb_linux_sdk
{
void RtmServerController::Init(Local<Object> exports)
{
  Nan::HandleScope scope;

  // Prepare constructor template
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
  tpl->SetClassName(Nan::New("RtmServerController").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  Nan::SetPrototypeMethod(tpl, "initialize", initialize);
  Nan::SetPrototypeMethod(tpl, "login", login);
  Nan::SetPrototypeMethod(tpl, "logout", logout);
  Nan::SetPrototypeMethod(tpl, "sendMessageToPeer", sendMessageToPeer);
  Nan::SetPrototypeMethod(tpl, "createChannel", createChannel);
  Nan::SetPrototypeMethod(tpl, "onEvent", onEvent);
  Nan::SetPrototypeMethod(tpl, "setParameters", setParameters);

  constructor.Reset(tpl->GetFunction());
  exports->Set(Nan::New("RtmServerController").ToLocalChecked(), tpl->GetFunction());
}

void RtmServerController::New(const Nan::FunctionCallbackInfo<Value> &args)
{
  RtmServerController *obj = new RtmServerController();
  obj->Wrap(args.This());
  args.GetReturnValue().Set(args.This());
}

void RtmServerController::initialize(const Nan::FunctionCallbackInfo<v8::Value> &args)
{
  NodeString app_id, user_id, token;
  uint32_t cid;
  Local<Function> success, fail;
  napi_get_value_nodestring_(args[0], app_id);

  Isolate *isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);
  RtmServerController *instance = ObjectWrap::Unwrap<RtmServerController>(args.Holder());
  int result = instance->controller_->initialize(app_id, instance);
  napi_set_int_result(args, result);
}

void RtmServerController::login(const Nan::FunctionCallbackInfo<v8::Value> &args)
{
  NodeString user_id, token;
  uint32_t cid;
  Local<Function> success, fail;
  napi_get_value_nodestring_(args[1], user_id);

  Isolate *isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);
  RtmServerController *instance = ObjectWrap::Unwrap<RtmServerController>(args.Holder());

  bool result = true;
  if(args[0]->IsNull()) {
    result = instance->controller_->login(NULL, user_id);
  } else {
    napi_get_value_nodestring_(args[0], token);
    result = instance->controller_->login(token, user_id);
  }

  napi_set_int_result(args, result);
}

void RtmServerController::logout(const Nan::FunctionCallbackInfo<v8::Value> &args)
{
  do
  {
    Isolate *isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);
    RtmServerController *instance = ObjectWrap::Unwrap<RtmServerController>(args.Holder());
    bool result = instance->controller_->logout();
    napi_set_bool_result(args, result);
  } while (false);
}

void RtmServerController::sendMessageToPeer(const Nan::FunctionCallbackInfo<v8::Value> &args)
{
  do
  {
    NodeString peer_id, message;
    napi_get_value_nodestring_(args[0], peer_id);
    napi_get_value_nodestring_(args[1], message);
    Isolate *isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);
    RtmServerController *instance = ObjectWrap::Unwrap<RtmServerController>(args.Holder());
    agora::rtm::IMessage *rtm_message = instance->controller_->createMessage();
    rtm_message->setText(message);
    bool result = instance->controller_->sendMessageToPeer(peer_id, rtm_message);
    napi_set_bool_result(args, result);
  } while (false);
}

void RtmServerController::setParameters(const Nan::FunctionCallbackInfo<v8::Value> &args)
{
  do
  {
    NodeString params;
    napi_get_value_nodestring_(args[0], params);
    Isolate *isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);
    RtmServerController *instance = ObjectWrap::Unwrap<RtmServerController>(args.Holder());
    int result = instance->controller_->setParameters(params);
    napi_set_int_result(args, result);
  } while (false);
}

RtmChannel *RtmServerController::getChannel(std::string &cname)
{
  auto it = m_channels.find(cname);
  if (it != m_channels.end())
  {
    return it->second;
  }
  return NULL;
}

void RtmServerController::createChannel(const Nan::FunctionCallbackInfo<v8::Value> &args)
{
  do
  {
    NodeString cname;
    napi_get_value_nodestring_(args[0], cname);
    Isolate *isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);
    RtmServerController *instance = ObjectWrap::Unwrap<RtmServerController>(args.Holder());
    RtmChannelEventHandler *handler = new RtmChannelEventHandler();
    agora::rtm::IChannel *channel = instance->controller_->createChannel(cname, handler);
    if(channel == nullptr) {
      args.GetReturnValue().Set(Nan::Null());
      break;
    }
    Local<Object> js = RtmChannel::generateJSInstance(instance->controller_, channel, handler);
    args.GetReturnValue().Set(js);
  } while (false);
}

void RtmServerController::onEvent(const Nan::FunctionCallbackInfo<v8::Value> &args)
{
  do
  {
    RtmServerController *instance = ObjectWrap::Unwrap<RtmServerController>(args.Holder());
    NodeString eventName;
    napi_status status = napi_get_value_nodestring_(args[0], eventName);
    // std::cout << eventName << std::endl;
    if (!args[1]->IsFunction())
    {
      cout << "Function expected" << endl;
      break;
    }

    Local<Function> callback = args[1].As<Function>();

    if (callback.IsEmpty())
    {
      cout << "Function expected" << endl;
      break;
    }

    Persistent<Function> persist;
    persist.Reset(args.GetIsolate(), callback);
    Local<Object> obj = args.This();
    Persistent<Object> persistObj;
    persistObj.Reset(args.GetIsolate(), obj);
    instance->addEventHandler((char *)eventName, persistObj, persist);
  } while (false);
}

void RtmServerController::addEventHandler(const std::string &eventName, Persistent<Object> &obj, Persistent<Function> &callback)
{
  NodeEventCallback *cb = new NodeEventCallback();
  cb->js_this.Reset(Isolate::GetCurrent(), obj);
  cb->callback.Reset(Isolate::GetCurrent(), callback);
  m_callbacks.emplace(eventName, cb);
}

RtmServerController::RtmServerController()
{
  agora::base::IAgoraService *agora_server = createAgoraService();
  agora_service_ = agora_server;
  agora_service_->initialize(ctx);
  agora::rtm::IRtmService *p_rs = agora_service_->createRtmService();
  // enable console and debug output
  std::ostringstream out;
  out << "{\"rtm.log_filter\": " << 0xffff << "}";
  p_rs->setParameters(out.str().c_str());
  controller_ = p_rs;
}

RtmServerController::~RtmServerController()
{
  // LbecQuit();
}

// rtm sdk callback
void RtmServerController::onLoginSuccess()
{
  agora::lb_linux_sdk::node_async_call::async_call([this]() {
    MAKE_JS_CALL_0(RTM_LOGIN_SUCESS);
  });
}

void RtmServerController::onLoginFailure(agora::rtm::LOGIN_ERR_CODE errorCode)
{
  std::cout << "log failed" << std::endl;
  agora::lb_linux_sdk::node_async_call::async_call([this, errorCode]() {
    MAKE_JS_CALL_1(RTM_LOGIN_FAILURE, int32, errorCode);
  });
}

void RtmServerController::onLogout()
{
  agora::lb_linux_sdk::node_async_call::async_call([this]() {
    MAKE_JS_CALL_0(RTM_LOGOUT);
  });
}

void RtmServerController::onConnectionStateChanged(agora::rtm::CONNECTION_STATE state)
{
  agora::lb_linux_sdk::node_async_call::async_call([this, state]() {
    MAKE_JS_CALL_1(RTM_CONNECTION_STATE_CHANGED, int32, state);
  });
}

void RtmServerController::onSendMessageResult(long long messageId, agora::rtm::PEER_MESSAGE_ERR_CODE state)
{
  agora::lb_linux_sdk::node_async_call::async_call([this, messageId, state]() {
    MAKE_JS_CALL_2(RTM_SEND_MESSAGE_STATE, int32, messageId, int32, state);
  });
}

void RtmServerController::onMessageReceivedFromPeer(const char *peerId, const agora::rtm::IMessage *message)
{
  std::string mPeerId = std::string(peerId);
  std::string mText = std::string(message->getText());
  agora::lb_linux_sdk::node_async_call::async_call([this, mPeerId, mText]() {
    MAKE_JS_CALL_2(RTM_MESSAGE_RECEIVED_FROM_PEER, string, mPeerId.c_str(), string, mText.c_str());
  });
}

RtmChannel::RtmChannel()
{
}

RtmChannel::~RtmChannel()
{
  if (channel_)
  {
    channel_->release();
  }
  if (handler_)
  {
    delete handler_;
  }
  rtm_ = nullptr;
}

Local<Object> RtmChannel::generateJSInstance(agora::rtm::IRtmService *rtm, agora::rtm::IChannel *channel, RtmChannelEventHandler *handler)
{
  Local<Object> instance;

  Nan::HandleScope scope;

  // Prepare constructor template
  Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
  tpl->SetClassName(Nan::New("RtmChannel").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  Nan::SetPrototypeMethod(tpl, "join", join);
  Nan::SetPrototypeMethod(tpl, "sendMessage", sendMessage);
  Nan::SetPrototypeMethod(tpl, "onEvent", onEvent);
  Nan::SetPrototypeMethod(tpl, "getMembers", getMembers);
  Nan::SetPrototypeMethod(tpl, "leave", leave);
  Nan::SetPrototypeMethod(tpl, "release", release);

  Local<Function> cons = tpl->GetFunction();
  Local<External> exrtm = Local<External>::New(Isolate::GetCurrent(), External::New(Isolate::GetCurrent(), rtm));
  Local<External> exch = Local<External>::New(Isolate::GetCurrent(), External::New(Isolate::GetCurrent(), channel));
  Local<External> exhandler = Local<External>::New(Isolate::GetCurrent(), External::New(Isolate::GetCurrent(), handler));
  Local<v8::Value> argv[3] = {exrtm, exch, exhandler};
  return Nan::NewInstance(cons, 3, argv).ToLocalChecked();
}

void RtmChannel::New(const Nan::FunctionCallbackInfo<Value> &args)
{
  RtmChannel *obj = new RtmChannel();
  Local<Value> instance = args[0];
  Local<Value> val = args[1];
  Local<Value> handlerVal = args[2];
  Local<External> exrtm = Local<External>::Cast(instance);
  Local<External> ex = Local<External>::Cast(val);
  Local<External> exhandler = Local<External>::Cast(handlerVal);
  agora::rtm::IRtmService *rtm = static_cast<agora::rtm::IRtmService *>(exrtm->Value());
  agora::rtm::IChannel *channel = static_cast<agora::rtm::IChannel *>(ex->Value());
  RtmChannelEventHandler *handler = static_cast<RtmChannelEventHandler *>(exhandler->Value());
  obj->channel_ = channel;
  obj->handler_ = handler;
  obj->rtm_ = rtm;
  obj->Wrap(args.This());
  args.GetReturnValue().Set(args.This());
}

void RtmChannel::join(const Nan::FunctionCallbackInfo<v8::Value> &args)
{
  int result = -1;
  do
  {
    Isolate *isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);
    RtmChannel *instance = ObjectWrap::Unwrap<RtmChannel>(args.Holder());
    CHECK_NATIVE_THIS(instance);
    result = instance->channel_->join();
  } while (false);
  napi_set_int_result(args, result);
}

void RtmChannel::leave(const Nan::FunctionCallbackInfo<v8::Value> &args)
{
  int result = -1;
  do
  {
    Isolate *isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);
    RtmChannel *instance = ObjectWrap::Unwrap<RtmChannel>(args.Holder());
    CHECK_NATIVE_THIS(instance);
    result = instance->channel_->leave();
  } while (false);
  napi_set_int_result(args, result);
}

void RtmChannel::release(const Nan::FunctionCallbackInfo<v8::Value> &args)
{
  int result = -1;
  do
  {
    Isolate *isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);
    RtmChannel *instance = ObjectWrap::Unwrap<RtmChannel>(args.Holder());
    CHECK_NATIVE_THIS(instance);
    instance->channel_->release();
    result = 0;
  } while (false);
  napi_set_int_result(args, result);
}

void RtmChannel::getMembers(const Nan::FunctionCallbackInfo<v8::Value> &args)
{
  int result = -1;
  do
  {
    Isolate *isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);
    RtmChannel *instance = ObjectWrap::Unwrap<RtmChannel>(args.Holder());
    CHECK_NATIVE_THIS(instance);
    result = instance->channel_->getMembers();
  } while (false);
  napi_set_int_result(args, result);
}

void RtmChannel::sendMessage(const Nan::FunctionCallbackInfo<v8::Value> &args)
{
  int result = -1;
  do
  {
    NodeString message;
    napi_get_value_nodestring_(args[0], message);

    Isolate *isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);
    RtmChannel *instance = ObjectWrap::Unwrap<RtmChannel>(args.Holder());
    CHECK_NATIVE_THIS(instance);
    agora::rtm::IMessage *rtm_message = instance->rtm_->createMessage();
    CHECK_NATIVE_THIS(rtm_message);
    rtm_message->setText(message);
    result = instance->channel_->sendMessage(rtm_message);
  } while (false);
  napi_set_int_result(args, result);
}

void RtmChannel::onEvent(const Nan::FunctionCallbackInfo<v8::Value> &args)
{
  do
  {
    RtmChannel *instance = ObjectWrap::Unwrap<RtmChannel>(args.Holder());
    NodeString eventName;
    napi_status status = napi_get_value_nodestring_(args[0], eventName);
    // std::cout << eventName << std::endl;
    if (!args[1]->IsFunction())
    {
      cout << "Function expected" << endl;
      break;
    }

    Local<Function> callback = args[1].As<Function>();

    if (callback.IsEmpty())
    {
      cout << "Function expected" << endl;
      break;
    }

    Persistent<Function> persist;
    persist.Reset(args.GetIsolate(), callback);
    Local<Object> obj = args.This();
    Persistent<Object> persistObj;
    persistObj.Reset(args.GetIsolate(), obj);
    instance->handler_->addEventHandler((char *)eventName, persistObj, persist);
  } while (false);
}

} // namespace lb_linux_sdk
} // namespace agora
