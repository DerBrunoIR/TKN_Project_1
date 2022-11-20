
#ifndef LIST 
#define LIST
#include <stdlib.h>

// types
typedef struct ListNode_s {
	void* data;
	struct ListNode_s* next;
} ListNode;


typedef struct List_s {
	struct ListNode_s* head;
	int size;
} List;

typedef int cmpfnct (void* data); 
typedef void* rdcfnct (void* data1, void* data2); 

// functions
List*   createList	(int size);
int	lengthList	(List* l);
void* 	getList		(List* l, int idx);
int 	setList		(List* l, int idx, void* data);
int   	searchList	(List* l, cmpfnct f);
void  	appendList	(List* l, void* data);
void* 	popList		(List* l);
int  	removeList	(List* l, int idx);
int  	cmpList		(List* l1, List* l2, cmpfnct f);
void  	freeList	(List* l);
void*  	reduceList	(List* l, rdcfnct f);
List*  	copyList	(List* l);

// TODO add catList
int	catList		(List* l1, List* l2);

#endif
