#ifndef VALUE_H
#define VALUE_H

#include "memory.h"
#include <stdlib.h>
#include <stdio.h>

typedef struct Obj Obj;

typedef enum {
	VALUE_NUMBER,
	VALUE_BOOL,
	VALUE_NIL,
	VALUE_OBJ,
} ValueType;

typedef struct {
	ValueType type;
	union {
		uint8_t boolean;
		double number;
		Obj *obj;
	} as;
} Value;

typedef struct {
	size_t count;
	size_t used;
	Value *values;
} ValueArray;


void initValueArray(ValueArray *);
void writeValueArray(ValueArray *, Value);
void freeValueArray(ValueArray *);
void printValue(Value);

Value makeBool(uint8_t), makeNumber(double), makeNil(), makeObj(Obj *);

uint8_t getBool(Value);
double getNumber(Value);
Obj *getObj(Value);

uint8_t isBool(Value), isNumber(Value), isNil(Value), isObj(Value);

#endif
