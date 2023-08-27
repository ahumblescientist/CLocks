#ifndef CHUNK_H
#define CHUNK_H

#include <stdint.h>
#include <stdlib.h>
#include "value.h"

#include "memory.h"

typedef enum {
	OP_CONSTANT,
	OP_CONSTANT_LONG,
	OP_RETURN,
} OPCODE;

typedef struct {
	size_t used;
	size_t count;
	uint8_t *code;
	ValueArray constants;
	size_t *lines;
} Chunk;

void initChunk(Chunk *);
void writeChunk(Chunk *, uint8_t, size_t );
uint16_t addConstant(Chunk *, Value); // return index of the constant 
void writeConstant(Chunk *, Value v, size_t);

#endif
