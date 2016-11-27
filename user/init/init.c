void init(void) {

  // init UART
  UART_WaitTxFifoEmpty(UART0);
  UART_WaitTxFifoEmpty(UART1);
  UART_ConfigTypeDef uart_config;
  uart_config.baud_rate = BIT_RATE_115200;
  uart_config.data_bits = UART_WordLength_8b;
  uart_config.parity = USART_Parity_None;
  uart_config.stop_bits = USART_StopBits_1;
  uart_config.flow_ctrl = USART_HardwareFlowControl_None;
  uart_config.UART_RxFlowThresh = 120;
  uart_config.UART_InverseMask = UART_None_Inverse;
  UART_ParamConfig(UART0, &uart_config);
  UART_SetPrintPort(UART0);

  // init wifi connection
  wifi_set_opmode(STATION_MODE);
  struct station_config * wifi_config = (struct station_config *) zalloc(sizeof(struct station_config));
  sprintf(wifi_config->ssid, "missing");
  sprintf(wifi_config->password, "zhimakaimen");
  wifi_station_set_config(wifi_config);
  free(wifi_config);
  //wifi_set_event_handler_cb(wifi_event_handler);
  wifi_station_connect();
}
