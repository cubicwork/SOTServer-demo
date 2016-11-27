// task of http worker.
void SOTServer_worker(void *para) {

  // block accept
  SOTServer_blockingAcceptFlag = true;

  int32 *clientSockPointer = para;
  int32 clientSock = *clientSockPointer;

  printf("get a connection : %d\n", clientSock);

// 0 watting, 1. method get, 2. request URI get 3. finish recive 4. start send 5.send finished
  int recieveStatus = 0;
  bool cgiRequest = true;
  char recieveBuffer;

  char *httpMethod = (char *)zalloc(8);
  int httpMethodLength = 0;
  char *httpRequestUri = (char *)zalloc(64);
  int httpRequestUriLength = 0;
  char *httpStopFlag = (char *)zalloc(4);
  int httpStopFlagLength = 0;

  httpMethod[0] = 0;
  httpRequestUri[0] = 0;
  httpStopFlag[0] = 0;

  // loop for recieve data
  for(;;) {

    read(clientSock, &recieveBuffer, 1);

    if(recieveStatus == 0) {
    // get method
      if(recieveBuffer != 32) {
        httpMethod[httpMethodLength] = recieveBuffer;
        httpMethodLength ++;
      } else {
        httpMethod[httpMethodLength] = 0;
        recieveStatus = 1;
      }
      continue;
    }

    if(recieveStatus == 1) {
    // get url
      if(recieveBuffer != 32) {
        httpRequestUri[httpRequestUriLength] = recieveBuffer;
        httpRequestUriLength ++;
      } else {
        httpRequestUri[httpRequestUriLength] = 0;
        recieveStatus = 2;
        // if prefix of request uri match SOT_SERVER_CGI_PREFIX, set cgiRequest true.
        int charIndex = 0;
        for(charIndex; charIndex < strlen(SOT_SERVER_CGI_PREFIX); charIndex++) {
          if(httpRequestUri[charIndex] != SOT_SERVER_CGI_PREFIX[charIndex]) {
            cgiRequest = cgiRequest && false;
          }
        }
      }
      continue;
    }

    if(recieveStatus == 2) {
      if(recieveBuffer == 10 || recieveBuffer == 13) {

        httpStopFlag[httpStopFlagLength] = recieveBuffer;
        httpStopFlagLength ++;
        httpStopFlag[httpStopFlagLength] = 0;
        if(httpStopFlag[0] == 13 && httpStopFlag[1] == 10 &&  httpStopFlag[2] == 13 &&  httpStopFlag[3] == 10) {
          recieveStatus == 3;
          break;
        }
      } else {
        httpStopFlagLength = 0;
        httpStopFlag[httpStopFlagLength] = 0;
      }
      continue;
    }
  }
  // loop for send data
  printf("Method=%s SOCK=%d\n", httpMethod, clientSock);
  printf("URI=%s SOCK=%d\n", httpRequestUri, clientSock);
  printf("CGIRequestFlag=%d SOCK=%d\n", cgiRequest, clientSock);

  if(cgiRequest) {
    // handled by cgi handler
    SOTCGI_handler(httpRequestUri, clientSock);

  } else {
    // handled by static file handler
    // if request uri is a path, request default document.
    if(httpRequestUri[httpRequestUriLength - 1] == 47) {
      sprintf(httpRequestUri, "%s%s", httpRequestUri, SOT_SERVER_DEFAULT_DOCUMENT);
    }

    // open request file and try to get the content
    SOTROM_file *fileResource = SOTROM_fopen(httpRequestUri);
    if(fileResource->fileExsit) {
      // make 200 response
      write(clientSock, SOT_SERVER_HEADER_200, strlen(SOT_SERVER_HEADER_200));
      write(clientSock, SOT_SERVER_HEADER_SERVER, strlen(SOT_SERVER_HEADER_SERVER));
      write(clientSock, SOT_SERVER_HEADER_MIME, strlen(SOT_SERVER_HEADER_MIME));
      write(clientSock, fileResource->mime, strlen(fileResource->mime));
      write(clientSock, SOT_SERVER_HEADER_FINISH, strlen(SOT_SERVER_HEADER_FINISH));
      write(clientSock, SOT_SERVER_HEADER_FINISH, strlen(SOT_SERVER_HEADER_FINISH));

      char *fileBuffer = (char *)zalloc(65);
      while(SOTROM_fread(fileResource, (uint32 *)fileBuffer, 64)) {
        write(clientSock, fileBuffer, strlen(fileBuffer));
      }
      free(fileBuffer);
      free(fileResource);

    } else {
      // make 404 response
      write(clientSock, SOT_SERVER_HEADER_404, strlen(SOT_SERVER_HEADER_200));
      write(clientSock, SOT_SERVER_HEADER_SERVER, strlen(SOT_SERVER_HEADER_SERVER));
      write(clientSock, SOT_SERVER_HEADER_TEXT, strlen(SOT_SERVER_HEADER_TEXT));
      write(clientSock, SOT_SERVER_HEADER_FINISH, strlen(SOT_SERVER_HEADER_FINISH));
      write(clientSock, "404 Not Found", 13);
    }
  }

  free(httpMethod);
  free(httpRequestUri);

  // stop blocking accept
  for( ;; ) {
    close(clientSock);
    vTaskDelay(50 / portTICK_RATE_MS);
    SOTServer_blockingAcceptFlag = false;
    vTaskDelete(NULL);
  }
}
