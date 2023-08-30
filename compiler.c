#include "compiler.h"

void compile(char *source) {
	initLexer(source);
	size_t line = 0;	
	while(1) {
		Token token = scanToken();
		if(token.type == TOKEN_EOF) {
			break;
		}
		line = token.line;
		printf("L:%4zu len: %2zu val: '%.*s' Type: %d \n", line, token.length, (int)token.length, token.start, token.type);
	}
}
