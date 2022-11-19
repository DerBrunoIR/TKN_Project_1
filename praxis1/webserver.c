
// TODO's
// implement pathcmp and replace strcmp
// mybe update receiveHttpReq, payload couldn't end with CRLF but is separated with 2*CRLF
// fix bugs

// BUGS:
// Symptom, error message, how to reproduce
// ServerCrash after first response: "corrupted size vs. prev_size", ./webserver 1234, SEND get-static-foo 
//


#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

// settings
#define HTTP_VERSION "HTTP/1.1"
#define BACKLOG 10
#define BUFFERSIZE 64
#define LOGLEVEL 0
// constants
#define LV_OUTPUT 0
#define LV_DEBUG 1
#define CRLF "\r\n"
#define CRLF_LEN 2
#define CRLFCRLF "\r\n\r\n"
#define CRLFCRLF_LEN 4
// macros
#define isLetter(x) (('A' <= x && x <= 'Z') || ('a' <= x && x <= 'z'))
#define isNumber(x) ('0' <= x && x <= '9')

// custom printfs
#define assertOne(x, msg, formats...) {\
		if (x == 1){\
			fprintf(stderr, "ERROR::Assertion::ExpectedOne, '%d'\n", x);\
			fprintf(stderr, msg, ##formats);\
			exit(EXIT_FAILURE);\
		}\
	}

#define assertZero(x, msg, formats...) {\
		if (x != 0){\
			fprintf(stderr, "ERROR::Assertion::ExpectedZero, '%d'\n", x);\
			fprintf(stderr, msg, ##formats);\
			exit(EXIT_FAILURE);\
		}\
	}

#define assertEqual(x, y, msg, formats...) {\
	if (x != y){\
		fprintf(stderr, "ERROR::Assertion::ExpectedNotEqual, got '%d' instead of '%d'\n", x, y);\
		fprintf(stderr, msg, ##formats);\
		exit(EXIT_FAILURE);\
		}\
	}

#define assertNotEqual(x, y, msg, formats...) {\
	if (x == y){\
		fprintf(stderr, "ERROR::Assertion::ExpectedEqual, got '%d' instead of '%d'\n", x, y);\
		fprintf(stderr, msg, ##formats);\
		exit(EXIT_FAILURE);\
		}\
	}

#define LOG(level, msg, formats...) {\
		if (level <= LOGLEVEL) {\
			printf("LOG::");\
			printf(msg, ##formats);\
			printf("\n");\
		}\
	}


// string functions

int pathcmp(const char* a, const char* b) {
	/* Function: pathcmp
	 * -----------------
	 * compare two paths from left to right
	 * const char* a:
	 * const char* b:
	 * returns: number of equal nodes
	 */
	// TODO
	assertZero(1, "ERROR::pathcmp::Not implemented!");
	return 0;
}

int itoa(char** str_ptr, int num) {
	/* Function: itoa
	 * --------------
	 *  write an integer into the given string.
	 *  char** str_ptr: pointer to string
	 *  int num: number to convert
	 *  returns: 0
	 */
	// convert num into reversed ascii representation
	int i = 0; 
	char* str = *str_ptr;
	str = malloc(32 * sizeof(char));
	while (num != 0) {
		int digit = num % 10;
		num /= 10;
		str[i] = '0' + digit;
		i++;
	}
	if (i == 0) {
		str[0] = '0';
		i++;
	}
	// reverse string
	for (int j = 0; j < i/2; j++) {
		char tmp = str[j];
		str[j] = str[i-j];
		str[i-j] = tmp;
	}
	str[i] = '\0';
	*str_ptr = realloc(*str_ptr, (i+1) * sizeof(char));

	return 0;
}

int joinStrings(char** str_ptr, const char** s, int s_len, char* del) {
	/* Function: joinStrings
	 * ----------------------
	 * joinStrings joins strings with a delimiter between each other.
	 * char** str_ptr: pointer to allocated string, which will be reallocated
	 * const char** s: array of stirngs to be joined
	 * const int s_len: length of (const char**) s
	 * const char* del: delimiter 
	 * returns: 0
	 */
	if (str_ptr == NULL)
		return 1;
	int len = 0;
	int null_count = 0;
	int del_len = strlen(del);
	// calc new string size
	for (int i = 0; i < s_len; i++) {
		if (s[i] == NULL) {
			null_count++;
			continue;
		}
		len += strlen(s[i]);
	}
	len += (s_len-1-null_count) * del_len;
	// resize string
	*str_ptr = malloc((len+1) * sizeof(char));
	// join strings
	char* loc = *str_ptr;
	for (int i = 0; i < s_len-1; i++) {
		if (s[i] == NULL)
			continue;
		strcpy(loc, s[i]);
		loc += strlen(s[i]);
		strcpy(loc, del);
		loc += del_len;
	}
	strcpy(loc, s[s_len-1]);
	loc += strlen(s[s_len-1]);
	return 0;
}

int concatenate(char** str_ptr, const char* a) {
	/* Function: concatenate
	 * ---------------------
	 *  append strings to each other
	 *  char** str_ptr: pointer to allocated string
	 *  const char* a: string to append
	 *  returns: 0
	 */
	int len = strlen(*str_ptr);
	int other_len = strlen(a);
	int new_len = len + other_len;
	*str_ptr = realloc(*str_ptr, new_len * sizeof(char));
	strncat(*str_ptr, a, other_len);
	return 0;

}

char* firstCharNotInSubstring(char* str, const char* sub) {
	/* Function: firstCharNotInSubstring
	 * ---------------------------------
	 *  firstCharNotInSubstring finds the location of the first character which isn't part of the given set
	 *  char* str: string to search in 
	 *  char* sub: set of characters 
	 *  returns: pointer to the character not present in sub
	 *  onerror: NULL is returned if strlen(str) == 0
	 */
	while (1) {
		if (*str == '\0')
			break;
		if (strchr(sub, (int) *str) == NULL)
			return str;
		str++;
	}
	return NULL;
}


// socket functions

typedef struct Header_t {
	char* key;
	char* value;
} Header;

typedef struct HttpReq_t {
	char* method;
	char* uri;
	char* version;
	int header_count;
	Header** headers;
	char* payload;
	int error;
} HttpReq;

typedef struct HttpResp_t {
	char* version;
	char* status;
	char* reason;
	int header_count;
	Header** headers;
	char* payload;
} HttpResp;

int init_server_socket(const char* port) {
	/* Function: init_server_socket
	 * ----------------------------
	 *  init_server_socket initializes a listing tcp socket on the given port. 
	 *  Calls getaddrinfo, socket, setsockopt and bind.
	 *  char* port: a alphanumeric representation for a port number
	 *  returns: filedescriptor
	 *  onerror: exits the program
	 */
	int status = 0;

	// getaddrinfo 
	struct addrinfo hints;
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_family = AF_UNSPEC;
	hints.ai_flags = AI_PASSIVE | AI_NUMERICSERV;
	hints.ai_protocol = 0;
	struct addrinfo *servinfo;

	status = getaddrinfo(NULL, port, &hints, &servinfo);
	assertZero(status, "ERROR::init_server_socket::getaddrinfo %s\n", gai_strerror(status));
	//TODO remove
	status = getaddrinfo(NULL, port, &hints, &servinfo);
	assertZero(status, "ERROR::init_server_socket::getaddrinfo %s\n", gai_strerror(status));
	

	// socket
	int socket_fd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
	assertZero(socket_fd == -1, "ERROR::init_server_socket::socket %s\n", strerror(errno));
	// make port reusable
	status = setsockopt(socket_fd, servinfo->ai_socktype, SO_REUSEADDR, &(int){1}, sizeof(int));
	assertZero(status<0, "ERROR::init_server_socket::setsockopt::%s\n", "Make socket reuasble faild");

	// bind
	status = bind(socket_fd, servinfo->ai_addr, servinfo->ai_addrlen);
	assertZero(status, "ERROR::init_server_socket::bind %s\n", strerror(errno));

	// free
	freeaddrinfo(servinfo);
	
	return socket_fd;
}

int wait_and_connect(int sockfd) {
	/* Function: wait_and_connect
	 * -------------------------
	 * This functions waits until a tcp connection is for the given socket filedescriptor possible and then accepts it. This function will block your programm!
	 * int sockfd: socket filedescriptor
	 * returns: a new filedescriptor which is connected
	 * onerror: NULL is returned
	 */
	int status = 0;
	// listen
	status = listen(sockfd, BACKLOG);
	assertZero(status, "ERROR::wait_and_connect::listen %s\n", strerror(errno));

	// accept
	LOG(LV_DEBUG, "wait_and_connect::waiting for connection...");
	struct addrinfo coninfo;
	memset(&coninfo, 0, sizeof(struct addrinfo));
	int con_socket_fd = accept(sockfd, coninfo.ai_addr, &coninfo.ai_addrlen);
	assertZero(con_socket_fd==-1, "ERROR::setup_connection::accept %s\n", strerror(errno));

	return con_socket_fd;
}

Header* allocateHeader(

int initHeader(Header* h, const char* s) {
	/* Function: initHeader
	 * --------------------
	 *  initHeader builds a Header struct by the given header line.
	 *  char* s: header line from a http packet
	 *  Header* h: pointer to allocated header 
	 *  returns: 0 if successfull, 1 if the header key is invalid, 2 if the header value is invalid and 3 if the header value is a string of whitespaces
	 */
	char* save_ptr = NULL;
	char* s_cpy = malloc(strlen(s) * sizeof(char));
	strcpy(s_cpy, s);

	// set header.key
	h->key = strtok_r(s_cpy, ":", &save_ptr);
	if (h->key == NULL) {
		return 1;
	}
	// set header.val
	h->value = strtok_r(NULL, CRLF, &save_ptr);
	if (h->value == NULL) {
		return 2;
	}
	// remove leading whitespaces
	h->value = firstCharNotInSubstring(h->value, " ");
	if (h->value == NULL) {
		return 3;
	}
	return 0;
}

void freeHeader(Header* h) {
	free(h);
}

int initHttpReq(HttpReq* p, char* buffer) {
	/* Function: initHttpReq
	 * ------------------------
	 *  initHttpReq builds an HttpReq structure from a given buffer. The last invalid header is assumed to be the payload.
	 * char* buffer: string buffer, ending with a CRLFCRLF
	 * HttpReq* p: allocated HttpReq strucutre
	 * returns: 0 if successfull, 1 if no method is found, 2 if no uri is found, 3 if no version is found, 4 if a header is incorrect.
	 */
	int status = 0;
	char* save_ptr = NULL;
	// parse method
	p->method = strtok_r(buffer, " ", &save_ptr);
	if (p->method == NULL)
		return 1;
	// parse uri
	p->uri = strtok_r(NULL, " ", &save_ptr);
	if (p->uri == NULL) 
		return 2;

	// parse version
	p->version = strtok_r(NULL, CRLF, &save_ptr);
	if (p->version == NULL)
		return 3;
	// parse headers and buffer 
	int header_count = 0;
	int content_length = -1;
	while (1) {
		char* header_tok = strtok_r(NULL, CRLF, &save_ptr);
		if (header_tok == NULL)
			break;
		Header* header;
		status = initHeader(&header, header_tok);
		if (status == 0) {
			// add header to header list
			header_count++;
			p->headers = realloc(p->headers, header_count * sizeof(Header));
			p->headers[header_count-1] = header;
		} else {
			return 4;
		}
	}
	p->header_count = header_count;
	
	return 0;
}

void freeHttpReq(HttpReq* p) {
	for (int i = 0; i < p->header_count; i++)
		freeHeader(p->headers[i]);
	free(p->headers);
	free(p);
	return;
}

void freeHttpResp(HttpResp* p) {
	for (int i = 0; i < p->header_count; i++)
		freeHeader(p->headers[i]);
	free(p->headers);
	free(p);
	return;
}

int initRespPayload(char** str_ptr, HttpResp* p, char* sep) {
	/* Function: initStringBuffer
	 * --------------------------------
	 *  initStringBuffer creates string buffer from the given HTTP packet structure.
	 *  HttpReq* p: HTTP packet
	 *  char* str_ptr: pointer to allocated string
	 *  char* sep: seperator string between payload lines.
	 *  returns: allocated string buffer
	 */
	int status = 0;
	int hasPayload = p->payload != NULL;
	int arr_len = 1 + p->header_count + hasPayload;
	char** arr = calloc(arr_len, sizeof(char**));
	
	// first line
	char** tmp = malloc(3 * sizeof(char*));
	tmp[0] = p->version;
	tmp[1] = p->status;
	tmp[2] = p->reason;
	joinStrings(&arr[0], (const char**) tmp, 3, " ");
	free(tmp);

	// Headers
	int i = 0;
	for (int i = 0; i < p->header_count; i++) {
		tmp = malloc(2 * sizeof(char));
		tmp[0] = p->headers[i]->key;
		tmp[1] = p->headers[i]->value;
		joinStrings(&arr[i+1], (const char**) tmp, 2, ":"); 
		free(tmp);
	}
	// Payload
	if (hasPayload)
		arr[i+2] = p->payload;

	// write joined strings into str_ptr
	joinStrings(str_ptr, (const char**) arr,  arr_len, sep);

	// free
	free(arr[0]);
	for (i = 0; i < p->header_count; i++)
		free(arr[i+1]);
	free(arr);

	// add CRLF to the end of the payload
	concatenate(str_ptr, CRLF);

	return 0;
}

int findHeaderHttpResp(HttpResp* resp, Header *h) {
	/* Function: findHeaderHttpResp
	 * ----------------------------
	 *  checks if the given HTTP response contains a header with a certain key
	 *  HttpResp* resp:
	 *  Header *h:
	 *  returns: -1 if nothing is found otherwise the index
	 */
	for (int i = 0; i < resp->header_count; i++) {
		if (strcmp(resp->headers[i]->key, h->key) == 0)
			return i;
	}
	return -1;
}

int addHeaderHttpResp(HttpResp* resp, Header *h) {
	/* Fucntion: addHeaderHttpResp
	 * ---------------------------
	 *  add a header to a given HTTP response
	 *  HttpResp* resp: 
	 *  Header *h: 
	 *  returns: 0
	 */
	resp->header_count++;
	resp->headers = realloc(resp->headers, resp->header_count);
	resp->headers[resp->header_count-1] = h;
	return 0;
}

int setPayloadHttpResp(HttpResp* resp, char* payload) {
	/* Function: setPayloadHttpResp
	 * ----------------------------
	 *  add a payload to a HTTP response.
	 *  HttpResp* resp: Responses
	 *  char* payload: payload
	 *  returns: 0
	 */
	// add Content-Lenght Header
	Header* h;
	h->key = "Content-Length";
	itoa(&h->value, strlen(payload));
	int idx = findHeaderHttpResp(resp, h);
	if (idx >= 0) {
		resp->headers[idx]->value = h->value;
		freeHeader(h);
	}
	else
		addHeaderHttpResp(resp, h);
	// set new payload
	resp->payload = payload;
	return 0;
}

// http functions
int receiveHttpReq(char** buf_ptr, int sockfd) {
	/* Fucntion: receiveHttpReq
	 * ---------------------------
	 *  receiveHttpReq receives an HTTP packet with a connected socket
	 *  int sockfd: connected socket filedescriptor
	 *  buffer: reallocatable string with size 0
	 *  returns: 0 if successfull else 1
	 *  onerror: if the connection is interrupted NULL is returned
	 *
	 */

	char* buffer = malloc((BUFFERSIZE+1) * sizeof(char));
	buffer[BUFFERSIZE] = '\0';
	int size = 0;
	char* header_end = NULL;
	// receive info and headers
	while (!header_end) {
		int bytes_recv = recv(sockfd, buffer+size, BUFFERSIZE-size, 0);
		assertOne(bytes_recv >= 0, "receiveHttp::recv::%s\n", strerror(errno));
		if (bytes_recv == 0) {
			LOG(LV_DEBUG, "receiveHttp::connection closed");
			return 1;
		}
		header_end = strstr(buffer+size, CRLFCRLF);
		size += bytes_recv;
		LOG(LV_DEBUG, "receiveHttpReq::received %d bytes", size);
		assertOne(BUFFERSIZE > size, "receiveHttpReq::recv::Bufferoverflow");
	}
	
	HttpReq* req;
	req->error = initHttpReq(req, buffer); 
	
	return 0;
}

int sendHttpPayload(int sockfd, const char* buffer) {
	/* Function: sendHttpPayload
	 * ------------------------
	 *  sends a http packet to the socket connected with the given socket
	 *  int sockfd: connected scoket
	 *  char* buffer: string buffer representing the HTTP packet to send
	 *  int size: amount of bytes to send
	 *  returns: 0
	 *  onerror: 
	 */
	int status = 0;
	int sum = 0;
	int size = strlen(buffer);
	while (sum < size) {
		status = send(sockfd, buffer, size-sum, 0);
		assertZero(status == -1, "ERRRO::sendHttpPayload %s\n", strerror(errno));
		sum += status;
	}
	return 0;
}

// resource stuff
typedef struct Resource_t {
	char* path;
	char* data;
} Resource;

int addResource(Resource** arr_ptr, int size, Resource r) {
	Resource* arr = malloc(++size * sizeof(Resource));
	arr[size-1] = r;
	*arr_ptr = arr;
	return size+1;
}

int findResource(Resource* arr, int storage_size, char* path) {
	for (int i = 0; i < storage_size; i++) {
		if (strcmp(arr[i].path, path) == 0)
			return i;
	}
	return -1;
}

int removeResource(Resource** arr_ptr, int size, int idx) {
	if (idx < 0) return -1;
	for (int i = idx+1; i < size; i++) {
		*arr_ptr[i-1] = *arr_ptr[i];
	}
	*arr_ptr = realloc(*arr_ptr, (size-1) * sizeof(char));
	return size-1;
}

void freeResourceArray(Resource* arr) {
	free(arr);
}

// fancy print

void printPayload(const char* payload) {
	int n = 9; // Primezahl
	printf("\n");
	for (int i = 0; i < n; i++) printf("#");
	printf(" Payload ");
	for (int i = 0; i < n; i++) printf("#");
	printf("\n");
	printf("%s", payload);
	printf("\n");
	for (int i = 0; i < n; i++) printf("#");
	printf(" --END-- ");
	for (int i = 0; i < n; i++) printf("#");
	printf("\n");
	printf("\n");
}

// main 
int main(int argc, char** argv) {
	// init arguemnts
	int status = 0;
	char* port;
	assertEqual(argc, 2, "ERROR::main::argument error!\n%s [port]\n", argv[0]);
	port = argv[1];
	LOG(LV_OUTPUT,"Starting webserver on port '%s'", port);

	// create dynamic resource storage
	Resource* dynamic_content;
	int dynamic_count = 0;

	// Responses
	HttpResp resp_client_error = {0};
	resp_client_error.version = HTTP_VERSION;
	resp_client_error.status = "400";

	while (1) {
		// setup server
		LOG(LV_OUTPUT,"Creating socket");
		int my_sockfd = init_server_socket(port);

		// setup connection
		int con_sockfd = wait_and_connect(my_sockfd);
		HttpResp resp;
		char* payload;

		// Logic tree
		LOG(LV_OUTPUT, "waiting for new connection");
		if (receiveHttpReq(&payload, con_sockfd) != 0){
			LOG(LV_OUTPUT, "got invalid request, returning 404");
			resp = resp_client_error;
		}
		else {
			// build request
			HttpReq req;
			initHttpReq(&req, payload);
			printPayload(payload);
			
			// build response
			Resource r = {req.uri, req.payload};
			resp.version = "HTTP/1.1";
			resp.reason = "OK";
			if (strcmp(req.method, "GET") == 0) {
				resp.status = "204";
				if (strncmp(req.uri, "static/", 7) == 0) {
					resp.status = "200";
					if (strcmp(req.uri, "static/foo") == 0){
						setPayloadHttpResp(&resp, "Foo");
					}
					else if (strcmp(req.uri, "static/bar")){
						setPayloadHttpResp(&resp, "Bar");
					}
					else if (strcmp(req.uri, "static/baz")){
						setPayloadHttpResp(&resp, "Baz");
					}
					else {
						resp.status = "404";
					}
				}
				else if (strncmp(req.uri, "dynamic/", 8) == 0) {
					int idx = findResource(dynamic_content, dynamic_count, r.path);
					if (idx >= 0) {
						setPayloadHttpResp(&resp, dynamic_content[idx].data);
					} 
					else {
						resp.status = "403";
					}
				}
				else {
					resp.status = "404";
				}
			}
			else if (strcmp(req.method, "PUT") == 0) {
				if (strncmp(req.uri, "dynamic/", 8) == 0) {
					int idx = findResource(dynamic_content, dynamic_count, r.path);
					if (idx >= 0) {
						dynamic_content[idx] = r;
					}
					else {
						addResource(&dynamic_content, dynamic_count, r);
					}
				}
			}
			else if (strcmp(req.method, "DELETE") == 0) {
				if (strncmp(req.uri, "dynamic/", 8) == 0) {
					int idx = findResource(dynamic_content, dynamic_count, r.path);
					if (idx >= 0)
						removeResource(&dynamic_content, dynamic_count, idx);
				}
			}
			else {
				resp.status = "501";
			}
		}
		// LOGIC TREE END

		// build response payload
		memset(payload, '\0', strlen(payload));
		payload = realloc(payload, 1);
		LOG(LV_OUTPUT, "initializing payload");
		initRespPayload(&payload, &resp, CRLF);
		printPayload(payload);

		// send response
		sendHttpPayload(con_sockfd, payload, strlen(payload));

		// free
		LOG(LV_OUTPUT,"teardown");
		free(payload);
		close(con_sockfd);
		close(my_sockfd);
	} 

	return 0;
}

