#include "rtm_server.h"

namespace agora{
    namespace lb_linux_sdk{
        RtmServer::RtmServer()  { }

        RtmServer::~RtmServer() { }
        void RtmServer::initialize(const std::string& appId){
            std::cout << "123456" << std::endl;
            std::unique_ptr<agora::base::IAgoraService> agora_service(createAgoraService());
            agora::base::AgoraServiceContext ctx;
            agora_service->initialize(ctx);
            int * p1 = (int*)malloc(sizeof(int));
            free(p1);
            std::cout << "12345678" << std::endl;
            agora::rtm::IRtmService* p_rs = agora_service->createRtmService();
            m_rtmService.reset(p_rs);
            std::cout <<"qwer" << std::endl;
            m_rtmService->initialize(appId.c_str(), this);
            std::cout << "asdf" << std::endl;
            p_rs = nullptr;
        }

        int RtmServer::login(const std::string& appId, const std::string& userId){
            
            this->initialize(appId);
            int ret = m_rtmService->login(appId.c_str(), userId.c_str());
            return ret;
        }

        int RtmServer::logout(){
            int ret = m_rtmService->logout();
            return ret;
        }


        int RtmServer::sendMessageToPeer(const std::string& peerId, const std::string& message) {
            agora::rtm::IMessage* rtmMessage = agora::rtm::IMessage::createMessage();
            rtmMessage->setText(message.c_str());
            int ret = m_rtmService->sendMessageToPeer(peerId.c_str(), rtmMessage);
            rtmMessage = nullptr;
            return ret;
        }

        void RtmServer::onLoginSuccess() {
            cbPrint("on login success");
            //rtm_control_->onLoginSuccess();
        }

        void RtmServer::onLoginFailure(agora::rtm::LOGIN_ERR_CODE errorCode) {
            cbPrint("on login failure: %d\n", errorCode);
            //rtm_control_->onLoginFailure(errorCode);
        }

        void RtmServer::onLogout() {
            cbPrint("onLogout");
            //rtm_control_->onLogout();
        }

        void RtmServer::onConnectionStateChanged(agora::rtm::CONNECTION_STATE state){
            cbPrint("onConnectionStateChanged: %d", state);
            //rtm_control_->onConnectionStateChanged(state);
        }

        void RtmServer::onSendMessageState(long long messageId, agora::rtm::PEER_MESSAGE_ERR_CODE state){
            cbPrint("onSendMessageState messageID: %lld state: %d", messageId, state);
            //rtm_control_->onSendMessageState(messageId, state);
        }

        void RtmServer::onMessageReceivedFromPeer(const char *peerId, const agora::rtm::IMessage *message){
            if (!peerId || !message) {
                return;
            }
            cbPrint("onMessageReceivedFromPeer peerId: %s message: %s", peerId, message->getText());
           // rtm_control_->onMessageReceivedFromPeer(peerId, message);
        }

    } // space lb_linux_sdk

} //space agora
