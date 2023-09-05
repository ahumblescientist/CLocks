#include "lexer.h"
#include <string.h>

Lexer lexer;


void initLexer(char *source) {
	lexer.start = source;
	lexer.current = source;
	lexer.line = 1;
}

Token makeToken(TokenType type) {
	Token ret;
	ret.length = lexer.current - lexer.start;
	ret.start = lexer.start;
	ret.type = type;
	ret.line = lexer.line;
	return ret;
}

Token errorToken(char *message) {
	Token ret;
	ret.length = strlen(message);
	ret.start = message;
	ret.type = TOKEN_ERROR;
	ret.line = lexer.line;
	return ret;
}

uint8_t isAtEnd() {
	return (*lexer.current == '\0');
}

char advance() {
	return *(lexer.current++);
}

uint8_t match(char c) {
	if(isAtEnd()) return 0;
	if(*lexer.current != c) return 0;
	lexer.current++;
	return 1;
}

char peek() {
	return *lexer.current;
}

char peekNext() {
	if(isAtEnd()) return '\0';
	return *(lexer.current + 1);
}

uint8_t isDigit(char c) {
	return (c >= '0' && c <= '9');
}

uint8_t isAlpha(char c) {
	return ((c <= 'Z' && c >= 'A') || (c <= 'z' && c >= 'a') || c == '_');
}

Token string() {
	while(peek() != '"' && !isAtEnd()) {
		if(peek() == '\n') lexer.line++;
		advance();
	}
	if(isAtEnd()) {
		return errorToken("Unterminated string.");
	}
	advance();
	return makeToken(TOKEN_STRING);
}

Token number() {
	while(isDigit(peek()) && !isAtEnd()) advance();
	if(peek() == '.') 									 advance();
	while(isDigit(peek()) && !isAtEnd()) advance();
	return makeToken(TOKEN_NUMBER);
}

TokenType compare(TokenType ret, char *val, size_t rest, size_t start) {
	if((start + rest == (lexer.current - lexer.start)) && !memcmp(val, lexer.start + start, rest)) {
		return ret;
	}
	return TOKEN_IDENTIFIER;
}

TokenType identifierType() {
	switch(*lexer.start) {
		case 'w':	return compare(TOKEN_WHILE, "hile", 4, 1); 
		case 'i':	return compare(TOKEN_IF, "f", 1, 1); 
		case 'e':	return compare(TOKEN_ELSE, "lse", 3, 1); 
		case 'T':	return compare(TOKEN_TRUE, "rue", 3, 1); 
		case 'F':	return compare(TOKEN_FALSE, "alse", 4, 1); 
		case 'N':	return compare(TOKEN_NIL, "il", 2, 1); 
		case 'o':	return compare(TOKEN_OR, "r", 1, 1); 
		case 'a':	return compare(TOKEN_AND, "nd", 2, 1); 
		case 'r':	return compare(TOKEN_RETURN, "eturn", 5, 1); 
		case 'v':	return compare(TOKEN_VAR, "ar", 2, 1); 
		case 's':	return compare(TOKEN_SUPER, "uper", 4, 1); 
		case 'p':	return compare(TOKEN_PRINT, "rint", 4, 1); 
		case 'c':	return compare(TOKEN_CLASS, "lass", 4, 1); 
		case 'f': {
			switch(*(lexer.start+1)) {
				case 'u': return compare(TOKEN_FUN, "nction", 6, 2);
				case 'o': return compare(TOKEN_FUN, "r", 1, 2);
				default: break;
			}
		}
		default: break;
	}
	return TOKEN_IDENTIFIER;
}

Token identifier() {
	while( isAlpha(peek()) || isDigit(peek()) ) {
		advance();
	}
	return makeToken(identifierType());
}

void skipSpaces() {
	while(1) {
		char c = peek();
		switch(c) {
			case ' ':
			case '\t':
			case '\r':
				advance();
				break;
			case '\n':
				lexer.line++;
				advance();
				break;
			case '/': {
				if(peekNext() == '/') {
					while(peek() != '\n' && !isAtEnd()) advance();
				} else {
					return;
				}
				break;
			}
			default:
				return;
		}
	}
}


Token scanToken() {
	skipSpaces();
	lexer.start = lexer.current;
	if(isAtEnd()) return makeToken(TOKEN_EOF);
	char c = advance();
	if(isDigit(c)) return number();
	if(isAlpha(c)) return identifier();
	switch(c) {
		case '(': return makeToken(TOKEN_LEFT_PAREN); break;
		case ')': return makeToken(TOKEN_RIGHT_PAREN); break;
		case '{': return makeToken(TOKEN_LEFT_BRACE); break;
		case '}': return makeToken(TOKEN_RIGHT_BRACE); break;
		case ';': return makeToken(TOKEN_SEMICOLON); break;
		case ',': return makeToken(TOKEN_COMMA); break;
		case '.': return makeToken(TOKEN_DOT); break;
		case '-': return makeToken(TOKEN_MINUS); break;
		case '+': return makeToken(TOKEN_PLUS); break;
		case '*': return makeToken(TOKEN_STAR); break;
		case '/': return makeToken(TOKEN_SLASH); break;
		case '!': {
			if(match('=')) return makeToken(TOKEN_BANG_EQUAL);
			return makeToken(TOKEN_BANG);
			break;
		}
		case '=': {
			if(match('=')) return makeToken(TOKEN_EQUAL_EQUAL);
			return makeToken(TOKEN_EQUAL);
			break;
		}
		case '>': {
			if(match('=')) return makeToken(TOKEN_GREATER_EQUAL);
			return makeToken(TOKEN_GREATER);
		}
		case '<': {
			if(match('=')) return makeToken(TOKEN_LESS_EQUAL);
			return makeToken(TOKEN_LESS);
		}
		case '"': return string(); break;
	}
	return makeToken(TOKEN_EOF);
}
