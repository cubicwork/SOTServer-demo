#include "SOTServer/include/SOTServer.h"

#include "SOTServer/include/SOTServer.c"
#include "SOTServer/include/SOTROM.c"
#include "SOTServer/task/SOTServer_bootstrap.c"
#include "SOTServer/task/SOTServer_httpd.c"
#include "SOTServer/task/SOTServer_worker.c"
#include "SOTServer/cgi/cgi.h"
#include "SOTServer/cgi/main.c"
#include "SOTServer/cgi/SOTCGI_PROG.c"


void SOTServer_start(void) {

  SOTServer_log("Ready to start SOTServer!");

  xTaskHandle *bootstrapTask;
  portBASE_TYPE bootstrapResult;

  bootstrapResult = xTaskCreate(SOTServer_bootstrap, "SOT:bootstrap", 256, NULL, 9, bootstrapTask);

  if(bootstrapResult != pdTRUE) {
    SOTServer_log("Server start failed!\nReset the device to restart!");
  } else {
    SOTServer_log("Server load...");
  }
}
