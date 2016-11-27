// This is main task of http server.
void SOTServer_httpd(void *para) {

  int32 *listenfdPointer = para;
  int32 listenfd = *listenfdPointer;

  int32 client_sock;
  portBASE_TYPE workerResult;
  struct sockaddr_in remote_addr;
  int32 len = sizeof(struct sockaddr_in);

  for(;;) {

    if(!SOTServer_blockingAcceptFlag) {

      if ((client_sock = accept(listenfd, (struct sockaddr *)&remote_addr, (socklen_t *)&len)) >= 0) {
        // get a connection and pass to process worker
        workerResult = xTaskCreate(SOTServer_worker, "SOT:worker", 256, (void*)&client_sock, 9, NULL);

        if(workerResult != pdTRUE) {
          SOTServer_log("Connection accept failed!\n");
        } else {
          SOTServer_log("Get a connection ...");
        }
      }
    }
    // if no incoming connection, this task will sleep 100ms
    vTaskDelay(50 / portTICK_RATE_MS);
  }
}
