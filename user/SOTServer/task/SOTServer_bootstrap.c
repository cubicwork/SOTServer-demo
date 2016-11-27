/*
* This file use to start SOTServer
* the start sequence follows the list below:
* 1. start a listening task to listen net state.
* 2. when net is connected start to create a local soket for inconming connection.
* 3. bind listening when a socket is ready.
* 4. start task to listening connection events.
*/

void SOTServer_bootstrap(void *para) {
  bool netConnected = FALSE;
  xTaskHandle *bindTask;
  portBASE_TYPE bindResult;

  for(;;) {
    netConnected = SOTServer_netConnected();
    if(netConnected){
      bindResult = xTaskCreate(SOTServer_bindListening, "SOT:bind", 256, NULL, 9, bindTask);
      if(bindResult != pdTRUE) {
        SOTServer_log("Server start failed!\nReset the device to restart!");
      } else {
        SOTServer_log("Start server ...");
      }
      vTaskDelete(NULL);
    } else {
      vTaskDelay(1000/portTICK_RATE_MS);
    }
  }
}


void SOTServer_bindListening(void *para) {

  int32 listenfd;
  int32 ret;
//  xTaskHandle *httpdTask;
  portBASE_TYPE httpdResult;
  struct sockaddr_in server_addr;
  int stack_counter=0;
  /* Construct local address structure */
  memset(&server_addr, 0, sizeof(server_addr)); /* Zero out structure */
  server_addr.sin_family = AF_INET; /* Internet address family */
  server_addr.sin_addr.s_addr = INADDR_ANY; /* Any incoming interface */
  server_addr.sin_len = sizeof(server_addr);
  server_addr.sin_port = htons(SOT_SERVER_PORT); /* Local port */
  /* Create socket for incoming connections */

  /* Create socket for incoming connections */
  do {
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd == -1) {
      SOTServer_log("Socket error, Next attempt will be made in 1 second!");
      vTaskDelay(1000 / portTICK_RATE_MS);
    }
  } while (listenfd == -1);
  SOTServer_log("Socket created!");

  /* Bind to the local port */
  do{
    ret = bind(listenfd, (struct sockaddr *)&server_addr,
    sizeof(server_addr));
    if (ret != 0) {
      SOTServer_log("Bind fail, Next attempt will be made in 1 second!");
      vTaskDelay(1000/portTICK_RATE_MS);
    }
  } while (ret != 0);
  SOTServer_log("Socket created!");
  printf("SOTServer: Bind on port:%d\n", ntohs(server_addr.sin_port));

  /* Listen to the local connection */
  do{
    ret = listen(listenfd, SOT_SERVER_MAX_CONNECTIONS);
    if (ret != 0) {
      SOTServer_log("Listening fail, Next attempt will be made in 1 second!");
      vTaskDelay(1000/portTICK_RATE_MS);
    }
  } while (ret != 0);
  SOTServer_log("Starting Listening ...");

  printf("fd:%d\n", listenfd);

  httpdResult = xTaskCreate(SOTServer_httpd, "SOT:httpd", 256, (void*)&listenfd, 9, NULL);

  if(httpdResult != pdTRUE) {
    SOTServer_log("Server start failed!\nReset the device to restart!");
  } else {
    SOTServer_log("Getting ready for connection ...");
  }

  vTaskDelete(NULL);
}
