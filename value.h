#ifndef VALUE_H
#define VALUE_H

#include "memory.h"
#include <stdlib.h>
#include <stdio.h>

typedef double Value;

typedef struct {
	size_t count;
	size_t used;
	Value *values;
} ValueArray;

void initValueArray(ValueArray *);
void writeValueArray(ValueArray *, Value);
void freeValueArray(ValueArray *);

void printValue(Value);

#endif
