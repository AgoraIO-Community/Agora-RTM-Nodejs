import EventEmitter from "events";

import * as AgoraRTM from "./rtm";
import Mutex from "./mutex";

export class AgoraRtmChannel extends EventEmitter {
  private channel: AgoraRTM.RTMChannel;
  private mutex: Mutex;
  public constructor(channel: AgoraRTM.RTMChannel) {
    super();
    this.channel = channel;
    this.mutex = new Mutex();
    this.subscribeEvents();
  }

  public fire(event: string, ...args: any[]) {
    setImmediate(() => {
      this.emit(event, ...args);
    })
  }

  public join() {
    return new Promise(async (resolve, reject) => {
      try {
        await this.mutex.wait("join");
        this.mutex.lock("join");
        this.channel.join()
        // if (!this.channel.join()) {
        //   this.mutex.unlock("join");
        //   reject(new Error(`Execute 'join' and return false`));
        // }
        this.once("JoinSuccess", () => {
          this.mutex.unlock("join");
          resolve();
        });
        this.once("JoinFailure", (ecode: number) => {
          this.mutex.unlock("join");
          reject(ecode);
        });
      } catch (err) {
        this.mutex.unlock("join");
        reject(err);
      }
    });
  }

  public leave() {
    return new Promise(async (resolve, reject) => {
      try {
        await this.mutex.wait("leave");
        this.mutex.lock("leave");
        this.channel.leave()
        // if (!this.channel.leave()) {
        //   this.mutex.unlock("leave");
        //   reject(new Error(`Execute 'leave' and return false`));
        // }
        this.once("LeaveChannel", () => {
          this.mutex.unlock("leave");
          resolve();
        });
      } catch (err) {
        this.mutex.unlock("leave");
        reject(err);
      }
    });
  }

  public release() {
    return this.channel.release();
  }

  public sendMessage(message: string) {
    return this.channel.sendMessage(message);
  }

  public getMembers() {
    return this.channel.getMembers();
  }

  private subscribeEvents = () => {
    this.channel.onEvent("JoinSuccess", () => {
      this.fire("JoinSuccess");
    });
    this.channel.onEvent("JoinFailure", (ecode: number) => {
      this.fire("JoinFailure", ecode)
    })
    this.channel.onEvent("SendMessageState", (msgid: string, state: number) => {
      this.fire("SendMessageState", msgid, state);
    });
    this.channel.onEvent("MessageReceived", (userid: string, msg: string) => {
      this.fire("MessageReceived", userid, msg);
    });
    this.channel.onEvent(
      "MemberJoined",
      (userid: string, channelid: string) => {
        this.fire("MemberJoined", userid, channelid);
      }
    );
    this.channel.onEvent("MemberLeft", (userid: string, channelid: string) => {
      this.fire("MemberLeft", userid, channelid);
    });
    this.channel.onEvent("GetMembers", (members: [], ecode: number) => {
      this.fire("GetMembers", members, ecode);
    });
    this.channel.onEvent("LeaveChannel", (ecode: number) => {
      this.fire("LeaveChannel", ecode);
    });
  };
}

class AgoraRtmSDK extends EventEmitter {
  private sdk: AgoraRTM.RTMController;
  private mutex: Mutex;
  public constructor() {
    super();
    this.sdk = AgoraRTM.createInstance();
    this.mutex = new Mutex();
    this.subscribeEvents();
  }

  public fire(event: string, ...args: any[]) {
    setImmediate(() => {
      this.emit(event, ...args);
    })
  }

  public initialize(appid: string) {
    return this.sdk.initialize(appid);
  }

  public login(token: string, account: string) {
    return new Promise(async (resolve, reject) => {
      try {
        await this.mutex.wait("login");
        this.mutex.lock("login");
        this.sdk.login(token, account)
        // if (!this.sdk.login(appid, account)) {
        //   this.mutex.unlock("login");
        //   reject(new Error(`Execute 'login' and return false`));
        // }
        this.once("LoginSuccess", () => {
          this.mutex.unlock("login");
          resolve();
        });
        this.once("LoginFailure", (ecode: number) => {
          this.mutex.unlock("login");
          reject(ecode);
        });
      } catch (err) {
        this.mutex.unlock("login");
        reject(err);
      }
    });
  }

  public logout() {
    return new Promise(async (resolve, reject) => {
      try {
        await this.mutex.wait("logout");
        this.mutex.lock("logout");
        this.sdk.logout()
        // if (!this.sdk.logout()) {
        //   this.mutex.unlock("logout");
        //   reject(new Error(`Execute 'logout' and return false`));
        // }
        this.once("Logout", () => {
          this.mutex.unlock("logout");
          resolve();
        });
      } catch (err) {
        this.mutex.unlock("logout");
        reject(err);
      }
    });
  }

  public sendMessageToPeer(peerId: string, message: string) {
    return this.sdk.sendMessageToPeer(peerId, message);
  }

  public createChannel(cname: string) {
    return new AgoraRtmChannel(this.sdk.createChannel(cname));
  }

  public setParameters(param: string) {
    return this.sdk.setParameters(param);
  }

  private subscribeEvents = () => {
    this.sdk.onEvent("LoginSuccess", () => {
      this.fire("LoginSuccess");
    });

    this.sdk.onEvent("LoginFailure", (ecode: number) => {
      this.fire("LoginFailure", ecode);
    });

    this.sdk.onEvent(
      "MessageReceivedFromPeer",
      (peerId: string, message: string) => {
        this.fire("MessageReceivedFromPeer", peerId, message);
      }
    );

    this.sdk.onEvent("Logout", () => {
      this.fire("Logout");
    });

    this.sdk.onEvent("ConnectionStateChanged", (state: number) => {
      this.fire("ConnectionStateChanged", state);
    });

    this.sdk.onEvent("SendMessageState", (messageId: number, state: number) => {
      this.fire("SendMessageState", messageId, state);
    });
  };
}

export default AgoraRtmSDK;
