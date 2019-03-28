#ifndef AGORA_RTM_CHANNEL_EVENT_HANDLER_H
#define AGORA_RTM_CHANNEL_EVENT_HANDLER_H

#include <unordered_map>
#include <string>
#include <uv.h>
#include "node_napi_api.h"
#include "rtm_controller.h"
#include "IAgoraService.h"
#include "IAgoraRtmService.h"
namespace agora {
    namespace lb_linux_sdk {
        class RtmChannelEventHandler : public agora::rtm::IChannelEventHandler
        {
#define RTM_CHANNEL_JOIN_SUCESS "JoinSuccess"
#define RTM_CHANNEL_JOIN_FAILURE "JoinFailure"
#define RTM_CHANNEL_SEND_MESSAGE_STATE "SendMessageState"
#define RTM_CHANNEL_MESSAGE_RECEIVED "MessageReceived"
#define RTM_CHANNEL_MEMBER_JOINED "MemberJoined"
#define RTM_CHANNEL_MEMBER_LEFT "MemberLeft"
#define RTM_CHANNEL_GET_MEMBERS "GetMembers"
#define RTM_CHANNEL_LEAVE_CHANNEL "LeaveChannel"
        public:
            struct NodeEventCallback
            {
                Persistent<Function> callback;
                Persistent<Object> js_this;
            };
            struct ChannelMember
            {
                std::string userId;
                std::string channelId;
            };
        public:
            RtmChannelEventHandler();
            ~RtmChannelEventHandler();
            void addEventHandler(const std::string& eventName, Persistent<Object>& obj, Persistent<Function>& callback);
            virtual void onJoinSuccess();
            virtual void onJoinFailure(agora::rtm::JOIN_CHANNEL_ERR errorCode);
            virtual void onSendMessageState(long long messageId, agora::rtm::CHANNEL_MESSAGE_ERR_CODE state);
            virtual void onMessageReceived(const char *userId, const agora::rtm::IMessage *message);
            virtual void onMemberJoined(agora::rtm::IChannelMember *member);
            virtual void onMemberLeft(agora::rtm::IChannelMember *member);
            virtual void onGetMembers(agora::rtm::IChannelMember **members, int userCount, agora::rtm::GET_MEMBERS_ERR errorCode);
            virtual void onLeave(agora::rtm::LEAVE_CHANNEL_ERR errorCode);
        public:
            RtmChannel* m_channel;
        private:
            std::unordered_map<std::string, NodeEventCallback*> m_callbacks;
        };
    }
}

#endif