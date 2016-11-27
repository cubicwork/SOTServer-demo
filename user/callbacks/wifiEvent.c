 void wifi_event_handler(System_Event_t *e) {
   printf("event from wifi -> CODE: %x\n", e->event_id);
   wifiStatus = e->event_id;
 }
