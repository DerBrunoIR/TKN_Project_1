#include "http.h"
#include "assert.h"


void TestfindHeader() {
	Header h[] = {{"key", "val"}, {"key", "val"}, {"cat", "dog"}};
	assert(findHeader(h, (Header*){"cat", "fox"})==2);
	assert(findHeader(h, (Header*){"key", "keyhole"})==0);
}
void TestsetHeader();
void TestaddHeader();
void TestremoveHeader();
void TestcopyHeader();
void TestcopyRequest();
void TestcopyResponse();


int main() {
	TestfindHeader();
	TestsetHeader();
	TestaddHeader();
	TestcopyHeader();
	TestcopyRequest();
	TestcopyResponse();
}
