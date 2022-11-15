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
#define log(x) printf("LOG::%s\n", x)

void handle_error(int non_error_cond, char* msg, const char* error) {
	if (non_error_cond != 0) {
		fprintf(stderr, msg, error);
		exit(1);
	}
	return;
}

int count_substring(char* s, int s_len, char* sub, int sub_len) {
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

int init_server_socket(char* port) {
	int status = 0;

	// getaddrinfo 
	struct addrinfo hints;
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_family = AF_UNSPEC;
	hints.ai_flags = AI_PASSIVE | AI_NUMERICSERV;
	struct addrinfo *servinfo;

	status = getaddrinfo(NULL, port, &hints, &servinfo);
	handle_error(status, "ERROR::init_server_socket::getaddrinfo %s\n", gai_strerror(status));

	// socket
	int socket_fd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
	handle_error(socket_fd == -1, "ERROR::init_server_socket::socket %s\n", strerror(errno));

	// bind
	status = bind(socket_fd, servinfo->ai_addr, servinfo->ai_addrlen);
	handle_error(status, "ERROR::init_server_socket::bind %s\n", strerror(errno));

	// free
	freeaddrinfo(servinfo);

	return socket_fd;
}

int wait_and_connect(int sockfd) {
	int status = 0;
	// listen
	status = listen(sockfd, BACKLOG);
	handle_error(status, "ERROR::wait_and_connect::listen %s\n", strerror(errno));

	// accept
	log("wait_and_connect::waiting for connection...");
	struct addrinfo coninfo;
	int con_socket_fd = accept(sockfd, coninfo.ai_addr, &coninfo.ai_addrlen);
	handle_error(con_socket_fd == -1, "ERROR::setup_connection::accept %s\n", strerror(errno));

	return con_socket_fd;
}

char* receiveHttpPacket(int sockfd) {
	int status = 0;

	// receive packet
	char* buffer = malloc(0);
	int cursize = 0;
	int remaining = 0;
	int crlf_count = 0;
	while (crlf_count < 3) {
		if (remaining == 0) {
			buffer = realloc(buffer, (cursize+BUFFERSIZE) * sizeof(char));
			remaining = BUFFERSIZE;
			buffer[cursize+BUFFERSIZE-1] = '\0';
		}
		log("receiveHttp::waiting for data");
		status = recv(sockfd, buffer+cursize, remaining, 0);
		handle_error(status == -1, "ERROR:receiveHttp::recv %s\n", strerror(errno));
		remaining -= status;
		crlf_count += count_substring(buffer+cursize, status, CRLF, 2);
		cursize += status;
		printf("received %d bytes\n", status);
	}
	cursize++;
	buffer = realloc(buffer, cursize * sizeof(char));
	buffer[cursize-1] = '\0';
	
	return buffer;
}

int sendHttpPacket(int sockfd, char* payload, int size) {
	int status = 0;
	int sum = 0;
	while (sum < size) {
		status = send(sockfd, payload, size-sum, 0);
		handle_error(status == -1, "ERRRO::sendHttpPacket %s\n", strerror(errno));
		sum += status;
	}
	return 0;
}

struct HttpPacket {
	char* method;
	char* uri;
	char* version;
	char** header;
	char* payload;
};

struct HttpPacket* initHttpPacket(char* payload) {
	// find version
	// find method
	// list headers
	// find payload
	struct HttpPacket *p = malloc(sizeof(struct HttpPacket));
	p->method = strtok(payload, " ");
	if (p->method == NULL) {
		printf("ERROR::initHttpPacket::ParsingError::no method found\n");
		return NULL;
	}

	p->uri = strtok(NULL, " ");
	if (p->uri == NULL) {
		printf("ERROR::initHttpPacket::ParsingError::no uri found\n");
		return NULL;
	}

	p->version = strtok(NULL, CRLF);
	if (p->version == NULL) {
		printf("ERROR::initHttpPacket::ParsingError::no version found\n");
		return NULL;
	}

	char* header = strtok(NULL, CRLF);

	p->payload = strtok(NULL, CRLF);
	if (p->payload == NULL) {
		printf("ERROR::initHttpPacket::ParsingError::no payload found\n");
		return NULL;
	}

	// TODO how two headers separated?
	int header_count = 0;
	while(strchr(header, '\n'))
		header_count++;
	printf("LOG::initHttpPacket Found %d headers\n", header_count);
	p->header = malloc(header_count * sizeof(char*));
	for (int i = 0; i < header_count; i++){
		p->header[i] = strtok((i==0)?header:NULL, "\n");
		if (p->header[i] == NULL){
			printf("ERROR::initHttpPacket::ParsingError::No header found\n");
			return NULL;
		}
	}

	return p;
}

int main(int argc, char** argv) {
	int status = 0;

	// program args
	char* port;
	handle_error(argc < 2, "ERROR::main::arguemnts %s\n", "Port parameter is undefined!");
	if (argc <= 2)
		port = argv[1];

	// server
	int my_sockfd = init_server_socket(port);
	int con_sockfd = wait_and_connect(my_sockfd);
	char* payload = receiveHttpPacket(con_sockfd);
	// handle packet
	printf("Data:\n%s", payload);
	sendHttpPacket(con_sockfd, "Replay\n", 7);
	struct HttpPacket *packet = initHttpPacket(payload);
	handle_error(packet==NULL, "ERROR::main::%s\n", "invalid http packet");
	// teardown
	close(my_sockfd);
	close(con_sockfd);
	free(packet);

	return 0;
}

