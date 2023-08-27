#include "chunk.h"


void initChunk(Chunk *chunk) {
	chunk->count = 0;
	chunk->used= 0;
	chunk->code = (uint8_t *)0;
	initValueArray(&chunk->constants);
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
	freeValueArray(&chunk->constants);
}

uint16_t addConstant(Chunk *chunk, Value val) {
	writeValueArray(&chunk->constants, val);
	return chunk->constants.used - 1;
}

void writeConstant(Chunk *chunk, Value v) {
	uint16_t index = addConstant(chunk, v);
	if(index > 255) {
		writeChunk(chunk, OP_CONSTANT_LONG);
		writeChunk(chunk, index & 0xFF);
		writeChunk(chunk, index >> 8);
	} else {
		writeChunk(chunk, OP_CONSTANT);
		writeChunk(chunk, index);
	}
}
