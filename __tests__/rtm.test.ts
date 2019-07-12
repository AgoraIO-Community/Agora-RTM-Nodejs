import {createInstance, RTMChannel} from '../src'

let client = createInstance()
let channel: RTMChannel

describe('C++ Addon API Test', () => {
  it('Login', (done) => {
    client = createInstance();
    client.initialize(process.env.AGORA_APP_ID)
    client.onEvent('LoginSuccess', () => {
      expect(true);
      done();
    })
    client.login(null, 'ttyy')
  })
  it('Join Channel', (done) => {
    channel = client.createChannel('agoratest')
      
    channel.onEvent('JoinSuccess', () => {
      expect(true);
      done();
    })
    channel.join()
  })
  it('GetMembers', (done) => {
    channel.onEvent('GetMembers', members => {
      expect(true);
      done();
    })
    channel.getMembers()
  })
  it('Complex Join/Leave', (done) => {
    channel.leave();
    channel.onEvent('LeaveChannel', (e) => {
      channel = client.createChannel('agoraAnotherTest');
      channel.onEvent('JoinSuccess', () => {
        expect(true);
        done();
      })
      channel.join()
    })
  })
})