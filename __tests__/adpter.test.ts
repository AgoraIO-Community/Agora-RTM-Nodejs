import AgoraRTM from '../src'

describe('Adapter API Test', () => {
  it('Join/Leave', async () => {
    const engine = new AgoraRTM();
    await engine.login(process.env.AGORA_APP_ID, 'ttyy')
    const channel = engine.createChannel('ttyy');
    await channel.join();
    await channel.leave();
    expect(true)
  })

  it('MemberJoined', async () => {
    const cb = jest.fn()
    const engine1 = new AgoraRTM();
    await engine1.login(process.env.AGORA_APP_ID, 'ttyy1')
    const channel1 = engine1.createChannel('ttyy');
    channel1.on('MemberJoined', (cb))
    await channel1.join();
    const engine2 = new AgoraRTM();
    await engine2.login(process.env.AGORA_APP_ID, 'ttyy2')
    const channel2 = engine2.createChannel('ttyy');
    await channel2.join();
    const engine3 = new AgoraRTM();
    await engine3.login(process.env.AGORA_APP_ID, 'ttyy3')
    const channel3 = engine3.createChannel('ttyy');
    await channel3.join();
    setTimeout(() => {
      expect(cb).toBeCalledTimes(2);
    }, 3000)
  }, 15000)
})
