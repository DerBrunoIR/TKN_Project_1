#include "http.h"
#include "assert.h"


void TestCmpHeaders() {
	Header h1 = {"ABC", "DEF"};
	Header h2 = {"ABC", "DEF"};
	assert(cmpHeader(&h1, &h2)==true);
	h2.key = "CBA";
	assert(cmpHeader(&h1, &h2)==false);
	h1.key = "CBA";
	h1.val = "FED";
	assert(cmpHeader(&h1, &h2)==false);
}

void TestCmpRequests() {
	int hcnt = 1;
	Header hs1[] = {{"ABC", "ABC"}};
	Header hs2[] = {{"ABC", "ABC"},{"ABC", "ABC"}};
	char* pld1 = "abcabcabc";
	char* pld2 = "abcabcabc";
	Request r1 = {GET, "abc", "abc", hcnt, hs1, pld1, SUCCESS};
	Request r2 = {GET, "abc", "abc", hcnt, hs1, pld2, SUCCESS};
	assert(cmpRequest(&r1, &r2)==true);
	r1.payload="abcabcab";
	assert(cmpRequest(&r1, &r2)==false);
	r2.payload="abcabcab";
	assert(cmpRequest(&r1, &r2)==true);
	r1.method=DELETE;
	assert(cmpRequest(&r1, &r2)==false);
	r2.method=DELETE;
	assert(cmpRequest(&r1, &r2)==true);
	r1.uri = "other";
	assert(cmpRequest(&r1, &r2)==false);
	r2.uri = "other";
	assert(cmpRequest(&r1, &r2)==true);
	r1.flags = 32;
	assert(cmpRequest(&r1, &r2)==false);
	r2.flags = 32;
	assert(cmpRequest(&r1, &r2)==true);
	r1.header_count += 1;
	r1.headers = hs2;
	assert(cmpRequest(&r1, &r2)==false);
	r2.header_count += 1;
	r2.headers = (Header[]){{"ABC", "ABC"}, {"ABC", "ABC"}};
	assert(cmpRequest(&r1, &r2)==true);
	r1.headers[0].key = "a";
	assert(cmpRequest(&r1, &r2)==false);
	r2.headers[0].key = "a";
	assert(cmpRequest(&r1, &r2)==true);
	r1.headers[0].val = "a";
	assert(cmpRequest(&r1, &r2)==false);
	r2.headers[0].val = "a";
	assert(cmpRequest(&r1, &r2)==true);

}

void TestCopyHeader() {
	Header h1 = {"abc", "abc"};
	Header h2 = *copyHeader(&h1);
	assert(strcmp(h1.key, h2.key)==0);
	assert(strcmp(h1.val, h2.val)==0);
	h1.key = "a";
	assert(strcmp(h1.key, h2.key)!=0);
	assert(strcmp(h1.val, h2.val)==0);
	h1.val = "a";
	assert(strcmp(h1.key, h2.key)!=0);
	assert(strcmp(h1.val, h2.val)!=0);
}

void TestCopyRequest() {
	Header hs[] = {{"ABC", "ABC"},{"ABC", "ABC"}};
	char* p = "abcabcabc";
	Request r1 = {GET, "abc", "abc", 2, hs, p, SUCCESS};
	Request r2 = *copyRequest(&r1);
	assert(cmpRequest(&r1, &r2)==true);
	r1.flags = 43;
	assert(cmpRequest(&r1, &r2)==false);
	r2.flags = 43;
	assert(cmpRequest(&r1, &r2)==true);
	r1.method = DELETE;
	assert(cmpRequest(&r1, &r2)==false);
	r2.method = DELETE;
	assert(cmpRequest(&r1, &r2)==true);
	r1.uri = "uuuu";
	assert(cmpRequest(&r1, &r2)==false);
	r2.uri = "uuuu";
	assert(cmpRequest(&r1, &r2)==true);
	r1.version = "uuuu";
	assert(cmpRequest(&r1, &r2)==false);
	r2.version = "uuuu";
	assert(cmpRequest(&r1, &r2)==true);
	r1.header_count -= 1;
	r1.headers = (Header[]){{"abc", "abc"}};	
	assert(cmpRequest(&r1, &r2)==false);
	r2.header_count -= 1;
	r2.headers = (Header[]){{"abc", "abc"}};	
	assert(cmpRequest(&r1, &r2)==true);
}

void TestCopyResponse() {
	Header hs[] = {{"ABC", "ABC"},{"ABC", "ABC"}};
	char* p = "abcabcabc";
	Response r1 = {"version", 200, "sas", 2, hs, p, 0};
	Response r2 = *copyResponse(&r1);
	assert(cmpResponse(&r1, &r2)==true);
	r1.flags = 43;
	assert(cmpResponse(&r1, &r2)==false);
	r2.flags = 43;
	assert(cmpResponse(&r1, &r2)==true);
	r1.status = 123;
	assert(cmpResponse(&r1, &r2)==false);
	r2.status = 123;
	assert(cmpResponse(&r1, &r2)==true);
	r1.reason = "ajio";
	assert(cmpResponse(&r1, &r2)==false);
	r2.reason = "ajio";
	assert(cmpResponse(&r1, &r2)==true);
	r1.version = "uuuu";
	assert(cmpResponse(&r1, &r2)==false);
	r2.version = "uuuu";
	assert(cmpResponse(&r1, &r2)==true);
	r1.header_count -= 1;
	r1.headers = (Header[]){{"abc", "abc"}};	
	assert(cmpResponse(&r1, &r2)==false);
	r2.header_count -= 1;
	r2.headers = (Header[]){{"abc", "abc"}};	
	assert(cmpResponse(&r1, &r2)==true);
}

void TestCmpResponse() {
	int hcnt = 1;
	Header hs1[] = {{"ABC", "ABC"}};
	Header hs2[] = {{"ABC", "ABC"},{"ABC", "ABC"}};
	char* pld1 = "abcabcabc";
	char* pld2 = "abcabcabc";
	Response r1 = {"version", 200, "reason", hcnt, hs1, pld1, SUCCESS};
	Response r2 = {"version", 200, "reason", hcnt, hs1, pld1, SUCCESS};
	assert(cmpResponse(&r1, &r2)==true);
	r1.payload="abcabcab";
	assert(cmpResponse(&r1, &r2)==false);
	r2.payload="abcabcab";
	assert(cmpResponse(&r1, &r2)==true);
	r1.version = "version1";
	assert(cmpResponse(&r1, &r2)==false);
	r2.version = "version1";
	assert(cmpResponse(&r1, &r2)==true);
	r1.reason = "other";
	assert(cmpResponse(&r1, &r2)==false);
	r2.reason = "other";
	assert(cmpResponse(&r1, &r2)==true);
	r1.flags = 32;
	assert(cmpResponse(&r1, &r2)==false);
	r2.flags = 32;
	assert(cmpResponse(&r1, &r2)==true);
	r1.header_count += 1;
	r1.headers = hs2;
	assert(cmpResponse(&r1, &r2)==false);
	r2.header_count += 1;
	r2.headers = (Header[]) {{"ABC", "ABC"}, {"ABC", "ABC"}};
	assert(cmpResponse(&r1, &r2)==true);
	r1.headers[0].key = "a";
	assert(cmpResponse(&r1, &r2)==false);
	r2.headers[0].key = "a";
	assert(cmpResponse(&r1, &r2)==true);
	r1.headers[0].val = "a";
	assert(cmpResponse(&r1, &r2)==false);
	r2.headers[0].val = "a";
	assert(cmpResponse(&r1, &r2)==true);
}


void TestSerializeRequest(Request req_test, char* req_test_payload) {
	char* payload_test = serializeRequest(&req_test);
	assert(strcasecmp(payload_test, req_test_payload)==0);
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
	assert(strcasecmp(test->version, resp_test.version)==0);
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

void TestInvalidRequests() {
	char payload1[] = "Request\r\n\r\n";
	char* ptr1;
	Request* r = deserializeRequest(payload1, &ptr1);
	assert((r->flags&ERROR)==1);
	assert(r->method==INVALID);
	assert(r->version==NULL);
	assert(r->headers==NULL);
	assert(r->payload==NULL);
	assert(r->uri==NULL);
	assert(r->header_count==0);

}


int main() {
	TestCmpHeaders();
	TestCmpRequests();
	TestCmpResponse();
	TestCopyResponse();
	TestCopyHeader();
	TestCopyRequest();


	// default objects
	char default_payload[] = "don't trust cats";
	int header_count = 3;
	Header headers[] = {{"Content-Length"}};
	Response response = {"HTTPS/1.1", 200, "OK", header_count, headers, default_payload};
	Request request = {0, "/home/usr/passwd", "HTTPS/1.1", header_count, headers, default_payload};
	


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
	char* payload_resp3_in = "http/1.1 204 OK\r\n\r\nGet / YOUFUCKEDUP";
	char* payload_req3_in = "put / HTTP/1.1\r\n\r\nGET /../abc.txt HTTP/1.3";

	TestSerializeRequest(req3, payload_req3_out);
	TestSerializeResponse(resp3, payload_resp3_out);
	TestDeserializationRequest(req3, payload_req3_in, 18);
	TestDeserializationResponse(resp3, payload_resp3_in, 19);


	// test overlapping with payload
	char payload4[] = "Don't trust cats"; // length=16
	Header headers4[] = {{"Content-Length", "16"}, {"Javascript framework", " without Javascript"}};
	Request req4 = {2, "/home/usr", "HTTP/9.2", 2, headers4, payload4, 0};
	Response resp4 = {"HTTP/9.2", 123, "HUNGRY", 2, headers4, payload4, 0};
	char payload_req4_in[] = \
		"delete /home/usr HTTP/9.2\r\nContent-Length:16\r\nJavascript framework: without Javascript\r\n\r\nDon't trust cats"\
		"GET /a/stupid/file HTTP/0.0\r\nanother header: 1234\r\n\r\njust don't do it";
	char payload_req4_out[] = \
		"DELETE /home/usr HTTP/9.2\r\nContent-Length:16\r\nJavascript framework: without Javascript\r\n\r\nDon't trust cats";
	char payload_resp4_in[] = \
				  "http/9.2 123 HUNGRY\r\nContent-Length:16\r\nJavascript framework: without Javascript\r\n\r\nDon't trust cats"\
				  "HTTP/9.2 321 YRGNUH\r\nContent-Length:-1\r\nJavascript:NO\r\n\r\nIt will end bad";
	char payload_resp4_out[] = \
				  "HTTP/9.2 123 HUNGRY\r\nContent-Length:16\r\nJavascript framework: without Javascript\r\n\r\nDon't trust cats";
	TestSerializeRequest(req4, payload_req4_out);
	TestSerializeResponse(resp4, payload_resp4_out);
	TestDeserializationRequest(req4, payload_req4_in, 106);
	TestDeserializationResponse(resp4, payload_resp4_in, 100);

	
	TestInvalidRequests();
	/* template
	int Y = 0, Z = 0;
	char payloadX[] = ""; //length=0
	Header headersX[] = {};
	Request reqX = {};
	Response respX = {};
	char payload_reqX_in[] = "";
	char payload_respX_in[] = "";
	char payload_reqX_out[] = "";
	char payload_respX_out[] = "";
	TestSerializeRequest(reqX, payload_reqX_out);
	TestSerializeResponse(respX, payload_reqX_out);
	TestDeserializationRequest(reqX, payload_respX_in, Y);
	TestDeserializationResponse(respX, payload_respX_in, Z);
	*/
}

