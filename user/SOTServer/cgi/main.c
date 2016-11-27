//this file contains code to handle cgi file
void SOTCGI_handler(char * cgiURI, int32 sock) {
  char *response = (char *)zalloc(64);
  SOTCGI_route("/cgi/fun/", cgiURI, sock, SOTCGI_PROG_gpio);
}

void SOTCGI_route(char *route, char *cgiURI, int32 sock, void (* callback)()) {
  if(strncmp(route, cgiURI, strlen(route)) == 0) {
    char *para = substr(cgiURI, strlen(route), strlen(cgiURI));
    (* callback)(para, sock);
    free(para);
  }
}
