#ifndef COMPILER_H
#define COMPILER_H

#include "lexer.h"
#include "chunk.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

uint8_t compile(char *source, Chunk *chunk);

#endif
