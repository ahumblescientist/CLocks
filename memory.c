#include "memory.h"
#include "obj.h"
#include "vm.h"


void *reallocate(void *P, size_t psize, size_t csize) {
	if(csize == 0) {
		free(P);
		return (void *)0;
	}
	void *res = realloc(P, csize);
	if(res == NULL) {
		exit(1);
	}
	return res;
}

void freeObject(Obj *obj) {
	switch(obj->type) {
		case OBJ_STRING: {
			ObjString *str = (ObjString *)obj;
			FREE_ARRAY(char, str->cstr, str->length+1);
			FREE(ObjString, str->cstr);
		}
	}
}

void freeObjects() {
	Obj *head = vm.objHead;
	Obj *temp;
	while(head != NULL) {
		temp = head;
		head = head->next;
		freeObject(temp);
	}
}
