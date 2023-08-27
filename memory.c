#include "memory.h"

void *reallocate(void *P, size_t psize, size_t csize) {
	if(csize == 0) {
		free(P);
		return (void *)0;
	}
	void *res = realloc(P, csize);
	if(res == NULL) {
		exit(1);
	}
	return res;
}
