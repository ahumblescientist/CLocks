#include "debug.h"

#include <stdio.h>
#include "value.h"


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

size_t constantInstruction(char *name, Chunk *chunk, size_t current) {
	uint8_t constant = chunk->code[current+1];
	printf("%-16s i:%04d '", name, constant);
	printValue(chunk->constants.values[constant]);
	printf("'\n");
	return 2;
}

size_t constantLongInstruction(char *name, Chunk *chunk, size_t current) {
	uint8_t constant1 = chunk->code[current+1];
	uint8_t constant2 = chunk->code[current+2];
	uint16_t index = (constant2 << 8) | constant1;
	printf("%-16s i:%04d '", name, index);
	printValue(chunk->constants.values[index]);
	printf("'\n");
	return 3;
}

void disassembleInstruction(Chunk *chunk, size_t *i) {
	uint8_t instruction = chunk->code[*i];
	printf("%04zu ", chunk->lines[*i]);
	switch(instruction) {
		case OP_RETURN:
			*i += simpleInstruction("OP_RETURN");
			break;
		case OP_CONSTANT:
			*i += constantInstruction("OP_CONSTANT", chunk, *i);
			break;
		case OP_CONSTANT_LONG:
			*i += constantLongInstruction("OP_CONSTANT_LONG", chunk, *i);
			break;
		case OP_TRUE:
			*i += simpleInstruction("OP_TRUE");
			break;
		case OP_FALSE:
			*i += simpleInstruction("OP_FALSE");
			break;
		case OP_NIL:
			*i += simpleInstruction("OP_NIL");
			break;
		default:
			break;
	}
}

