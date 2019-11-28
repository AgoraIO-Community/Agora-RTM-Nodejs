const RTMServerController = require("../build/Release/agorasdk")
  .RtmServerController;

export interface RTMController {
  initialize(appId: string): number;
  login(token: string | null, uid: string): boolean;
  logout(): boolean;
  renewToken(token: string): number;
  sendMessageToPeer(peerId: string, message: string): boolean;
  createChannel(channelName: string): RTMChannel;
  setParameters(parameter: string): number;
  onEvent<E extends keyof RTMControllerEvent>(
    eventName: E,
    callback: RTMControllerEvent[E]
  ): void;
  queryPeersOnlineStatus(peerIds:string[]): number;
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
  release(): void;
}

export interface RTMControllerEvent {
  LoginSuccess: () => void;
  LoginFailure: (ecode: number) => void;
  Logout: (ecode: number) => void;
  MessageReceivedFromPeer: (peerId: string, message: string) => void;
  ConnectionStateChanged: (state: number, ecode: number) => void;
  RenewTokenResult: (token: string, ecode: number) => void;
  TokenExpired: () => void;
  SendMessageState: (messageId: number, state: number) => void;
  QueryPeersOnlineStatusResult: (requestId: number, peersStatus:any, errorCode: number) => void;
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
