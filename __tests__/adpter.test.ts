import AgoraRTM from "../src";
import { APP_ID } from "./config";

describe("Adapter API Test", () => {
  it("Join/Leave", async () => {
    const engine = new AgoraRTM();
    await engine.login(APP_ID, "ttyy");
    const channel = engine.createChannel("ttyy");
    await channel.join();
    await channel.leave();
    expect(true);
  });
});
