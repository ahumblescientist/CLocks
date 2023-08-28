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
	printf("%g", v);
}
