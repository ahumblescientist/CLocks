#include "compiler.h"
#include "obj.h"


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

uint16_t identifierConstant(Token), parseVariable(char *);

static void advance(), consume(TokenType, char *), number(), boolean(), string(), variable();
static uint8_t check(TokenType), match(TokenType);
void expression(), parsePrec(Precedence), grouping(), unary(), binary(), statement(), declaration(), printStatement();

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
  [TOKEN_BANG]          = {unary,     NULL,   PREC_NONE},
  [TOKEN_BANG_EQUAL]    = {NULL,     binary,   PREC_EQUALITY},
  [TOKEN_EQUAL]         = {NULL,     NULL,   PREC_NONE},
  [TOKEN_EQUAL_EQUAL]   = {NULL,     binary,   PREC_EQUALITY},
  [TOKEN_GREATER]       = {NULL,     binary,   PREC_COMPARISON},
  [TOKEN_GREATER_EQUAL] = {NULL,     binary,   PREC_COMPARISON},
  [TOKEN_LESS]          = {NULL,     binary,   PREC_COMPARISON},
  [TOKEN_LESS_EQUAL]    = {NULL,     binary,   PREC_COMPARISON},
  [TOKEN_IDENTIFIER]    = {variable,     NULL,   PREC_NONE},
  [TOKEN_STRING]        = {string,     NULL,   PREC_NONE},
  [TOKEN_NUMBER]        = {number,   NULL,   PREC_NONE},
  [TOKEN_AND]           = {NULL,     NULL,   PREC_NONE},
  [TOKEN_CLASS]         = {NULL,     NULL,   PREC_NONE},
  [TOKEN_ELSE]          = {NULL,     NULL,   PREC_NONE},
  [TOKEN_FALSE]         = {boolean,     NULL,   PREC_NONE},
  [TOKEN_FOR]           = {NULL,     NULL,   PREC_NONE},
  [TOKEN_FUN]           = {NULL,     NULL,   PREC_NONE},
  [TOKEN_IF]            = {NULL,     NULL,   PREC_NONE},
  [TOKEN_NIL]           = {boolean,     NULL,   PREC_NONE},
  [TOKEN_OR]            = {NULL,     NULL,   PREC_NONE},
  [TOKEN_PRINT]         = {NULL,     NULL,   PREC_NONE},
  [TOKEN_RETURN]        = {NULL,     NULL,   PREC_NONE},
  [TOKEN_SUPER]         = {NULL,     NULL,   PREC_NONE},
  [TOKEN_TRUE]          = {boolean,     NULL,   PREC_NONE},
  [TOKEN_VAR]           = {NULL,     NULL,   PREC_NONE},
  [TOKEN_WHILE]         = {NULL,     NULL,   PREC_NONE},
  [TOKEN_ERROR]         = {NULL,     NULL,   PREC_NONE},
  [TOKEN_THIS]         =  {NULL,     NULL,   PREC_NONE},
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

void sync() {
  parser.panicMode = 0;
  while(parser.current.type != TOKEN_EOF) {
    if(parser.prev.type == TOKEN_SEMICOLON) return;
    switch(parser.current.type) {
      case TOKEN_FOR:
      case TOKEN_IF:
      case TOKEN_VAR:
      case TOKEN_WHILE:
      case TOKEN_PRINT:
      case TOKEN_RETURN:
      case TOKEN_CLASS:
      case TOKEN_FUN:
        return;
      default:
        // nothing
    }
    advance();
  }
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

static uint8_t match(TokenType type) {
  if(!check(type)) return 0;
  advance();
  return 1;
}

static uint8_t check(TokenType type) {
  return (parser.current.type == type);
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

uint16_t makeConstant(Value v) {
	uint32_t index = addConstant(currentChunk(), v);
	if(index > 255) {
		writeChunk(currentChunk(), OP_CONSTANT_LONG, parser.prev.line);
		writeChunk(currentChunk(), index & 0xFF, parser.prev.line);
		writeChunk(currentChunk(), index >> 8, parser.prev.line);
	} else if(index < UINT16_MAX) {
		writeChunk(currentChunk(), OP_CONSTANT, parser.prev.line);
		writeChunk(currentChunk(), index, parser.prev.line);
	} else {
		error("Too many constants in one chunk, max is [0xFFFF] constants");
    return 0;
	}
  return index;
}

void endCompiler() {
	addReturn();
}

static void boolean() {
  if(parser.prev.type == TOKEN_TRUE) {
    addByte(OP_TRUE);
  } else if(parser.prev.type == TOKEN_FALSE) {
    addByte(OP_FALSE);
  } else {
    addByte(OP_NIL);
  }
}


static void number() {
	double value = strtod(parser.prev.start, NULL);
  makeConstant(makeNumber(value));
}

static void string() {
  makeConstant(makeObj((Obj *)copyString(parser.prev.start + 1llu, parser.prev.length - 2)));
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
		case TOKEN_BANG: addByte(OP_NOT); break;
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
    case TOKEN_EQUAL_EQUAL: addByte(OP_EQUAL); break;
    case TOKEN_BANG_EQUAL: addBytes(OP_EQUAL, OP_NOT); break;
    case TOKEN_LESS: addByte(OP_LESS); break;
    case TOKEN_LESS_EQUAL: addBytes(OP_GREATER, OP_NOT); break;
    case TOKEN_GREATER: addByte(OP_GREATER); break;
    case TOKEN_GREATER_EQUAL: addBytes(OP_GREATER, OP_NOT); break;
		default: break;
	}
}

void namedVariable(Token t) {
  uint16_t num = identifierConstant(t);
  addBytes(OP_GET_GLOBAL, num); 
}

static void variable() {
  namedVariable(parser.prev);
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

void printStatement() {
  expression();
  consume(TOKEN_SEMICOLON, "expected ';' after value.");
  addByte(OP_PRINT);
}

void expressionStatement() {
  expression();
  consume(TOKEN_SEMICOLON, "expected ';' after expression.");
  addByte(OP_POP);
}

uint16_t identifierConstant(Token t) {
  return makeConstant(makeObj(copyString(t.start, t.length)));
}

uint16_t parseVariable(char *errorMessage) {
  consume(TOKEN_IDENTIFIER, errorMessage);
  return identifierConstant(parser.prev);
}

void defineVariable(uint16_t global) {
  addBytes(OP_DEFINE_GLOBAL, global);
}

void varDeclare() {
  uint16_t global = parseVariable("Expected variable name.");
  if(match(TOKEN_EQUAL)) {
    expression();
  } else {
    addByte(OP_NIL);
  }
  consume(TOKEN_SEMICOLON, "expected ';' after variable declaration.");
  defineVariable(global);
}

void expression() {
	parsePrec(PREC_ASSIGNMENT);
}


void statement() {
  if(match(TOKEN_PRINT)) {
    printStatement();
  } else {
    expressionStatement();
  }
}

void declaration() {
  if(match(TOKEN_VAR)) {
    varDeclare();
  } else {
    statement();
  }
}

uint8_t compile(char *source, Chunk *chunk) {
	initLexer(source);
	parser.hadError = 0;
	parser.panicMode = 0;
	compillingChunk = chunk;
	advance();
  while(!match(TOKEN_EOF)) {
    declaration();
  }
	endCompiler();
	return !(parser.hadError);
}
