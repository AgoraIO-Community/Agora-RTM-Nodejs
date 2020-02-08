#include "rtm_channel_event_handler.h"
#include <stdio.h>
#include "node_uid.h"
#include "uv.h"
#include "node_async_queue.h"
#include "log.h"

namespace agora {
    namespace lb_linux_sdk {

        RtmChannelEventHandler::RtmChannelEventHandler()
        {
        }

        RtmChannelEventHandler::~RtmChannelEventHandler() 
        {
            for (auto& handler : m_callbacks) {
                delete handler.second;
            }
        }

#define MAKE_JS_CALL_0(ev) \
        auto it = m_callbacks.find(ev); \
        if (it != m_callbacks.end()) {\
            Isolate *isolate = Isolate::GetCurrent();\
            HandleScope scope(isolate);\
            NodeEventCallback& cb = *it->second;\
            cb.callback.Get(isolate)->Call(cb.js_this.Get(isolate), 0, nullptr);\
        }

#define MAKE_JS_CALL_1(ev, type, param) \
        auto it = m_callbacks.find(ev); \
        if (it != m_callbacks.end()) {\
            Isolate *isolate = Isolate::GetCurrent();\
            HandleScope scope(isolate);\
            Local<Value> argv[1]{ napi_create_##type##_(isolate, param)\
                                };\
            NodeEventCallback& cb = *it->second;\
            cb.callback.Get(isolate)->Call(cb.js_this.Get(isolate), 1, argv);\
        }

#define MAKE_JS_CALL_2(ev, type1, param1, type2, param2) \
        auto it = m_callbacks.find(ev); \
        if (it != m_callbacks.end()) {\
            Isolate *isolate = Isolate::GetCurrent();\
            HandleScope scope(isolate);\
            Local<Value> argv[2]{ napi_create_##type1##_(isolate, param1),\
                                  napi_create_##type2##_(isolate, param2)\
                                };\
            NodeEventCallback& cb = *it->second;\
            cb.callback.Get(isolate)->Call(cb.js_this.Get(isolate), 2, argv);\
        }

#define MAKE_JS_CALL_3(ev, type1, param1, type2, param2, type3, param3) \
        auto it = m_callbacks.find(ev); \
        if (it != m_callbacks.end()) {\
            Isolate *isolate = Isolate::GetCurrent();\
            HandleScope scope(isolate);\
            Local<Value> argv[3]{ napi_create_##type1##_(isolate, param1),\
                                  napi_create_##type2##_(isolate, param2),\
                                  napi_create_##type3##_(isolate, param3) \
                                };\
            NodeEventCallback& cb = *it->second;\
            cb.callback.Get(isolate)->Call(cb.js_this.Get(isolate), 3, argv);\
        }

#define MAKE_JS_CALL_4(ev, type1, param1, type2, param2, type3, param3, type4, param4) \
        auto it = m_callbacks.find(ev); \
        if (it != m_callbacks.end()) {\
            Isolate *isolate = Isolate::GetCurrent();\
            HandleScope scope(isolate);\
            Local<Value> argv[4]{ napi_create_##type1##_(isolate, param1),\
                                  napi_create_##type2##_(isolate, param2),\
                                  napi_create_##type3##_(isolate, param3), \
                                  napi_create_##type4##_(isolate, param4), \
                                };\
            NodeEventCallback& cb = *it->second;\
            cb.callback.Get(isolate)->Call(cb.js_this.Get(isolate), 4, argv);\
        }

#define MAKE_JS_CALL_5(ev, type1, param1, type2, param2, type3, param3, type4, param4, type5, param5) \
        auto it = m_callbacks.find(ev); \
        if (it != m_callbacks.end()) {\
            Isolate *isolate = Isolate::GetCurrent();\
            HandleScope scope(isolate);\
            Local<Value> argv[5]{ napi_create_##type1##_(isolate, param1),\
                                  napi_create_##type2##_(isolate, param2),\
                                  napi_create_##type3##_(isolate, param3), \
                                  napi_create_##type4##_(isolate, param4), \
                                  napi_create_##type5##_(isolate, param5), \
                                };\
            NodeEventCallback& cb = *it->second;\
            cb.callback.Get(isolate)->Call(cb.js_this.Get(isolate), 5, argv);\
        }

        void RtmChannelEventHandler::addEventHandler(const std::string& eventName, Persistent<Object>& obj, Persistent<Function>& callback)
        {
            NodeEventCallback *cb = new NodeEventCallback();
            cb->js_this.Reset(Isolate::GetCurrent(), obj);
            cb->callback.Reset(Isolate::GetCurrent(), callback);
            m_callbacks.emplace(eventName, cb);
        }

        void RtmChannelEventHandler::onJoinSuccess()
        {
            agora::lb_linux_sdk::node_async_call::async_call([this]() {
                MAKE_JS_CALL_0(RTM_CHANNEL_JOIN_SUCESS);
            });
        }

        void RtmChannelEventHandler::onJoinFailure(agora::rtm::JOIN_CHANNEL_ERR errorCode)
        {
            agora::lb_linux_sdk::node_async_call::async_call([this, errorCode]() {
                MAKE_JS_CALL_1(RTM_CHANNEL_JOIN_FAILURE, int32, errorCode);
            });
        }

        void RtmChannelEventHandler::onSendMessageResult(long long messageId, agora::rtm::CHANNEL_MESSAGE_ERR_CODE state)
        {
            agora::lb_linux_sdk::node_async_call::async_call([this, messageId, state]() {
                MAKE_JS_CALL_2(RTM_CHANNEL_SEND_MESSAGE_STATE, uint64, messageId, int32, state);
            });
        }

        void RtmChannelEventHandler::onMessageReceived(const char *userId, const agora::rtm::IMessage *message)
        {
            std::string mUserId(userId);
            std::string mText(message->getText());
            agora::lb_linux_sdk::node_async_call::async_call([this, mUserId, mText]() {
                MAKE_JS_CALL_2(RTM_CHANNEL_MESSAGE_RECEIVED, string, mUserId.c_str(), string, mText.c_str());
            });
        }

        void RtmChannelEventHandler::onMemberJoined(agora::rtm::IChannelMember *member) {
            LOG2(Info, "onMemberJoined %s %s", member->getUserId(), member->getChannelId());
            
            std::string mUserId(member->getUserId());
            std::string mChannelId(member->getChannelId());
            
            agora::lb_linux_sdk::node_async_call::async_call([this, mUserId, mChannelId]() {
                LOG2(Info, "async onMemberJoined %s %s", mUserId.c_str(), mChannelId.c_str());
                MAKE_JS_CALL_2(RTM_CHANNEL_MEMBER_JOINED, string, mUserId.c_str(), string, mChannelId.c_str());
            });
        }

        void RtmChannelEventHandler::onMemberLeft(agora::rtm::IChannelMember *member) {
            LOG2(Info, "onMemberLeft %s %s", member->getUserId(), member->getChannelId());
            std::string mUserId(member->getUserId());
            std::string mChannelId(member->getChannelId());

            agora::lb_linux_sdk::node_async_call::async_call([this, mUserId, mChannelId]() {
                LOG2(Info, "async onMemberLeft %s %s", mUserId.c_str(), mChannelId.c_str());
                MAKE_JS_CALL_2(RTM_CHANNEL_MEMBER_LEFT, string, mUserId.c_str(), string, mChannelId.c_str());
            });
        }

        void RtmChannelEventHandler::onGetMembers(agora::rtm::IChannelMember **members, int userCount, agora::rtm::GET_MEMBERS_ERR errorCode) {
            ChannelMember* users = new ChannelMember[userCount];
            for(int i = 0; i < userCount; i++) {
                agora::rtm::IChannelMember* member = members[i];
                std::string sUserId(member->getUserId());
                std::string sChannelId(member->getChannelId());
                users[i].userId = sUserId;
                users[i].channelId = sChannelId;
            }
            agora::lb_linux_sdk::node_async_call::async_call([this, users, userCount, errorCode]() {
                Isolate *isolate = Isolate::GetCurrent();
                HandleScope scope(isolate);
                Local<Array> array = Array::New(isolate, userCount);
                for(int i = 0; i < userCount; i++) {
                    array->Set(i, napi_create_string_(isolate, users[i].userId.c_str()));
                }
                Local<Value> argv[2]{ array,
                    napi_create_int32_(isolate, errorCode)
                };
                auto it = m_callbacks.find(RTM_CHANNEL_GET_MEMBERS);
                if (it != m_callbacks.end()) {
                    NodeEventCallback& cb = *it->second;
                    cb.callback.Get(isolate)->Call(cb.js_this.Get(isolate), 2, argv);
                }
                delete []users;
            });
        }
        void RtmChannelEventHandler::onLeave(agora::rtm::LEAVE_CHANNEL_ERR errorCode){
            agora::lb_linux_sdk::node_async_call::async_call([this, errorCode]() {
                MAKE_JS_CALL_1(RTM_CHANNEL_LEAVE_CHANNEL, int32, errorCode);
            });
        }
    }
}
