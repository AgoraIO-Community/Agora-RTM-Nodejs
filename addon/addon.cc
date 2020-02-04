#include <nan.h>
#include "rtm_controller.h"
#include "node_log.h"
#include "log_helper.h"

using namespace v8;
using namespace agora::lb_linux_sdk;

LogHelper g_logHelper;

void InitAll(Local<Object> exports){
  RtmServerController::Init(exports);
}

NODE_MODULE(addon, InitAll)
