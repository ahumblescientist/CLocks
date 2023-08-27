#include "debug.h"

#include <stdio.h>


void disassembleChunk(Chunk *chunk, char *name) {
	printf("==== %s ====\n", name);
	for(size_t i=0;i<chunk->used;){
		disassembleInstruction(chunk, &i);
	}
}

size_t simpleInstruction(char *name) {
	printf("%s\n", name);
	return 1;
}

void disassembleInstruction(Chunk *chunk, size_t *i) {
	uint8_t instruction = chunk->code[*i];
	switch(instruction) {
		case OP_RETURN:
			*i += simpleInstruction("OP_RETURN");
			break;
		default:
			break;
	}
}

