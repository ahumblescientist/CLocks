#ifndef TABLE_H
#define TABLE_H

#include <stdio.h>
#include <stdint.h>

#include "value.h"
#include "obj.h"

#define TABLE_MAX_LOAD 0.75

typedef struct {
	ObjString *key;
	Value value;
} Entry;

typedef struct {
	size_t used;
	size_t count;
	Entry *entries;
} Table;


void tableInit(Table *);
void tableFree(Table *);
void tableAddAll(Table *, Table *);
uint8_t tableSet(Table *, ObjString *, Value);
ObjString *tableFindString(Table *, char *, size_t, uint32_t);

#endif
