#include <stdio.h>
#include "chunk.h"
#include "debug.h"
#include "vm.h"

int main() {
	initVM();
	Chunk chunk;
	initChunk(&chunk);
	writeConstant(&chunk, 3.4, 1);
	writeConstant(&chunk, 3.4, 1);
	writeChunk(&chunk, OP_ADD, 1);
	writeConstant(&chunk, 3.4, 1);
	writeChunk(&chunk, OP_DIVIDE, 1);
	writeChunk(&chunk, OP_RETURN, 1);
	interpret(&chunk);
	freeChunk(&chunk);
	freeVM();
	return 0;
}
