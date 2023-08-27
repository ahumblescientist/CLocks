#ifndef DEBUG_H
#define DEBUG_H

#include "chunk.h"

void disassembleChunk(Chunk *chunk, char *name);
void disassembleInstruction(Chunk *chunk, size_t *offset);


#endif
