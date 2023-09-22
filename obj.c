#include "obj.h"
#include "string.h"
#include "memory.h"

#define ALLOCATE_OBJ(T, t)\
	(T *)allocateObject(sizeof(T), t)

ObjType objType(Value v) {
	return getObj(v)->type;
}

uint8_t isString(Value v) {
	return isObj(v) && (getObj(v)->type == OBJ_STRING);
}

ObjString *getString(Value v) {
	return (ObjString *)(getObj(v));
}

char *getCString(Value v) {
	return ((ObjString *)(getObj(v)))->cstr;
}

ObjString *copyString(char *start, size_t size) {
	char *chars = ALLOCATE(char, size+1);
	memcpy(chars, start, size);
	chars[size] = '\0';
	return allocateString(chars, size);
}

ObjString *allocateString(char *cstr, size_t size) {
	ObjString *string = ALLOCATE_OBJ(ObjString, OBJ_STRING);
	string->cstr = cstr;
	string->length = size;
	return string;
}

Obj *allocateObject(size_t size, ObjType type) {
	Obj *object = (Obj *)reallocate(NULL, 0, size);
	object->type = type;
	return object;
}

void printObj(Value v) {
	switch(objType(v)) {
		case OBJ_STRING: printf("%s", getCString(v)); break;
		default: break;
	}
}
