// switch to print message
#define SOT_SERVER_DEBUG 1
// set listening port
#define SOT_SERVER_PORT 80
// set max connection number p.s. lwip only accept 1 connection.
#define SOT_SERVER_MAX_CONNECTIONS 1
// http method
#define SOT_SERVER_METHOD_UNKNOWN 0
#define SOT_SERVER_METHOD_GET 1
#define SOT_SERVER_METHOD_POST 2
#define SOT_SERVER_METHOD_PUT 3
#define SOT_SERVER_METHOD_DELETE 4
#define SOT_SERVER_METHOD_HEAD 5
#define SOT_SERVER_METHOD_OPTION 6
#define SOT_SERVER_METHOD_TRACE 7
// http response headers
#define SOT_SERVER_HEADER_200 "HTTP/1.1 200 OK\r\n"
#define SOT_SERVER_HEADER_404 "HTTP/1.1 404 Not Found\r\n"
#define SOT_SERVER_HEADER_405 "HTTP/1.1 405 Method Not Allowed\r\n"
#define SOT_SERVER_HEADER_500 "HTTP/1.1 500 Internal Server Error\r\n"
#define SOT_SERVER_HEADER_SERVER "Server: SOTServer\r\n"
#define SOT_SERVER_HEADER_NOCACHE "Cache-Control: no-cache, must-revalidate\r\n"
#define SOT_SERVER_HEADER_STATIC_FILE "Content-Encoding: gzip\r\n"
#define SOT_SERVER_HEADER_TEXT "Content-Type: text/plain; charset=utf-8\r\n"
#define SOT_SERVER_HEADER_MIME "Content-Type: "
#define SOT_SERVER_HEADER_FINISH "\r\n"
// server settings
#define SOT_SERVER_DEFAULT_DOCUMENT "index.htm"
#define SOT_SERVER_CGI_PREFIX "/cgi/"
// accept status flag
bool SOTServer_blockingAcceptFlag = false;
// print message
void SOTServer_log(char* log);
// server start function
void SOTServer_start(void);
// server init function
void SOTServer_init(void);
// task of bootstrap
void SOTServer_bootstrap(void *para);
// task of bootstrap
void SOTServer_bindListening(void *para);
// task of httpd
void SOTServer_httpd(void *para);
// task of http worker
void SOTServer_worker(void *para);
// cgi handler
void SOTCGI_handler(char * cgiURI, int32 sock);
void SOTCGI_route(char *route, char *cgiURI, int32 sock, void (* callback)());

// SOT_ROM Setting
#define SOT_ROM_ORG 0x00100000
#define SOT_ROM_FAT_SIZE 0x00010000
// for SOT_ROM File api
struct SOTROM_filePointer {
  uint32 location;
  uint32 offset;
  uint32 fileSize;
  bool fileExsit;
  char *mime;
};
typedef struct SOTROM_filePointer SOTROM_file;
SOTROM_file *SOTROM_fopen(char* fileName);
bool SOTROM_fread(SOTROM_file *file, uint32* data, int32 datalength);


// system function
char *substr( char *source, int start, int end) {
  char *returnString = (char *)zalloc(64);
  returnString[0] = 0;
  start --;
  while(start < end) {
    returnString[strlen(returnString)] = source[start];
    start ++;
  }
  returnString[strlen(returnString)] = 0;
  return returnString;
}
