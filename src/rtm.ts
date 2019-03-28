const RTMServerController = require("../build/Release/agorasdk")
  .RtmServerController;

export interface RTMController {
  login(appId: string, uid: string): boolean;
  logout(): boolean;
  sendMessageToPeer(peerId: string, message: string): boolean;
  createChannel(channelName: string): RTMChannel;
  onEvent<E extends keyof RTMControllerEvent>(
    eventName: E,
    callback: RTMControllerEvent[E]
  ): void;
}

export interface RTMChannel {
  join(): number;
  leave(): number;
  sendMessage(message: string): number;
  getMembers(): number;
  onEvent<E extends keyof RTMChannelEvent>(
    eventName: E,
    callback: RTMChannelEvent[E]
  ): void;
}

export interface RTMControllerEvent {
  LoginSuccess: () => void;
  LoginFailure: (ecode: number) => void;
  Logout: () => void;
  MessageReceivedFromPeer: (peerId: string, message: string) => void;
  ConnectionStateChanged: (state: number) => void;
  SendMessageState: (messageId: number, state: number) => void;
}

export interface RTMChannelEvent {
  JoinSuccess: () => void;
  JoinFailure: (ecode: number) => void;
  LeaveChannel: (ecode: number) => void;
  SendMessageState: (messageId: string, state: number) => void;
  MessageReceived: (peerId: string, message: string) => void;
  MemberJoined: (peerId: string, channelName: string) => void;
  MemberLeft: (peerId: string, channelName: string) => void;
  GetMembers: (members: [], ecode: number) => void;
}

export const createInstance = (): RTMController => {
  return new RTMServerController();
};
