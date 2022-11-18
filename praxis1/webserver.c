#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
// Beej includes
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>


#define BACKLOG 1
#define BUFFERSIZE 64
#define CRLF "\r\n"
#define CRLF_LEN 2
#define CRLFCRLF "\r\n\r\n"
#define CRLFCRLF_LEN 4
#define isLetter(x) (('A' <= x && x <= 'Z') || ('a' <= x && x <= 'z'))
#define isNumber(x) ('0' <= x && x <= '9')
#define LOGLEVEL -1
#define LV_OUTPUT 0
#define LV_DEBUG 1
#define HTTP_VERSION "HTTP/1.1"

// ERROR stuff 

#define assertOne(x, msg, args...) \
	{\
		if (x == 1){\
			fprintf(stderr, "ERROR::Assertion::ExpectedOne, '%d'\n", x);\
			fprintf(stderr, msg, ##args);\
			exit(EXIT_FAILURE);\
		}\
	}

#define assertZero(x, msg, args...) \
	{\
		if (x != 0){\
			fprintf(stderr, "ERROR::Assertion::ExpectedZero, '%d'\n", x);\
			fprintf(stderr, msg, ##args);\
			exit(EXIT_FAILURE);\
		}\
	}

#define assertEqual(x, y, msg, args...) {\
	if (x != y){\
		fprintf(stderr, "ERROR::Assertion::ExpectedNotEqual, got '%d' instead of '%d'\n", x, y);\
		fprintf(stderr, msg, ##args);\
		exit(EXIT_FAILURE);\
		}\
	}

#define assertNotEqual(x, y, msg, args...) {\
	if (x == y){\
		fprintf(stderr, "ERROR::Assertion::ExpectedEqual, got '%d' instead of '%d'\n", x, y);\
		fprintf(stderr, msg, ##args);\
		exit(EXIT_FAILURE);\
		}\
	}

#define LOG(level, msg, args...) \
	{\
		if (level >= LOGLEVEL) {\
			printf("LOG::");\
			printf(msg, ##args);\
			printf("\n");\
		}\
	}


// string functions

char* allocateEmptyString() {
	char* s = malloc(1);
	s[0] = '\0';
	return s;
}

void freeString(char* s) {
	free(s);
}

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
	while (num != 0) {
		*str_ptr = realloc(*str_ptr, (i+1) * sizeof(char));
		str = *str_ptr;

		int digit = num % 10;
		num /= 10;
		str[i] = '0' + digit;
		i++;
	}
	// reverse string
	for (int j = 0; j < i/2; j++) {
		char tmp = str[j];
		str[j] = str[i-j];
		str[i-j] = tmp;
	}
	// add '\0'
	*str_ptr = realloc(*str_ptr, (i+1) * sizeof(char));
	str = *str_ptr;
	str[i] = '\0';

	return 0;
}

int joinStrings(char** str_ptr, const char** s, const int s_len, const char* del) {
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
	*str_ptr = realloc(*str_ptr,(len+1) * sizeof(char));
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

int count_substring(const char* s, const int s_len, const char* sub, const int sub_len) {
	/* Function: count_substring
	 * -------------------------
	 *  count_substring counts how often another string exist inside the first string. Both strings do not need to be terminated by '\0'.
	 *  char* s: string in which to search
	 *  int s_len: length of the first_string
	 *  char* sub: string which will be searched
	 *  int sub_len: sub length
	 *  returns: number of sub in s 
	 */
	// for non \0 terminated strings
	if (s_len < sub_len) return 0;
	if (s_len == 0) return 0;
	int count = 0;
	int cur = 0;
	for (int i = 0; i < s_len; i++) {
		for (int j = 0; j < sub_len; j++) {
			if (s[i+j] != sub[j]) break;
			if (j+1 == sub_len) count++;
		}
	}
	return count;
}

// socket functions

int init_server_socket(const char* port) {
	/* Function: init_server_socket
	 * ----------------------------
	 *  init_server_socket initializes a listing tcp socket on the given port.
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
	struct addrinfo *servinfo;

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

int wait_and_connect(const int sockfd) {
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
	struct addrinfo coninfo = {0};
	int con_socket_fd = accept(sockfd, coninfo.ai_addr, &coninfo.ai_addrlen);
	assertZero(con_socket_fd==-1, "ERROR::setup_connection::accept %s\n", strerror(errno));

	return con_socket_fd;
}

// http functions
int receiveHttpReq(char* buffer, const int sockfd) {
	/* Fucntion: receiveHttpReq
	 * ---------------------------
	 *  receiveHttpReq receives an HTTP packet with a connected socket
	 *  int sockfd: connected socket filedescriptor
	 *  buffer: reallocatable string with size 0
	 *  returns: 0 if successfull else 1
	 *  onerror: if the connection is interrupted NULL is returned
	 *
	 */
	int status = 0;

	// receive packet
	int cursize = 0;
	int remaining = 0;
	int stop = 0;
	while (stop == 0) {
		if (remaining == 0) {
			buffer = realloc(buffer, (cursize+BUFFERSIZE) * sizeof(char));
			remaining = BUFFERSIZE;
			buffer[cursize+BUFFERSIZE-1] = '\0';
		}
		LOG(LV_DEBUG, "receiveHttp::waiting for data");
		status = recv(sockfd, buffer+cursize, remaining, 0);
		if (status == 0) {
			LOG(LV_DEBUG, "receiveHttp::received zero bytes, no more bytes will be received");
			return 1;
		}
		assertZero(status == -1, "ERROR:receiveHttp::recv %s\n", strerror(errno));
		remaining -= status;
		cursize += status;
		stop += count_substring(buffer, cursize, CRLFCRLF, CRLFCRLF_LEN);
		LOG(LV_DEBUG, "receiveHttpReq received %d bytes", status);
	}
	cursize++;
	buffer = realloc(buffer, cursize * sizeof(char));
	buffer[cursize-1] = '\0';
	
	return 0;
}

int sendHttpPayload(const int sockfd, const char* payload,const int size) {
	/* Function: sendHttpPayload
	 * ------------------------
	 *  sends a http packet to the socket connected with the given socket
	 *  int sockfd: connected scoket
	 *  char* payload: string buffer representing the HTTP packet to send
	 *  int size: amount of bytes to send
	 *  returns: 0
	 *  onerror: 
	 */
	int status = 0;
	int sum = 0;
	while (sum < size) {
		status = send(sockfd, payload, size-sum, 0);
		assertZero(status == -1, "ERRRO::sendHttpPayload %s\n", strerror(errno));
		sum += status;
	}
	return 0;
}

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
} HttpReq;

typedef struct HttpResp_t {
	char* version;
	char* status;
	char* reason;
	int header_count;
	Header** headers;
	char* payload;
} HttpResp;

Header* allocateHeader() {
	return calloc(1, sizeof(Header));
}

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

HttpResp* allocateHttpResp() {
	HttpResp* p = calloc(1, sizeof(HttpResp));
	p->headers = malloc(0);
	assertZero(p==NULL||p->headers==NULL, "ERROR::allocateHttpResp::couldn't allocate HttpResp\n");
	return p;
}

HttpReq* allocateHttpReq() {
	HttpReq* p = calloc(1, sizeof(HttpReq));
	p->headers = malloc(0);
	assertZero(p==NULL||p->headers==NULL, "ERROR::allocateHttpReq::couldn't allocate HttpReq\n");
	return p;
}

int initHttpReq(HttpReq* p, const char* payload) {
	/* Function: initHttpReq
	 * ------------------------
	 *  initHttpReq builds an HttpReq structure from a given payload. The last invalid header is assumed to be the payload.
	 * char* payload: string buffer, ending with a CRLFCRLF
	 * HttpReq* p: allocated HttpReq strucutre
	 * returns: 0 if successfull, 1 if no method is found, 2 if no uri is found, 3 if no version is found.
	 */
	int status = 0;
	char* save_ptr = NULL;
	char* payload_cpy = malloc(strlen(payload) * sizeof(char));
	assertZero(payload_cpy==NULL, "ERROR::initHttpReq::couldn't allocate array of size %d\n", (int) strlen(payload));
	strcpy(payload_cpy, payload);
	// parse method
	p->method = strtok_r(payload_cpy, " ", &save_ptr);
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
	// parse headers and payload 
	int header_count = 0;
	int content_length = -1;
	while (1) {
		char* header_tok = strtok_r(NULL, CRLF, &save_ptr);
		if (header_tok == NULL)
			break;
		Header* header = allocateHeader();
		status = initHeader(header, header_tok);
		if (status == 0) {
			// add header to header list
			header_count++;
			p->headers = realloc(p->headers, header_count * sizeof(Header));
			p->headers[header_count-1] = header;
		} else {
			// TODO if header is invalid it's assumed to be the payload
			p->payload = header_tok;
		}
	}
	// TODO Payload check at the end
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
	arr[0] = allocateEmptyString();
	joinStrings(&arr[0], (const char**) tmp, 3, " ");
	free(tmp);

	// Headers
	int i = 0;
	for (int i = 0; i < p->header_count; i++) {
		arr[i+1] = allocateEmptyString();
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
	Header* h = allocateHeader();
	h->key = "Content-Length";
	h->value = allocateEmptyString();
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

// resource stuff
typedef struct Resource_t {
	char* path;
	char* data;
} Resource;

Resource* allocateResourceArray() {
	return calloc(0, sizeof(Resource));
}

int addResource(Resource* arr, int size, Resource r) {
	arr = realloc(arr, ++size * sizeof(Resource));
	arr[size-1] = r;
	return size+1;
}

int findResource(Resource* arr, int storage_size, char* path) {
	for (int i = 0; i < storage_size; i++) {
		if (strcmp(arr[i].path, path) == 0)
			return i;
	}
	return -1;
}

int removeResource(Resource* arr, int size, int idx) {
	if (idx == -1) return -1;
	for (int i = idx+1; i < size; i++) {
		arr[i-1] = arr[i];
	}
	arr = realloc(arr, (size-1) * sizeof(char));
	return size-1;
}

void freeResourceArray(Resource* arr) {
	free(arr);
}

void printPayload(const char* payload) {
	int n = 23;
	for (int i = 0; i < n; i++)
		printf("#");
	printf("\n");
	printf("%s", payload);
	printf("\n");
	for (int i = 0; i < n; i++)
		printf("#");
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
	Resource* dynamic_content = allocateResourceArray();
	int dynamic_count = 0;

	// Responses
	HttpResp* resp_client_error = allocateHttpResp();
	resp_client_error->version = HTTP_VERSION;
	resp_client_error->status = "400";

	while (1) {
		// setup server
		LOG(LV_OUTPUT,"Creating socket");
		int my_sockfd = init_server_socket(port);

		// setup connection
		int con_sockfd = wait_and_connect(my_sockfd);
		HttpResp* resp;
		char* payload = allocateEmptyString();

		// Logic tree
		LOG(LV_OUTPUT, "waiting for new connection");
		if (receiveHttpReq(payload, con_sockfd) != 0)
			resp = resp_client_error;
		else {
			// build request
			HttpReq* req = allocateHttpReq();
			initHttpReq(req, payload);
			printPayload(payload);
			resp = allocateHttpResp();
			
			// build response
			Resource r = {req->uri, req->payload};
			resp->version = "HTTP/1.1";
			resp->reason = "OK";
			if (strcmp(req->method, "GET") == 0) {
				resp->status = "204";
				if (strncmp(req->uri, "static/", 7) == 0) {
					resp->status = "200";
					if (strcmp(req->uri, "static/foo") == 0){
						setPayloadHttpResp(resp, "Foo");
					}
					else if (strcmp(req->uri, "static/bar")){
						setPayloadHttpResp(resp, "Bar");
					}
					else if (strcmp(req->uri, "static/baz")){
						setPayloadHttpResp(resp, "Baz");
					}
					else {
						resp->status = "404";
					}
				}
				else if (strncmp(req->uri, "dynamic/", 8) == 0) {
					int idx = findResource(dynamic_content, dynamic_count, r.path);
					if (idx >= 0) {
						setPayloadHttpResp(resp, dynamic_content[idx].data);
					} 
					else {
						resp->status = "403";
					}
				}
				else {
					resp->status = "404";
				}
			}
			else if (strcmp(req->method, "PUT") == 0) {
				if (strncmp(req->uri, "dynamic/", 8) == 0) {
					int idx = findResource(dynamic_content, dynamic_count, r.path);
					if (idx >= 0) {
						dynamic_content[idx] = r;
					}
					else {
						addResource(dynamic_content, dynamic_count, r);
					}
				}
			}
			else if (strcmp(req->method, "DELETE") == 0) {
				if (strncmp(req->uri, "dynamic/", 8) == 0) {
					int idx = findResource(dynamic_content, dynamic_count, r.path);
					if (idx >= 0)
						removeResource(dynamic_content, dynamic_count, idx);
				}
			}
			else {
				resp->status = "501";
			}

			freeHttpReq(req);
		}
		// LOGIC TREE END

		// build response payload
		memset(payload, '\0', strlen(payload));
		payload = realloc(payload, 1);
		LOG(LV_OUTPUT, "initializing payload");
		initRespPayload(&payload, resp, CRLF);
		printPayload(payload);

		// send response
		sendHttpPayload(con_sockfd, payload, strlen(payload));

		// free
		LOG(LV_OUTPUT,"teardown");
		free(payload);
		freeHttpResp(resp);
		close(con_sockfd);
		close(my_sockfd);
	} 

	return 0;
}

