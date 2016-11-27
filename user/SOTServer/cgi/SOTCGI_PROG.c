#include "driver/gpio.c"
void SOTCGI_PROG_gpio(char *para, int32 sock) {
  printf("GET CGIINST: %s\n", para);
  char *option, *funID;
  // option: get,set,clr
  // gpio: 0, 1, 2, 3, a
  PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDI_U, FUNC_GPIO12); //0
  PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTCK_U, FUNC_GPIO13); //1
  PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTMS_U, FUNC_GPIO14); //2
  PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO4_U, FUNC_GPIO4); //3
  //gpio_output_set(BIT12|BIT13|BIT14|BIT4, 0, BIT12|BIT13|BIT14|BIT4, 0);

  option = substr(para, 2, 4);
  para = substr(para, 6, 6);
  bool errorFlag = false;
  int32 gpioSet = 0x00000000;

  if(para[0] == 48 | para[0] == 49 | para[0] == 50) {
    gpioSet = 0x01 << (para[0] - 36);
  } else if (para[0] == 51) {
    gpioSet = 0x01 << 4;
  } else if (para[0] == 97) {
    gpioSet = 0x7010;
  } else {
    errorFlag = true;
  }

  if(strcmp(option, "set") == 0) {
    gpio_output_set(gpioSet, 0, 0x7010, 0);
  } else if(strcmp(option, "clr") == 0) {
    gpio_output_set(0, gpioSet, 0x7010, 0);
  } else if(strcmp(option, "get") == 0) {
    // do nothing
  } else {
    errorFlag = true;
  }
  free(option);
  free(para);

  int32 gpioStstus = gpio_input_get();
  char status[4] = "0000";
  status[0] = 48 + (gpioStstus >> 12 & 0x01);
  status[1] = 48 + (gpioStstus >> 13 & 0x01);
  status[2] = 48 + (gpioStstus >> 14 & 0x01);
  status[3] = 48 + (gpioStstus >> 4 & 0x01);

  if(errorFlag) {
    write(sock, SOT_SERVER_HEADER_500, strlen(SOT_SERVER_HEADER_500));
    write(sock, SOT_SERVER_HEADER_NOCACHE, strlen(SOT_SERVER_HEADER_NOCACHE));
    write(sock, SOT_SERVER_HEADER_SERVER, strlen(SOT_SERVER_HEADER_SERVER));
    write(sock, SOT_SERVER_HEADER_TEXT, strlen(SOT_SERVER_HEADER_TEXT));
    write(sock, SOT_SERVER_HEADER_FINISH, strlen(SOT_SERVER_HEADER_FINISH));
    write(sock, "500 Internal Server Error", 25);
  } else {
    write(sock, SOT_SERVER_HEADER_200, strlen(SOT_SERVER_HEADER_200));
    write(sock, SOT_SERVER_HEADER_NOCACHE, strlen(SOT_SERVER_HEADER_NOCACHE));
    write(sock, SOT_SERVER_HEADER_SERVER, strlen(SOT_SERVER_HEADER_SERVER));
    write(sock, SOT_SERVER_HEADER_TEXT, strlen(SOT_SERVER_HEADER_TEXT));
    write(sock, SOT_SERVER_HEADER_FINISH, strlen(SOT_SERVER_HEADER_FINISH));
    write(sock, status, 4);
  }

}
