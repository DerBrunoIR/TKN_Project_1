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

void handle_error(int non_error_cond, char* msg) {
	if (non_error_cond != 0) {
		fprintf(stderr, msg, strerror(errno));
		exit(1);
	}
	return;
}

int main(int argc, char** argv) {
	int status = 0;
	char* port;
	if (argc >= 2) {
		port = argv[1]; 
		printf("port number: %s\n", port);
	} else {
		printf("argument error: no port specified!\n");
		exit(1);
	}

	// prepare socket creation
	struct addrinfo hints;
	struct addrinfo *servinfo;
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC; // select IP version automatically
	hints.ai_socktype = SOCK_STREAM; // TCP
	hints.ai_flags = AI_PASSIVE | AI_NUMERICSERV; // enable hoste mode, service argument is numeric string 

	status = getaddrinfo(NULL, port, &hints, &servinfo);
	if (status != 0) {
		fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
		exit(1);
	}
	
	// create socket
	int socket_fd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
	handle_error(socket_fd == -1, "socket error: %s\n");
	
	// bind socket
	status = bind(socket_fd, servinfo->ai_addr, servinfo->ai_addrlen);
	handle_error(status, "bind error: %s\n");

	// wait and acccept connection
	status = listen(socket_fd, BACKLOG);
	handle_error(status, "listen error: %s\n");

	struct addrinfo coninfo;
	memset(&coninfo, 0, sizeof coninfo);
	int con_socket_fd = accept(socket_fd, coninfo.ai_addr, &coninfo.ai_addrlen);
	handle_error(con_socket_fd == -1, "accept error: %s\n");

	// receive packet
	int sum = 0;
	int buflen = 32;
	char* buf = malloc((buflen+1) * sizeof(char));
	while (sum < buflen) {
		status = recv(con_socket_fd, buf, buflen, 0);
		handle_error(status == -1, "recv error: %s\n");
		sum += status;
	}

	// handle packet
	printf(buf);
	
	// respond
	
	
	// free all stuff
	close(con_socket_fd);
	close(socket_fd);
	freeaddrinfo(servinfo);

	return EXIT_SUCCESS;
}
