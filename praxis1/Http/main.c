#include "http.h"


int main() {
	char payload[] = \
	"GET uri HTTP/1.1\r\nHeader1:abc\r\nContent-Length:6\r\n\r\n123456GET /nxt/req/ HTTP/1.2\r\nHeader2:cba\r\n\r\n";

	char* nxtPtr;
	Request *r = deserializeRequest(payload, &nxtPtr);
	printf("\"%s\"\n", payload);
	printf("first packet: methodid=%d, header_count=%d, payload=\"%s\"\n", r->method, r->header_count, r->payload);
	printf("next packet: \"%s\"", nxtPtr);
}
