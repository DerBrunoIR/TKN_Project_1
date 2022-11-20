#include "list.h"


List* createList(int size) {
	if (size < 0)
		return NULL;
	List* l = malloc(sizeof(List));
	l->size = size;
	l->head = NULL;
	if (size > 0) {
		l->head = calloc(1, sizeof(ListNode));
		ListNode* head = l->head;
		for (int i = 0; i < size-1; i++) {
			head->next = calloc(1,sizeof(ListNode));
			head = head->next;
		}
		head->next = NULL;
	}
	return l;
}

int lengthList(List* l) {
	return l->size;
}

void* getList(List* l, int idx) {
	if (idx < 0 || idx >= l->size)
		return NULL;
	ListNode* head = l->head;
	for (int i = 0; i < idx; i++) {
		head = head->next;
	}
	return head->data;
}

int setList(List* l, int idx, void* data) {
	if (idx < 0 || idx >= l->size) 
		return -1;
	ListNode* head = l->head;
	for (int i = 0; i < idx; i++) {
		head = head->next;
	}
	head->data = data;
	return 0;
}

int searchList(List* l, cmpfnct f) {
	if (l->size == 0)
		return -1;
	ListNode* head = l->head;
	for (int i = 0; head; i++) {
		if (f(head->data) == 1)
			return i;
		head = head->next;
	}
	return -1;
}

void appendList(List* l, void* data) {
	if (l->size == 0) {
		l->head = calloc(1, sizeof(ListNode));
		l->head->data = data;
	} else {
		ListNode* head = l->head;
		while (head->next)
			head = head->next;
		head->next = calloc(1, sizeof(ListNode));
		head = head->next;
		head->data = data;
	}
	l->size++;
}

void* popList(List* l) {
	void* res;

	if (l->size == 0)
		return NULL;

	ListNode* head = l->head;
	if (l->size == 1) {
		res = head->data;
		free(head);
		l->head = NULL;
	} else {
		while(head->next->next != NULL) {
			head = head->next;
		}
		ListNode* nxt = head->next;
		res = nxt->data;
		free(nxt);
		head->next = NULL;
	}
	l->size--;
	return res;
}

int removeList(List* l, int idx) {
	if (idx < 0 || idx >= l->size)
		return -1;
	if (idx == 0) {
		ListNode* nxt = l->head->next;
		free(l->head);
		l->head = nxt;
	} else {
		ListNode* prv = l->head;
		ListNode* nxt = prv->next;
		for (int i = 0; i < idx-1; i++) {
			prv = nxt;
			nxt = nxt->next;
		}
		prv->next = nxt->next;
		free(nxt);
	}
	l->size--;
	return 0;
}

int cmpList(List* l1, List* l2, cmpfnct f) {
	int left = 0;
	int right = 0;
	ListNode* headl = l1->head;
	ListNode* headr = l2->head;
	while (headl && headr) {
		left += f(headl->data);
		right += f(headr->data);
		headl = headl->next;
		headr = headr->next;
	}
	return left - right;
}

void* reduceList(List* l, rdcfnct f) {
	if (l->head==NULL)
		return NULL;
	if (l->head->next==NULL)
		return l->head->data;
	ListNode* head = l->head;
	void* tmp = head->data;
	while (head->next) {
		head = head->next;		
		tmp = f(tmp, head->data);
	}
	return tmp;
}

List* copyList(List* l) {
	List* cpy = createList(l->size);
	ListNode* head = l->head;
	ListNode* new = cpy->head;
	while (head) {
		new->data = head->data;
		head = head->next;
		new = new->next;
	}
	return cpy;
}

int catList(List* l1, List* l2) {
	ListNode* tail = l1->head;
	while(tail->next)
		tail = tail->next;
	tail->next = l2->head;

	l1->size += l2->size;
	l2->head = NULL;
	l2->size = 0;
	return 0;
}

void freeList(List* l) {
	if (l==NULL)
		exit(EXIT_FAILURE);
	ListNode* head = l->head;
	ListNode* prev;
	while (head) {
		prev = head;
		head = head->next;
		free(prev);
	}
	l->size = 0;
	free(l);
}

