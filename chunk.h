#ifndef CHUNK_H
#define CHUNK_H

#include <stdint.h>
#include <stdlib.h>

#include "memory.h"

typedef enum {
	OP_RETURN,
} OPCODE;

typedef struct {
	size_t used;
	size_t count;
	uint8_t *code;
} Chunk;

void initChunk(Chunk *);
void writeChunk(Chunk *, uint8_t);

#endif
