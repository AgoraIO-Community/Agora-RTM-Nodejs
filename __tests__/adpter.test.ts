import AgoraRTM from '../src'

describe('Adapter API Test', () => {
  it('Join/Leave', async () => {
    const engine = new AgoraRTM();
    engine.initialize(process.env.AGORA_APP_ID)
    await engine.login(null, 'ttyy')
    const channel = engine.createChannel('ttyy');
    await channel.join();
    await channel.leave();
    expect(true)
  })

  it('query peer status', async(done) => {
    const cb = jest.fn()
    const engine1 = new AgoraRTM();
    engine1.initialize(process.env.AGORA_APP_ID)
    await engine1.login(null, 'ttyy1')
    engine1.on('QueryPeersOnlineStatusResult', () => {
      expect(true)
      done()
    })
    engine1.queryPeersOnlineStatus(["ttyy1"])
  })

  // disable since createChannel no longer returns null for invalid channel name
  // since 1.2.0
  xit('Invalid channel', async() => {
    const cb = jest.fn()
    const engine1 = new AgoraRTM();
    engine1.initialize(process.env.AGORA_APP_ID)
    await engine1.login(null, 'ttyy1')
    const channel1 = engine1.createChannel('非法频道');
    expect(channel1).toBe(null);
  })

  it('MemberJoined', async () => {
    const cb = jest.fn()
    const engine1 = new AgoraRTM();
    engine1.initialize(process.env.AGORA_APP_ID)
    await engine1.login(null, 'ttyy1')
    const channel1 = engine1.createChannel('ttyy');
    channel1.on('MemberJoined', (cb))
    await channel1.join();
    const engine2 = new AgoraRTM();
    engine2.initialize(process.env.AGORA_APP_ID)
    await engine2.login(null, 'ttyy2')
    const channel2 = engine2.createChannel('ttyy');
    await channel2.join();
    const engine3 = new AgoraRTM();
    engine3.initialize(process.env.AGORA_APP_ID)
    await engine3.login(null, 'ttyy3')
    const channel3 = engine3.createChannel('ttyy');
    await channel3.join();
    setTimeout(() => {
      expect(cb).toBeCalledTimes(2);
    }, 3000)
  }, 15000)
})
