#include "obj.h"
#include "string.h"
#include "memory.h"
#include "vm.h"

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

uint32_t hashString(char *str, size_t length) {
  uint32_t hash = 2166136261u;
  for (int i = 0; i < length; i++) {
    hash ^= (uint8_t)str[i];
    hash *= 16777619;
  }
	return hash;
}

ObjString *copyString(char *start, size_t size) {
	uint32_t hash = hashString(start, size);
	ObjString *interned = tableFindString(&vm.strings, start, size, hash);
	if(interned != NULL) return interned;
	char *chars = ALLOCATE(char, size+1);
	memcpy(chars, start, size);
	chars[size] = '\0';
	return allocateString(chars, size, hash);
}

ObjString *takeString(char *start, size_t size) {
	uint32_t hash = hashString(start, size);
	ObjString *interned = tableFindString(&vm.strings, start, size, hash);
	if(interned != NULL) {
		FREE_ARRAY(char, start, size+1);
		return interned;
	}
	return allocateString(start, size, hash);
}

ObjString *allocateString(char *cstr, size_t size, uint32_t hash) {
	ObjString *string = ALLOCATE_OBJ(ObjString, OBJ_STRING);
	string->cstr = cstr;
	string->length = size;
	string->hash = hash;
	tableSet(&vm.strings, string, makeNil());
	return string;
}

Obj *allocateObject(size_t size, ObjType type) {
	Obj *object = (Obj *)reallocate(NULL, 0, size);
	object->type = type;
	object->next = vm.objHead;
	vm.objHead = object;
	return object;
}

void printObj(Value v) {
	switch(objType(v)) {
		case OBJ_STRING: printf("%s", getCString(v)); break;
		default: break;
	}
}
