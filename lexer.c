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
				break;
		}
	}
}


Token scanToken() {
	lexer.start = lexer.current;
	if(isAtEnd()) return makeToken(TOKEN_EOF);
	char c = advance();
	if(isDigit(c)) return number();
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
