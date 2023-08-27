#include "chunk.h"


void initChunk(Chunk *chunk) {
	chunk->count = 0;
	chunk->used= 0;
	chunk->code = (uint8_t *)0;
}

void writeChunk(Chunk *chunk, uint8_t byte) {
	if(chunk->count == chunk->used) {
		size_t oldcount = chunk->count;
		chunk->count = GROW_CAPACITY(chunk->count);
		chunk->code = GROW_ARRAY(uint8_t, chunk->code, oldcount, chunk->count);
	}
	chunk->code[chunk->used++] = byte;
}

void freeChunk(Chunk *chunk) {
	FREE_ARRAY(uint8_t, chunk->code, chunk->count);
}
