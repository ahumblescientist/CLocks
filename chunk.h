#ifndef CHUNK_H
#define CHUNK_H

#include <stdint.h>
#include <stdlib.h>
#include "value.h"

#include "memory.h"

typedef enum {
	OP_CONSTANT,
	OP_CONSTANT_LONG,
	OP_NEGATE,
	OP_ADD,
	OP_SUBTRACT,
	OP_MULTIPLY,
	OP_DIVIDE,
	OP_NIL,
	OP_FALSE,
	OP_TRUE,
	OP_NOT,
	OP_EQUAL,
	OP_GREATER,
	OP_LESS,
	OP_RETURN,
	OP_PRINT,
	OP_POP,
	OP_DEFINE_GLOBAL,
	OP_GET_GLOBAL,
} OPCODE;

typedef struct {
	size_t used;
	size_t count;
	uint8_t *code;
	ValueArray constants;
	size_t *lines;
} Chunk;

void initChunk(Chunk *);
void freeChunk(Chunk *);
void writeChunk(Chunk *, uint8_t, size_t );
uint32_t addConstant(Chunk *, Value); // return index of the constant 
uint8_t writeConstant(Chunk *, Value v, size_t);

#endif
