void SOTServer_log(char* log) {
  if(SOT_SERVER_DEBUG) {
    printf("\nSOTServer:%s\n", log);
  }
}

bool SOTServer_netConnected( void) {
  STATION_STATUS netStatus;
  netStatus = wifi_station_get_connect_status();
  if(netStatus == STATION_GOT_IP) {
    return TRUE;
  } else {
    SOTServer_log("net connection not ready yet!");
    return FALSE;
  }
}
