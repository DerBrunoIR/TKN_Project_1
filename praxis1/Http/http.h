#ifndef WEBSERVER
#define WEBSERVER

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>


enum FLAGS {
	SUCCESS,
	ERROR,
};

enum HTTP_METHOD {
	GET,
	PUT,
	DELETE,
};


typedef struct Header {
	char* key;
	char* val;
} Header;


typedef struct Request {
	enum HTTP_METHOD method;
	char* uri;
	char* version;
	int header_count;
	Header* headers;
	FLAGS flags;
} Request;

typedef struct Response {
	char* version;
	int status;
	char* reason;
	int header_count;
	Header* headers;
	FLAGS flags;
} Response;


char* serializeRequest(Request* req);
char* serializeResponse(Response* resp);
Response* deserializeResponse(char* resp);
Request* deserializeRequest(char* req);


#endif

