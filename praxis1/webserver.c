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

int itoa(char* str, int num) {
	int i = 0; 
	while (num != 0) {
		str = realloc(str, (i+1) * sizeof(char));
		int digit = num % 10;
		num /= 10;
		str[i] = '0' + digit;
		i++;
	}
	for (int j = 0; j < i/2; j++) {
		char tmp = str[j];
		str[j] = str[i-j];
		str[i-j] = tmp;
	}
	str = realloc(str, (i+1) * sizeof(char));
	str[i] = '\0';
	return 0;
}

int joinStrings(char** res, const char** s, const int s_len, const char* del) {
	/* Function: joinStrings
	 * ----------------------
	 * joinStrings joins strings with a delimiter between each other.
	 * const char** s: array of stirngs to be joined
	 * const int s_len: length of (const char**) s
	 * const char* del: delimiter 
	 * returns: 0
	 */
	int len = 0;
	int del_len = strlen(del);
	// calc new string size
	for (int i = 0; i < s_len; i++)
		len += strlen(s[i]);
	len += (s_len-1) * del_len;
	// TODO fix this
	*res = realloc(*res,(len+1) * sizeof(char));
	if (res == NULL)
		return 1;
	char* loc = *res;
	for (int i = 0; i < s_len-1; i++) {
		strcpy(loc, s[i]);
		loc += strlen(s[i]);
		strcpy(loc, del);
		loc += del_len;
	}
	strcpy(loc, s[s_len-1]);
	loc += strlen(s[s_len-1]);
	return 0;
}

char* firstCharNotInSubstring(char* str, const char* sub) {
	/* Function: firstCharNotInSubstring
	 * ---------------------------------
	 *  firstCharNotInSubstring finds the location of the first character which isn't part of the given set
	 *  char* str: searched string
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
	struct addrinfo coninfo;
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
	 *  initHttpReq builds an HttpReq structure from a given payload. Invalid headers are assumed to be the payload.
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

int initRespPayload(char* res, HttpResp* p, char* sep) {
	/* Function: initStringBuffer
	 * --------------------------------
	 *  initStringBuffer creates string buffer from the given HTTP packet structure.
	 *  HttpReq* p: HTTP packet
	 *  char* res: allocated string
	 *  char* sep: seperator string between payload lines.
	 *  returns: allocated string buffer
	 */
	int hasPayload = p->payload != NULL;
	int status = 0;
	int arr_len = p->header_count + 1 + hasPayload;
	char** arr = calloc(arr_len, sizeof(char**));

	char* info = malloc(0);
	char** f = malloc(3 * sizeof(char*));
	f[0] = p->version;
	f[1] = p->status;
	f[2] = p->reason;
	joinStrings(&info, (const char**) f, 3, " ");
	arr[0] = info;

	// Header
	int i = 0;
	for (; i < p->header_count; i++) {
		char* tmp = malloc(0);
		char** s = malloc(2 * sizeof(char));
		s[0] = p->headers[i]->key;
		s[1] = p->headers[i]->value;
		joinStrings(&tmp, (const char**) s, 2, ":"); 
		arr[i+1] = tmp;
	}
	// Payload
	if (hasPayload)
		arr[i+1] = p->payload;
	joinStrings(&res, (const char**) arr,  arr_len, sep);

	for (int i = 0; i < i+1; i++)
		free(arr[i]);
	free(arr);

	return 0;
}

int findHeaderHttpResp(HttpResp* resp, Header *h) {
	for (int i = 0; i < resp->header_count; i++) {
		if (strcmp(resp->headers[i]->key, h->key) == 0)
			return i;
	}
	return -1;
}

int addHeaderHttpResp(HttpResp* resp, Header *h) {
	resp->header_count++;
	resp->headers = realloc(resp->headers, resp->header_count);
	resp->headers[resp->header_count-1] = h;
	return 0;
}

int setPayloadHttpResp(HttpResp* resp, char* payload) {
	Header* h = allocateHeader();
	h->key = "Content-Type";
	h->value = malloc(0);
	itoa(h->value, strlen(payload));
	int idx = findHeaderHttpResp(resp, h);
	if (idx >= 0) {
		resp->headers[idx]->value = h->value;
		freeHeader(h);
	}
	else
		addHeaderHttpResp(resp, h);
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

// main 
int main(int argc, char** argv) {
	LOG(LV_OUTPUT,"Starting webserver");
	int status = 0;
	char* port;
	assertZero(argc < 2, "ERROR::main::arguemnts %s\n", "Port parameter is undefined!");
	if (argc <= 2)
		port = argv[1];


	Resource* dynamic_content = allocateResourceArray();
	int dynamic_count = 0;

	// Responses
	HttpResp* resp_client_error = allocateHttpResp();
	resp_client_error->version = HTTP_VERSION;
	resp_client_error->status = "400";

	// setup server
	LOG(LV_OUTPUT,"Creating socket");
	int my_sockfd = init_server_socket(port);

	while (1) {
		int con_sockfd = wait_and_connect(my_sockfd);
		HttpResp* resp;
		char* payload = malloc(0);

		// Logic tree
		LOG(LV_OUTPUT, "waiting for new connection");
		if (receiveHttpReq(payload, con_sockfd) != 0)
			resp = resp_client_error;
		else {
			HttpReq* req = allocateHttpReq();
			initHttpReq(req, payload);
			resp = allocateHttpResp();
			
			Resource r = {req->uri, req->payload};
			resp->version = "HTTP/1.1";
			resp->reason = "OK";
			if (strcmp(req->method, "GET") == 0) {
				resp->status = "204";
				// static test
				if (strncmp(req->uri, "static/", 7) == 0) {
					resp->status = "200";
					if (strcmp(req->uri, "static/foo") == 0)
						setPayloadHttpResp(resp, "Foo");
					else if (strcmp(req->uri, "static/bar"))
						setPayloadHttpResp(resp, "Bar");
					else if (strcmp(req->uri, "static/baz"))
						setPayloadHttpResp(resp, "Baz");
					else 
						resp->status = "404";
				}
				else if (strncmp(req->uri, "dynamic/", 8) == 0) {
					int idx = findResource(dynamic_content, dynamic_count, r.path);
					if (idx >= 0) {
						setPayloadHttpResp(resp, dynamic_content[idx].data);
					} 
					else
						resp->status = "403";
				}
				else 
					resp->status = "404";
			}
			else if (strcmp(req->method, "PUT") == 0) {
				if (strncmp(req->uri, "dynamic/", 8) == 0) {
					int idx = findResource(dynamic_content, dynamic_count, r.path);
					if (idx >= 0)
						dynamic_content[idx] = r;
					else
						addResource(dynamic_content, dynamic_count, r);
				}
			}
			else if (strcmp(req->method, "DELETE") == 0) {
				if (strncmp(req->uri, "dynamic/", 8) == 0) {
					int idx = findResource(dynamic_content, dynamic_count, r.path);
					if (idx >= 0)
						removeResource(dynamic_content, dynamic_count, idx);
				}
			}
			else
				resp->status = "501";

			freeHttpReq(req);
		}
		// send 
		free(payload);
		payload = malloc(0);
		LOG(LV_OUTPUT, "initializing payload");
		initRespPayload(payload, resp, CRLF);
		LOG(LV_OUTPUT, "sending payload");
		sendHttpPayload(con_sockfd, payload, strlen(payload));
		// free
		free(payload);
		freeHttpResp(resp);
		close(con_sockfd);
	} 

	// teardown
	LOG(LV_OUTPUT,"Teardown");
	close(my_sockfd);

	return 0;
}

