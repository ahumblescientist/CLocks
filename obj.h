#ifndef OBJ_H
#define OBJ_H

#include <stdint.h>

#include "value.h"

typedef enum {
	OBJ_STRING,
} ObjType;

struct Obj {
	ObjType type;
	Obj *next;
};

typedef struct {
	struct Obj obj;
	size_t length;
	char *cstr;
	uint32_t hash;
} ObjString;

ObjType objType(Value);
ObjString *getString(Value);
ObjString *copyString(char *, size_t);
ObjString *takeString(char *, size_t);
ObjString *allocateString(char *, size_t, uint32_t);
Obj *allocateObject(size_t, ObjType);

char *getCString(Value);
uint8_t isString(Value);

void printObj(Value);

#endif
