#include <stdio.h>
#include <stdlib.h>
#include "chunk.h"
#include "debug.h"
#include "vm.h"
#include "compiler.h"

void runFile(char *filename) {
	FILE *file = fopen(filename, "r");
	if(file == NULL) {
		printf("failed to open file %s\n", filename);
		exit(1);
	}
	fseek(file, 0L, SEEK_END);
	size_t filesize = ftell(file);
	fseek(file, 0L, SEEK_SET);
	char *source;
	source = (char *)malloc((filesize+1) * sizeof(char));
	if(source == NULL) {
		printf("failed to allocate memory\n");
		exit(1);
	}
	if((fread(source, 1, filesize, file)) < filesize) {
		printf("failed to read from file %s", filename);
		exit(1);
	}
	source[filesize] = '\0';
	fclose(file);
	InterpretResult result = interpret(source);
	if(result != INTERPRET_OK) exit(1);
}

void runPrompt() {
#define LINE_SIZE 1024
	char line[LINE_SIZE];
	while(1) {
		printf("Lox > ");
		if(!fgets(line, sizeof(char) * LINE_SIZE, stdin)) {
			printf("\n");
			break;
		}
		interpret(line);
	}
}

int main(int argc, char **argv) {
	initVM();
	if(argc == 2) {
		runFile(argv[1]);
	} else if(argc == 1) {
		runPrompt();
	} else {
		printf("Usage: %s [file.lox]", argv[0]);
	}
	return 0;
}
