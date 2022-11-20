#include <assert.h>
#include "list.h"


int arr[] = {0, 1,2,3,4,5,6,7,8,9};
char* str = "abcdefghijklmnop";

void TestCreateList() {
	assert(NULL==createList(-1));

	List* s = createList(0);
	assert(s->size==0);
	assert(s->head==NULL);
	freeList(s);

	List* s0 = createList(1);
	assert(s0->size==1);
	assert(s0->head);
	assert(s0->head->next==NULL);
	assert(s0->head->data==NULL);
	freeList(s0);
	
	List* s1 = createList(4);
	assert(s1->size==4);
	ListNode* head = s1->head;
	for (int i = 0; i < 3; i++) {
		assert(head);
		assert(head->data==NULL);
		head = head->next;
	}
	assert(head);
	assert(head->data==NULL);
	assert(head->next==NULL);
	freeList(s1);
}

void TestLengthList() {
	List* s0 = createList(0);
	assert(lengthList(s0) == 0);
	freeList(s0);
	
	List* s1 = createList(1);
	assert(lengthList(s1) == 1);
	freeList(s1);

	List* s2 = createList(322);
	assert(lengthList(s2) == 322);
	freeList(s2);
}

void TestSetList() {
	List* s1 = createList(3);
	assert(setList(s1, -1, str-1)==-1);
	assert(setList(s1, 0, str+0)==0);
	assert(s1->head->data==str+0);
	assert(setList(s1, 1, str+1)==0);
	assert(s1->head->next->data==str+1);
	assert(setList(s1, 2, str+2)==0);
	assert(s1->head->next->next->data==str+2);
	assert(setList(s1, 3, str+3)==-1);
	freeList(s1);

	List* s0 = createList(100);
	assert(setList(s0, -1, str)==-1);
	assert(setList(s0, 99, str)==0);
	assert(setList(s0, 100, str)==-1);
	assert(setList(s0, 101, str)==-1);
	for (int i = 0; i < 100; i++)
		assert(setList(s0, i, str)==0);
	freeList(s0);
}

void TestGetList() {
	List* s0 = createList(10);
	for (int i = 0; i < 10; i++)
		setList(s0, i, str+i);
	for (int i = 0; i < 10; i++)
		assert(getList(s0, i)==str+i);
	assert(getList(s0, -1)==NULL);
	assert(getList(s0, 10)==NULL);
	freeList(s0);
}

int testSearchCmpFnct1(void* a) {
	return *((int*)a)==9;
}

int testSearchCmpFnct2(void* a) {
	return *((int*)a)==0;
}

void TestSearchList(){
	List* s0 = createList(10);
	for (int i = 0; i < 10; i++)
		setList(s0, i, arr+i);
	assert(searchList(s0, testSearchCmpFnct1)==9);
	assert(searchList(s0, testSearchCmpFnct2)==0);
	freeList(s0);

	List* s1 = createList(7);
	for (int i = 0; i < 8; i++)
		setList(s1, i, arr+i);
	assert(searchList(s1, testSearchCmpFnct1)==-1);
	freeList(s1);

	List* s2 = createList(0);
	assert(searchList(s2, testSearchCmpFnct1)==-1);
}

void TestAppendList() {
	List* s0 = createList(0);
	assert(s0->size==0);

	appendList(s0, str+1);
	assert(s0->size==1);
	assert(s0->head->data==str+1);

	appendList(s0, str+3);
	assert(s0->size==2);
	assert(s0->head->next->data==str+3);

	appendList(s0, str+4);
	assert(s0->size==3);
	assert(s0->head->next->next->data==str+4);
	freeList(s0);

	List* s1 = createList(0);
	assert(s1->size==0);
	for (int i = 0; i < 100; i++)
		appendList(s1, str);
	assert(s1->size==100);
	freeList(s1);
}

void TestPopList(){
	List* s0 = createList(10);
	for (int i = 0; i < 10; i++)
		setList(s0, i, str+i);
	assert(s0->size==10);
	for (int i = 0; i < 10; i++)
		assert(popList(s0)==str+9-i);
	assert(s0->size==0);
	assert(popList(s0)==NULL);
	freeList(s0);
}

void TestRemoveList() {
	List* s0 = createList(3);
	setList(s0, 0, str);
	setList(s0, 1, str+1);
	setList(s0, 2, str+2);
	assert(s0->size==3);
	assert(removeList(s0, 0)==0);
	assert(s0->size==2);
	assert(s0->head->data==str+1);
	assert(removeList(s0, 1)==0);
	assert(s0->head->next==NULL);
	assert(removeList(s0, 0)==0);
	assert(s0->head==NULL);
	assert(removeList(s0, 0)==-1);
	assert(removeList(s0, -1)==-1);
}

int testCmpFnct(void *a) {
	return *((int*) a);
}

void TestCmpList() {
	List* a = createList(5);
	List* b = createList(5);
	List* c = createList(5);
	for (int i = 0; i < 5; i++) {
		setList(a, i, arr+0);
		setList(b, i, arr+0);
		setList(c, i, arr+4);
	}
	assert(cmpList(a, b, testCmpFnct)==0);
	assert(cmpList(a, c, testCmpFnct)==-20);
	freeList(a);
	freeList(b);
	freeList(c);
}

void TestFreeList() {
	List* a = createList(12349);
	freeList(a);
}

void* testReduceFnct(void* a, void* b) {
	return arr + (*((int*)a) + *((int*) b)) % 10;
}

void TestReduceList() {
	List* s = createList(4);
	for (int i = 0; i < 4; i++)
		setList(s, i, arr+2);
	assert(reduceList(s, testReduceFnct)==arr+8);
	freeList(s);

	List* a = createList(0);
	assert(reduceList(a, testReduceFnct)==NULL);
	appendList(a, arr+1);
	assert(reduceList(a, testReduceFnct)==arr+1);
	freeList(a);
}

void TestCopyList() {
	List* a = createList(2);
	appendList(a, arr+1);
	appendList(a, arr+1);
	
	List* b = copyList(a);
	assert(b->head->data == a->head->data);
	assert(b->head->next->data == a->head->next->data);
	setList(b, 0, arr+3);
	assert(b->head->data != a->head->data);
}

void TestCatList() {
	List* a = createList(3);
	assert(a->size==3);
	List* b = createList(3);
	assert(a->size==3);
	assert(catList(a, b)==0);
	assert(a->size==6);
	assert(a->head->next->next->next);
	assert(b->size==0);
	assert(b->head==NULL);
}

int main() {
	TestCreateList();
	TestLengthList();
	TestSetList();
	TestGetList();
	TestSearchList();
	TestAppendList();
	TestPopList();
	TestRemoveList();
	TestCmpList();
	TestFreeList();
	TestReduceList();
	TestCopyList();
	TestCatList();
}
