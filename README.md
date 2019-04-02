# Agora-RTM-Nodejs
> Use Agora RTM Linux SDK (C++) as Node Addon

## How to use
Make sure you have `node-gyp` environment.

Download Agora RTM Linux SDK and extract `libagora_rtm_sdk.so & include/` from it under this project.

If you use docker, just build image and container based on our docker file. Apart from this, you should move `libagora_rtm_sdk.so` to `/usr/lib` manaully.

```bash
npm install
```

Run command above to generate `build/ & dist/` which contains node addon and js api.

## How to run test

Get appid and set it in .env file

```bash
npm run test
```




