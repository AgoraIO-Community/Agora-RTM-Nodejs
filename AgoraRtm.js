const node = require('./agorasdk')
const EventEmitter = require('events')

class AgoraRtmChannel extends EventEmitter {
    constructor(chls) {
        super()
        this.channel = chls
        this.subscribeEvents()
    }

    join() {
        return this.channel.join()
    }

    leave() {
        return this.channel.leave()
    }

    sendMessage(msg) {
        return this.channel.sendMessage(msg)
    }

    getMembers() {
        return this.channel.getMembers()
    }

    subscribeEvents() {
        let fire = (...args) => {
            setImmediate(() => {
                this.emit(...args)
            })
        }
        this.channel.onEvent('JoinSuccess', () => {
            fire('JoinSuccess')
        })
        this.channel.onEvent('SendMessageState', (msgid, state) => {
            fire('SendMessageState', msgid, state)
        })
        this.channel.onEvent('MessageReceived', (userid, msg) => {
            fire('MessageReceived', userid, msg)
        })
        this.channel.onEvent('MemberJoined', (userid, channelid) => {
            fire('MemberJoined', userid, channelid)
        })
        this.channel.onEvent('MemberLeft', (userid, channelid) => {
            fire('MemberLeft', userid, channelid)
        })
        this.channel.onEvent('GetMembers', (members, err) => {
            fire('GetMembers', members, err)
        })
        this.channel.onEvent('LeaveChannel', (err) => {
            fire('LeaveChannel', err)
        })
    }
}

class AgoraRtmSDK extends EventEmitter {
    constructor() {
        super()
        this.sdk = new node.RtmServerController()
        this.subscribeEvents()
    }

    /**
     * login
     * @param {*} appid 
     * @param {*} account 
     */
    login(appid, account) {
        return this.sdk.login(appid, account)
    }

    createChannel(cname) {
        return new AgoraRtmChannel(this.sdk.createChannel(cname))
    }

    subscribeEvents() {
        let fire = (...args) => {
            setImmediate(() => {
                this.emit(...args)
            })
        }
        this.sdk.onEvent('LoginSuccess', () => {
            fire('LoginSuccess')
        })

        this.sdk.onEvent('LoginFailure', errCode => {
            fire('LoginFailure', errCode)
        })
    }
}

module.exports = AgoraRtmSDK