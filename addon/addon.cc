#include <nan.h>
#include "rtm_controller.h"

using namespace v8;
using namespace agora::lb_linux_sdk;

void InitAll(Local<Object> exports){
  RtmServerController::Init(exports);
}

NODE_MODULE(addon, InitAll)
