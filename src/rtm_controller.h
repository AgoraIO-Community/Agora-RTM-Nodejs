// Copyright (c) 2017-2017 Agora.io, Inc.
// server streaming sdk module

#pragma once  // NOLINT(build/header_guard)

#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <event2/event.h>

#include <map>
#include <string>
#include <thread>
#include <utility>
#include <vector>
#include <unordered_map>
#include <uuid/uuid.h>
#include <nan.h>
#include <node.h>
#include <iostream>
#include <node_object_wrap.h>
#include "IAgoraService.h"
#include "IAgoraRtmService.h"

using v8::Persistent;
using v8::Function;
using v8::Object;
using v8::Local;
using std::cout;
using std::endl;

namespace agora {
namespace lb_linux_sdk {
class RtmChannelEventHandler;
class RtmChannel :  public Nan::ObjectWrap
{
  public:
    RtmChannel();
    ~RtmChannel();
    static Local<Object> generateJSInstance(agora::rtm::IRtmService* rtm, agora::rtm::IChannel* channel, RtmChannelEventHandler* handler);
    static void New(const Nan::FunctionCallbackInfo<v8::Value> &args);
    static void join(const Nan::FunctionCallbackInfo<v8::Value> &args);
    static void leave(const Nan::FunctionCallbackInfo<v8::Value> &args);
    static void sendMessage(const Nan::FunctionCallbackInfo<v8::Value> &args);
    static void onEvent(const Nan::FunctionCallbackInfo<v8::Value> &args);
    static void getMembers(const Nan::FunctionCallbackInfo<v8::Value> &args);

  public:
    agora::rtm::IChannel* channel_;
    RtmChannelEventHandler* handler_;
    agora::rtm::IRtmService* rtm_;
};

class RtmServerController : public Nan::ObjectWrap,
  public agora::rtm::IRtmServiceEventHandler {
#define RTM_LOGIN_SUCESS "LoginSuccess"
#define RTM_LOGIN_FAILURE "LoginFailure"
#define RTM_LOGOUT "Logout"
#define RTM_CONNECTION_STATE_CHANGED "ConnectionStateChanged"
#define RTM_SEND_MESSAGE_STATE "SendMessageState"
#define RTM_MESSAGE_RECEIVED_FROM_PEER "MessageReceivedFromPeer"
#define RTM_MESSAGE_RECEIVED_FROM_PEER "MessageReceivedFromPeer"
 public:
  struct NodeEventCallback
  {
    Persistent<Function> callback;
    Persistent<Object> js_this;
  };
  struct CommandPrivData
  {
    NodeEventCallback* success;
    NodeEventCallback* fail;
    std::string uuid;

    ~CommandPrivData(){
      delete success;
      delete fail;
    }
  };
  RtmChannel* getChannel(std::string& cname);
  void addEventHandler(const std::string& eventName, Persistent<Object>& obj, Persistent<Function>& callback);
  static void Init(v8::Local<v8::Object> exports);
  static void New(const Nan::FunctionCallbackInfo<v8::Value> &info);
  static void login(
      const Nan::FunctionCallbackInfo<v8::Value> &args);
  static void logout(
      const Nan::FunctionCallbackInfo<v8::Value> &args);
  static void sendMessageToPeer(
      const Nan::FunctionCallbackInfo<v8::Value> &args);
  static void onEvent(
      const Nan::FunctionCallbackInfo<v8::Value> &args);
  static void createChannel(
      const Nan::FunctionCallbackInfo<v8::Value> &args);

 private:
  RtmServerController();
  virtual ~RtmServerController();

 public:
  virtual void onLoginSuccess();
  virtual void onLoginFailure(agora::rtm::LOGIN_ERR_CODE errorCode);
  virtual void onLogout();
  virtual void onConnectionStateChanged(agora::rtm::CONNECTION_STATE state);
  virtual void onSendMessageState(long long messageId, agora::rtm::PEER_MESSAGE_ERR_CODE state);
  virtual void onMessageReceivedFromPeer(const char *peerId, const agora::rtm::IMessage *message);
 public:
  std::unordered_map<std::string, RtmChannel*> m_channels;
 private:
  agora::base::AgoraServiceContext ctx;
  agora::base::IAgoraService* agora_service_;
  agora::rtm::IRtmService* controller_;
  std::unordered_map<std::string, NodeEventCallback*> m_callbacks;
  std::unordered_map<std::string, CommandPrivData*> m_commands;
};
  /*
  * to return int value for JS call.
  */
  #define napi_set_int_result(args, result) (args).GetReturnValue().Set(Integer::New(args.GetIsolate(), (result)))

  /**
  * to return bool value for JS call
  */
  #define napi_set_bool_result(args, result) (args).GetReturnValue().Set(v8::Boolean::New(args.GetIsolate(), (result)))


  /*
  * to return string value for JS call
  */
  #define napi_set_string_result(args, data) \
      Local<Value> tmp = String::NewFromUtf8(args.GetIsolate(), data, NewStringType::kInternalized).ToLocalChecked();\
      args.GetReturnValue().Set(tmp);

}  //lb_linux_sdk
}  //agora
