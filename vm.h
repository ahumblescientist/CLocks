#ifndef VM_H
#define VM_H

#include "chunk.h"
#include "debug.h"
#include "value.h"

#include <stdio.h>

#define STACK_MAX 0x100

typedef struct {
	Chunk *chunk;
	uint8_t *pc;
	Value stack[STACK_MAX];
	Value *sp;
} VM;

typedef enum {
	INTERPRET_OK,
	INTERPRET_COMPILE_ERROR,
	INTERPRET_RUNTIME_ERROR,
} InterpretResult;


void initVM();
void freeVM();
void push(Value);
Value pop();

InterpretResult interpret(Chunk *chunk);

#endif
