CFILES = main.c chunk.c memory.c debug.c 
HFILES = Makefile chunk.h memory.h debug.h
FILES = $(CFILES) $(HFILES)
CC = clang
OUT = bin/main
FLAGS = -ggdb3 -O0 -o $(OUT)


main: $(FILES)
	$(CC) $(CFILES) $(FLAGS)
