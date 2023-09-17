#include "value.h"

void initValueArray(ValueArray *arr) {
	arr->used = 0;
	arr->count = 0;
	arr->values = (Value *)0;
}

void writeValueArray(ValueArray *arr, Value val) {
	if(arr->used == arr->count) {
		size_t oldCount = arr->count;
		arr->count = GROW_CAPACITY(arr->count);
		arr->values = GROW_ARRAY(Value, arr->values, oldCount, arr->count);
	}
	arr->values[arr->used++] = val;
}

void freeValueArray(ValueArray *arr) {
	FREE_ARRAY(Value, arr->values, arr->count);
}

void printValue(Value v) {
	switch(v.type) {
		case VALUE_BOOL: printf(getBool(v) ? "True" : "False"); break;
		case VALUE_NIL: printf("Nil"); break;
		case VALUE_NUMBER: printf("%f", getNumber(v)); break;
		default: break;
	}
}

Value makeBool(uint8_t v) {
	Value ret;
	ret.type = VALUE_BOOL;
	ret.as.boolean = v ? 0xFF : 0x00;
	return ret;
}

Value makeNumber(double v) {
	Value ret;
	ret.type = VALUE_NUMBER;
	ret.as.number = v;
	return ret;
}

Value makeNil() {
	Value ret;
	ret.type = VALUE_NIL;
	return ret;
}

uint8_t getBool(Value val) {
	return val.as.boolean;
} 

double getNumber(Value val) {
	return val.as.number;	
}

uint8_t isNumber(Value v) {
	return v.type == VALUE_NUMBER;
}


uint8_t isBool(Value v) {
	return v.type == VALUE_BOOL;
}

uint8_t isNil(Value v) {
	return v.type == VALUE_NIL;
}
