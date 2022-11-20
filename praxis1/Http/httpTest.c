#include "http.h"
#include "string.h"
#include "assert.h"


void TestSerializeRequest(Request req_test, char* req_test_payload) {
	char* payload_test = serializeRequest(&req_test);
	assert(strcmp(payload_test, req_test_payload)==0);
	free(payload_test);
}

void TestSerializeResponse(Response resp_test, char* resp_test_payload) {
	char* payload_test = serializeResponse(&resp_test);
	assert(strcmp(payload_test, resp_test_payload)==0);
	free(payload_test);
}

void TestDeserializationResponse(Response resp_test, char* resp_test_payload, int offset) {
	char* cpy_payload_resp1 = malloc(strlen(resp_test_payload)*sizeof(char));
	strcpy(cpy_payload_resp1, resp_test_payload);
	char* testNxtPacket;
	Response* test = deserializeResponse(cpy_payload_resp1, &testNxtPacket);
	free(cpy_payload_resp1);
	//Test
	assert(testNxtPacket==cpy_payload_resp1+offset);
	assert(test->header_count==resp_test.header_count);
	assert(test->status==resp_test.status);
	assert(test->flags==resp_test.flags);
	for (int i = 0; i < test->header_count; i++) {
		assert(strcmp(test->headers[i].key, resp_test.headers[i].key)==0);
		assert(strcmp(test->headers[i].val, resp_test.headers[i].val)==0);
	}
	assert(strcmp(test->reason, resp_test.reason)==0);
	assert(strcmp(test->version, resp_test.version)==0);
	if (resp_test.payload!=NULL) 
		assert(strcmp(test->payload, resp_test.payload)==0);
	else 
		assert(test->payload==NULL);
	//Free
	freeResponse(test);
}

void TestDeserializationRequest(Request req, char* payload, int offset) {
	char* payload_cpy = malloc(strlen(payload)*sizeof(char));
	strcpy(payload_cpy, payload);
	char* testNxtPacket;
	Request* res = deserializeRequest(payload_cpy, &testNxtPacket);
	free(payload_cpy);
	//Test
	assert(testNxtPacket==payload_cpy+offset);
	assert(res->header_count==req.header_count);
	assert(res->method==req.method);
	assert(res->flags==req.flags);
	for (int i = 0; i < res->header_count; i++) {
		assert(strcmp(res->headers[i].key, req.headers[i].key)==0);
		assert(strcmp(res->headers[i].val, req.headers[i].val)==0);
	}
	assert(strcmp(res->uri, req.uri)==0);
	assert(strcmp(res->version, req.version)==0);
	if (req.payload!=NULL)
		assert(strcmp(res->payload, req.payload)==0);
	else 
		assert(res->payload==NULL);
	//Free
	freeRequest(res);
}

int main() {
	// test maximal reqeuest
	char payload[] = "This is an amazing payload!!!!"; // length=30
	Header headers1[] = {{"Content-Type", "text/html"}, {"Content-Length", "30"},{"Foo", "Bar"}};
	Response resp1 = {"HTTP/1.1", 200, "OK", 3, headers1, payload, 0};
	char* payload_resp1 = "HTTP/1.1 200 OK\r\nContent-Type:text/html\r\nContent-Length:30\r\nFoo:Bar\r\n\r\nThis is an amazing payload!!!!";
	Request req1 = {0, "/dir/file.txt", "HTTP/1.1", 3, headers1 ,payload, 0};
	char* payload_req1 = "GET /dir/file.txt HTTP/1.1\r\nContent-Type:text/html\r\nContent-Length:30\r\nFoo:Bar\r\n\r\nThis is an amazing payload!!!!";

	TestSerializeRequest(req1, payload_req1);
	TestSerializeResponse(resp1, payload_resp1);
	TestDeserializationResponse(resp1, payload_resp1, 101);
	TestDeserializationRequest(req1, payload_req1, 112);

	// test minimal request
	Request req2 = {1, "/", "HTTP/1.1", 0, NULL, 0};
	char* payload_req2 = "PUT / HTTP/1.1\r\n\r\n";
	Response resp2 = {"HTTP/1.1", 204, "OK", 0, NULL, NULL, 0};
	char* payload_resp2 = "HTTP/1.1 204 OK\r\n\r\n";

	TestSerializeRequest(req2, payload_req2);
	TestSerializeResponse(resp2, payload_resp2);
	TestDeserializationResponse(resp2, payload_resp2, strlen(payload_resp2));
	TestDeserializationRequest(req2, payload_req2, strlen(payload_req2));

	// test overlapping http packets without payload
	Request req3 = {1, "/", "HTTP/1.1", 0, NULL, 0};
	Response resp3 = {"HTTP/1.1", 204, "OK", 0, NULL, NULL, 0};
	char* payload_req3_out = "PUT / HTTP/1.1\r\n\r\n";
	char* payload_resp3_out = "HTTP/1.1 204 OK\r\n\r\n";
	char* payload_resp3_in = "HTTP/1.1 204 OK\r\n\r\nGet / YOUFUCKEDUP";
	char* payload_req3_in = "PUT / HTTP/1.1\r\n\r\nGET /../abc.txt HTTP/1.3";

	TestSerializeRequest(req3, payload_req3_out);
	TestSerializeResponse(resp3, payload_resp3_out);
	TestDeserializationRequest(req3, payload_req3_in, 18);
	TestDeserializationResponse(resp3, payload_resp3_in, 19);


	// test overlapping with payload
	char payload4[] = "Don't trust cats"; // length=16
	Header headers4[] = {{"Content-Length", "16"}, {"Javascript framework", " without Javascript"}};
	Request req4 = {2, "/home/usr", "HTTP/9.2", 2, headers4, payload4, 0};
	Response resp4 = {"HTTP/9.2", 123, "HUNGRY", 2, headers4, payload4, 0};
	char payload_req4_in[] = "";
	char payload_req4_out[] = "";
	char payload_resp4_in[] = "";
	char payload_resp4_out[] = "";
	TestSerializeRequest(req4, payload_req4_out);
	TestSerializeResponse(resp4, payload_resp4_out);
	TestDeserializationRequest(req4, payload_req4_in, );
	TestDeserializationResponse(resp4, payload_resp4_in, );




}

