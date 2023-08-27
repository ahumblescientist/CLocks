#include <stdio.h>
#include "chunk.h"
#include "debug.h"

int main() {
	Chunk chunk;
	initChunk(&chunk);
	writeConstant(&chunk, 1.22, 1);
	disassembleChunk(&chunk, "TEST");
	return 0;
}
