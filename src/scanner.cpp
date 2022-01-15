#include "scanner.hpp"

#include <cassert>
#include <cstring>

struct Keyword {
  const char* identifier;
  size_t      length;
  Token::TokenType   tokenType;
};

static const Keyword keywords[] = {
  {"and",       3, Token::TOKEN_AND},
  {"class",     5, Token::TOKEN_CLASS},
  {"else",      4, Token::TOKEN_ELSE},
  {"false",     5, Token::TOKEN_FALSE},
  {"for",       3, Token::TOKEN_FOR},
  {"fun",	3, Token::TOKEN_FUN},
  {"if",        2, Token::TOKEN_IF},
  {"nil",	3, Token::TOKEN_NIL},
  {"or",	2, Token::TOKEN_OR},
  {"print",	5, Token::TOKEN_PRINT},	    // TODO change print to a function.
  {"return",    6, Token::TOKEN_RETURN},
  {"super",     5, Token::TOKEN_SUPER},
  {"this",      4, Token::TOKEN_THIS},
  {"true",      4, Token::TOKEN_TRUE},
  {"var",       3, Token::TOKEN_VAR},
  {"while",     5, Token::TOKEN_WHILE},
  {nullptr,     0, Token::TOKEN_EOF} // Sentinel to mark the end of the array.
};

static inline bool isAlpha(char c) {
  return (c >= 'a' && c <= 'z') ||
	 (c >= 'A' && c <= 'Z') ||
	 (c == '_');
}

static inline bool isDigit(char c) {
  return c >= '0' && c <= '9';
}

Token Scanner::scanToken(void) {
  skipWhitespace();
  start = current;

  if(isAtEnd())
    return makeToken(Token::TOKEN_EOF);

  char c = advance();

  if(isAlpha(c))
    return identifier();

  if(isDigit(c))
    return number();

  switch (c) {
    case '(': return makeToken(Token::TOKEN_LEFT_PAREN);
    case ')': return makeToken(Token::TOKEN_RIGHT_PAREN);
    case '{': return makeToken(Token::TOKEN_LEFT_BRACE);
    case '}': return makeToken(Token::TOKEN_RIGHT_BRACE);
    case ';': return makeToken(Token::TOKEN_SEMICOLON); 
    case ',': return makeToken(Token::TOKEN_COMMA);     
    case '.': return makeToken(Token::TOKEN_DOT);       
    case '-': return makeToken(Token::TOKEN_MINUS);     
    case '+': return makeToken(Token::TOKEN_PLUS);      
    case '/': return makeToken(Token::TOKEN_SLASH);     
    case '*': return makeToken(Token::TOKEN_STAR);      
    case '!': return makeToken(match('=') ? Token::TOKEN_BANG_EQUAL
      				    : Token::TOKEN_BANG); 
    case '=': return makeToken(match('=') ? Token::TOKEN_EQUAL_EQUAL
      				    : Token::TOKEN_EQUAL);
    case '<': return makeToken(match('=') ? Token::TOKEN_LESS_EQUAL
      				    : Token::TOKEN_LESS); 
    case '>': return makeToken(match('=') ? Token::TOKEN_GREATER_EQUAL
      				    : Token::TOKEN_GREATER);
    case '"': return string();
  }

  return errorToken("Unexpected character.");
}

inline Token Scanner::errorToken(const char *message) const {
  assert(message != NULL);
  return Token(Token::TOKEN_ERROR, message, strlen(message), line);
}

inline Token Scanner::makeToken(Token::TokenType type) const {
  return Token(type, start, current - start, line);
}

Token Scanner::identifier(void) {
  while (isAlpha(peek()) || isDigit(peek())) advance();

  // TODO possible optimization: convert to trie implemented by switch statements.
  // This COULD improve performance, but at the cost of readability.
  size_t length = current - start;
  for(const Keyword *kwd = keywords; kwd->identifier; kwd++) {
    if(length == kwd->length && strncmp(start, kwd->identifier, length) == 0)
      return makeToken(kwd->tokenType);
  }

  return makeToken(identifierType());
}

Token Scanner::number(void) {
  while(isDigit(peek()))
    advance();

  // Look for a fractional part.             
  if(peek() == '.' && isDigit(peekNext())) {
    // Consume the "."                       
    advance();                               
    while(isDigit(peek()))
      advance();       
  }                                          

  return makeToken(Token::TOKEN_NUMBER);
}

Token Scanner::string(void) {
  while(peek() != '"' && !isAtEnd()) {
    if(peek() == '\n')
      line++;
    advance();
  }

  if(isAtEnd())
    return errorToken("Unterminated string.");

  advance();
  return makeToken(Token::TOKEN_STRING);
}

inline char Scanner::advance(void) {
  assert(current != NULL);
  return *current++;
}

inline char Scanner::peek(void) const {
  assert(current != NULL);
  return *current;
}

inline char Scanner::peekNext(void) const {
  if(isAtEnd())
    return '\0';
  assert(current != NULL);
  return current[1];
}

inline bool Scanner::isAtEnd(void) const {
  assert(current != NULL);
  return *current == '\0';
}

Token::TokenType Scanner::identifierType(void) const {
  return Token::TOKEN_IDENTIFIER;
}

bool Scanner::match(char expected) {
  if(isAtEnd())
    return false;                   
  assert(current != NULL);
  if(*current != expected)
    return false;

  current++;                            
  return true;
}

void Scanner::skipWhitespace(void) {
  while(true) {
    char c = peek();
    switch(c) {
      case '\n':
	line++;
      case ' ':
      case '\r':
      case '\t':
	advance();
	break;
      case '/':                                         
	if (peekNext() == '/') {                        
	  // A comment goes until the end of the line.   
	  while (peek() != '\n' && !isAtEnd()) advance();
	} else {                                         
	  return;                                        
	}                                                
	break;
      default:
	return;
    }
  }
}
