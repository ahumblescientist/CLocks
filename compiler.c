#include "compiler.h"

void compile(char *source) {
	initLexer(source);
	size_t line = 0;	
	while(1) {
		Token token = scanToken();
		line = token.line;
		printf("%4zu %2zu '%.s'\n", line, token.length, token.start);
	}
}
