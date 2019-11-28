const AgoraRtmSDK = require('../dist/adapter').default

let rtm = new AgoraRtmSDK()

const run = async() => {
    rtm.initialize('aab8b8f5a8cd4469a63042fcfafe7063')
    await rtm.login(null, 'test')
    console.log(`channel joined`)
    rtm.on('QueryPeersOnlineStatusResult', (requestId, status, errorCode) => {
        console.log(`${requestId} ${status} ${errorCode}`)
    })
    rtm.queryPeersOnlineStatus(["ttyy1"])
    setInterval(() => {}, 1000000)
}

run()