#include "chunk.h"


void initChunk(Chunk *chunk) {
	chunk->count = 0;
	chunk->used= 0;
	chunk->code = (uint8_t *)0;
	chunk->lines = NULL;
	initValueArray(&chunk->constants);
}

void freeChunk(Chunk *chunk) {
	FREE_ARRAY(uint8_t, chunk->code, chunk->count);
	freeValueArray(&chunk->constants);
}

void writeChunk(Chunk *chunk, uint8_t byte, size_t line) {
	if(chunk->count == chunk->used) {
		size_t oldcount = chunk->count;
		chunk->count = GROW_CAPACITY(chunk->count);
		chunk->code = GROW_ARRAY(uint8_t, chunk->code, oldcount, chunk->count);
		chunk->lines = GROW_ARRAY(size_t, chunk->lines, oldcount, chunk->count);
	}
	chunk->code[chunk->used] = byte;
	chunk->lines[chunk->used] = line;
	chunk->used++;
}


uint32_t addConstant(Chunk *chunk, Value val) {
	writeValueArray(&chunk->constants, val);
	return chunk->constants.used - 1;
}

uint8_t writeConstant(Chunk *chunk, Value v, size_t line) {
	uint16_t index = addConstant(chunk, v);
	if(index > 255) {
		writeChunk(chunk, OP_CONSTANT_LONG, line);
		writeChunk(chunk, index & 0xFF, line);
		writeChunk(chunk, index >> 8, line);
	} else if(index < UINT16_MAX) {
		writeChunk(chunk, OP_CONSTANT, line);
		writeChunk(chunk, index, line);
	} else {
		return 1;
	}
	return 0;
}
