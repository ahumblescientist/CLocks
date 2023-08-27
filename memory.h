#ifndef MEMORY_H
#define MEMORY_H

#include <stdlib.h>
#include <stdint.h>

#define GROW_CAPACITY(cap) ((cap) < 8 ? 8 : cap * 2)

#define GROW_ARRAY(type, P, prev, current)\
	(type *)reallocate(P, prev * sizeof(type), current * sizeof(type))

#define FREE_ARRAY(type, P, prev)\
	reallocate(P, sizeof(type) * prev, 0);


void *reallocate(void *, size_t, size_t);

#endif
