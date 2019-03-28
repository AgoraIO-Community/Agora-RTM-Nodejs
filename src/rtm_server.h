 #pragma once
#include<iostream>
#include<string>
#include<memory>
#include "IAgoraService.h"
#include "IAgoraRtmService.h"
namespace agora{
namespace lb_linux_sdk{
class RtmServer : public agora::rtm::IRtmServiceEventHandler {
public:
    RtmServer();
    ~RtmServer();
    int login(const std::string& appId, const std::string& userId);
    int logout();
    int sendMessageToPeer(const std::string& peerId, const std::string& message);

    void onLoginSuccess();
    void onLoginFailure(agora::rtm::LOGIN_ERR_CODE errorCode);
    void onLogout();
    void onConnectionStateChanged(agora::rtm::CONNECTION_STATE state);
    void onSendMessageState(long long messageId, agora::rtm::PEER_MESSAGE_ERR_CODE state);
    void onMessageReceivedFromPeer(const char *peerId, const agora::rtm::IMessage *message);
private:
    void initialize(const std::string& appId);
    std::unique_ptr<agora::rtm::IRtmService> m_rtmService;
    void cbPrint(const char* fmt, ...) {
        printf("\x1b[32m<< RTM async callback: ");
        va_list args;
        va_start(args, fmt);
        vprintf(fmt, args);
        va_end(args);
        printf(" >>\x1b[0m\n");
    }
};
} //space lb_linux_sdk
} //space agora
