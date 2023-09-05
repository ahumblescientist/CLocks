#include "compiler.h"


Chunk *compillingChunk;



struct Parser {
	Token current;
	Token prev;
	uint8_t hadError;
  uint8_t panicMode;
};

struct Parser parser;

typedef enum {
	PREC_NONE,
	PREC_ASSIGNMENT,
	PREC_OR,
	PREC_AND,
	PREC_EQUALITY,
	PREC_COMPARISON,
	PREC_TERM,
	PREC_FACTOR,
	PREC_UNARY,
	PREC_CALL,
	PREC_PRIMARY
} Precedence;

typedef void (*ParseFn)(void);

typedef struct {
	ParseFn prefix;
	ParseFn infix;
	Precedence precedence;
} ParseRule;

Chunk *currentChunk();

void errorAt(Token *, char *), errorAtCurrnt(char *), error(char *),
addByte(uint8_t), endCompiler(), addReturn();

static void advance(), consume(TokenType, char *), number();
void expression(), parsePrec(Precedence), grouping(), unary(), binary();

ParseRule rules[TOKEN_EOF+1] = {
  [TOKEN_LEFT_PAREN]    = {grouping, NULL,   PREC_NONE},
  [TOKEN_RIGHT_PAREN]   = {NULL,     NULL,   PREC_NONE},
  [TOKEN_LEFT_BRACE]    = {NULL,     NULL,   PREC_NONE},
  [TOKEN_RIGHT_BRACE]   = {NULL,     NULL,   PREC_NONE},
  [TOKEN_COMMA]         = {NULL,     NULL,   PREC_NONE},
  [TOKEN_DOT]           = {NULL,     NULL,   PREC_NONE},
  [TOKEN_MINUS]         = {unary,    binary, PREC_TERM},
  [TOKEN_PLUS]          = {NULL,     binary, PREC_TERM},
  [TOKEN_SEMICOLON]     = {NULL,     NULL,   PREC_NONE},
  [TOKEN_SLASH]         = {NULL,     binary, PREC_FACTOR},
  [TOKEN_STAR]          = {NULL,     binary, PREC_FACTOR},
  [TOKEN_BANG]          = {NULL,     NULL,   PREC_NONE},
  [TOKEN_BANG_EQUAL]    = {NULL,     NULL,   PREC_NONE},
  [TOKEN_EQUAL]         = {NULL,     NULL,   PREC_NONE},
  [TOKEN_EQUAL_EQUAL]   = {NULL,     NULL,   PREC_NONE},
  [TOKEN_GREATER]       = {NULL,     NULL,   PREC_NONE},
  [TOKEN_GREATER_EQUAL] = {NULL,     NULL,   PREC_NONE},
  [TOKEN_LESS]          = {NULL,     NULL,   PREC_NONE},
  [TOKEN_LESS_EQUAL]    = {NULL,     NULL,   PREC_NONE},
  [TOKEN_IDENTIFIER]    = {NULL,     NULL,   PREC_NONE},
  [TOKEN_STRING]        = {NULL,     NULL,   PREC_NONE},
  [TOKEN_NUMBER]        = {number,   NULL,   PREC_NONE},
  [TOKEN_AND]           = {NULL,     NULL,   PREC_NONE},
  [TOKEN_CLASS]         = {NULL,     NULL,   PREC_NONE},
  [TOKEN_ELSE]          = {NULL,     NULL,   PREC_NONE},
  [TOKEN_FALSE]         = {NULL,     NULL,   PREC_NONE},
  [TOKEN_FOR]           = {NULL,     NULL,   PREC_NONE},
  [TOKEN_FUN]           = {NULL,     NULL,   PREC_NONE},
  [TOKEN_IF]            = {NULL,     NULL,   PREC_NONE},
  [TOKEN_NIL]           = {NULL,     NULL,   PREC_NONE},
  [TOKEN_OR]            = {NULL,     NULL,   PREC_NONE},
  [TOKEN_PRINT]         = {NULL,     NULL,   PREC_NONE},
  [TOKEN_RETURN]        = {NULL,     NULL,   PREC_NONE},
  [TOKEN_SUPER]         = {NULL,     NULL,   PREC_NONE},
  [TOKEN_THIS]          = {NULL,     NULL,   PREC_NONE},
  [TOKEN_TRUE]          = {NULL,     NULL,   PREC_NONE},
  [TOKEN_VAR]           = {NULL,     NULL,   PREC_NONE},
  [TOKEN_WHILE]         = {NULL,     NULL,   PREC_NONE},
  [TOKEN_ERROR]         = {NULL,     NULL,   PREC_NONE},
  [TOKEN_EOF]           = {NULL,     NULL,   PREC_NONE},
};


Chunk *currentChunk() {
	return compillingChunk;
}

void errorAt(Token *token, char *message) {
	if(parser.panicMode) return;
	parser.panicMode = 1;
	printf("[error at line: %zu] ", token->line);
	if(token->type == TOKEN_EOF) {
		printf("at the end ");
	} else if(token->type != TOKEN_ERROR) {
		printf("in '%*.s': ", (int)token->length, token->start);
	}
	printf("%s\n", message);
	parser.hadError = 1;
}

void errorAtCurrent(char *message) {
	errorAt(&parser.current, message);
}

void error(char *message) {
	errorAt(&parser.prev, message);
}


static void advance() {
	parser.prev = parser.current;
	parser.current = scanToken();
	while(parser.current.type == TOKEN_ERROR) errorAtCurrent(parser.current.start);
}

static void consume(TokenType type, char *message) {
	if(parser.current.type != type) {
		errorAtCurrent(message);
		return;
	}
	advance();
}


void addByte(uint8_t b) {
	writeChunk(currentChunk(), b, parser.prev.line); 
}

void addBytes(uint8_t b1, uint8_t b2) {
	writeChunk(currentChunk(), b1, parser.prev.line); 
	writeChunk(currentChunk(), b2, parser.prev.line); 
}

void addReturn() {
	addByte(OP_RETURN);
}

void endCompiler() {
	addReturn();
}

static void number() {
	double value = strtod(parser.prev.start, NULL);
	if(writeConstant(currentChunk(), value, parser.prev.line)) {
		error("Too many constants in one chunk, constants number can only be in the range [0x00-0xFFFF]");
	}
}

ParseRule *getRule(TokenType type) {
	return &rules[type];
}

void grouping() {
	expression();
	consume(TOKEN_RIGHT_PAREN, "Expected ')' after expression.");
}

void unary() {
	TokenType operator = parser.prev.type;
	parsePrec(PREC_UNARY);
	switch(operator) {
		case TOKEN_MINUS: addByte(OP_NEGATE); break;
		default: break;
	}
}

void binary() {
	TokenType operator = parser.prev.type;
	ParseRule *rule = getRule(operator);
	parsePrec((Precedence)(rule->precedence + 1));
	switch(operator) {
		case TOKEN_PLUS: addByte(OP_ADD); break;
		case TOKEN_MINUS: addByte(OP_SUBTRACT); break;
		case TOKEN_STAR: addByte(OP_MULTIPLY); break;
		case TOKEN_SLASH: addByte(OP_DIVIDE); break;
		default: break;
	}
}

void parsePrec(Precedence prec) {
	advance();	
	ParseRule *prefixRule = getRule(parser.prev.type);
	if(prefixRule->prefix == NULL) {
		error("Invalid expression.");
		return;
	}
	prefixRule->prefix();
	ParseRule *currentRule = getRule(parser.current.type);
	while(currentRule->precedence >= prec) {
		advance();
		currentRule->infix();
    currentRule = getRule(parser.current.type);
	}
}

void expression() {
	parsePrec(PREC_ASSIGNMENT);
}

uint8_t compile(char *source, Chunk *chunk) {
	initLexer(source);
	parser.hadError = 0;
	parser.panicMode = 0;
	compillingChunk = chunk;
	advance();
	expression();
	consume(TOKEN_EOF, "Expect end of expression\n");
	endCompiler();
	return !(parser.hadError);
}
