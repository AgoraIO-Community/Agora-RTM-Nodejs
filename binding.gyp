{
  "targets": [{
    "target_name": "agorasdk",
    "sources": [
      "addon/node_uid.h",
      "addon/node_uid.cpp",
      "addon/node_napi_api.h",
      "addon/node_napi_api.cpp",
      "addon/node_async_queue.cpp",
      "addon/node_async_queue.h",
      "addon/rtm_controller.cc",
      "addon/addon.cc",
      "addon/rtm_channel_event_handler.cc"
    ],
    "include_dirs": ["<!(node -e \"require('nan')\")", "include/"],
    "libraries": ["-Wl,-rpath,lib/libagora_rtm_sdk.so", "-lagora_rtm_sdk", "-lpthread"],
    "cflags": ["-std=c++11"]
  }]
}
