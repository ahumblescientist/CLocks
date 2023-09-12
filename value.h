#ifndef VALUE_H
#define VALUE_H

#include "memory.h"
#include <stdlib.h>
#include <stdio.h>

typedef enum {
	VALUE_NUMBER,
	VALUE_BOOL,
	VALUE_NIL,
} ValueType;

typedef struct {
	ValueType type;
	union {
		uint8_t boolean;
		double number;
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

Value makeBool(uint8_t), makeNumber(double), makeNil();

uint8_t getBool(Value);
double getNumber(Value);
uint8_t isBool(Value), isNumber(Value), isNil(Value);

#endif
