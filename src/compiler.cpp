#include "compiler.hpp"

#include <iostream>

#include <cassert>

using namespace std;


typedef void (Compiler::*ParseFn)(void);

struct ParseRule {
  ParseFn prefix;
  ParseFn infix;
  Precedence precedence;
};

ParseRule rules[] = {
  { &Compiler::grouping, NULL,		    PREC_CALL },       // TOKEN_LEFT_PAREN
  { NULL,		 NULL,    	    PREC_NONE },       // TOKEN_RIGHT_PAREN
  { NULL,     		 NULL,    	    PREC_NONE },       // TOKEN_LEFT_BRACE
  { NULL,     		 NULL,    	    PREC_NONE },       // TOKEN_RIGHT_BRACE
  { NULL,     		 NULL,    	    PREC_NONE },       // TOKEN_COMMA
  { NULL,     		 NULL,    	    PREC_CALL },       // TOKEN_DOT
  { &Compiler::unary,    &Compiler::binary, PREC_TERM },       // TOKEN_MINUS
  { NULL,		 &Compiler::binary, PREC_TERM },       // TOKEN_PLUS
  { NULL,     		 NULL,		    PREC_NONE },       // TOKEN_SEMICOLON
  { NULL,     		 &Compiler::binary, PREC_FACTOR },     // TOKEN_SLASH
  { NULL,     		 &Compiler::binary, PREC_FACTOR },     // TOKEN_STAR
  { NULL,     		 NULL,		    PREC_NONE },       // TOKEN_BANG
  { NULL,     		 NULL,    	    PREC_EQUALITY },   // TOKEN_BANG_EQUAL
  { NULL,     		 NULL,    	    PREC_NONE },       // TOKEN_EQUAL
  { NULL,     		 NULL,    	    PREC_EQUALITY },   // TOKEN_EQUAL_EQUAL
  { NULL,     		 NULL,    	    PREC_COMPARISON }, // TOKEN_GREATER
  { NULL,     		 NULL,    	    PREC_COMPARISON }, // TOKEN_GREATER_EQUAL
  { NULL,     		 NULL,    	    PREC_COMPARISON }, // TOKEN_LESS
  { NULL,     		 NULL,    	    PREC_COMPARISON }, // TOKEN_LESS_EQUAL
  { NULL,     		 NULL,    	    PREC_NONE },       // TOKEN_IDENTIFIER
  { NULL,     		 NULL,    	    PREC_NONE },       // TOKEN_STRING
  { &Compiler::number,   NULL,    	    PREC_NONE },       // TOKEN_NUMBER
  { NULL,		 NULL,    	    PREC_AND },        // TOKEN_AND
  { NULL,     		 NULL,    	    PREC_NONE },       // TOKEN_CLASS
  { NULL,     		 NULL,    	    PREC_NONE },       // TOKEN_ELSE
  { NULL,     		 NULL,    	    PREC_NONE },       // TOKEN_FALSE
  { NULL,     		 NULL,    	    PREC_NONE },       // TOKEN_FUN
  { NULL,     		 NULL,    	    PREC_NONE },       // TOKEN_FOR
  { NULL,     		 NULL,    	    PREC_NONE },       // TOKEN_IF
  { NULL,     		 NULL,    	    PREC_NONE },       // TOKEN_NIL
  { NULL,     		 NULL,    	    PREC_OR },         // TOKEN_OR
  { NULL,     		 NULL,    	    PREC_NONE },       // TOKEN_PRINT
  { NULL,     		 NULL,    	    PREC_NONE },       // TOKEN_RETURN
  { NULL,     		 NULL,    	    PREC_NONE },       // TOKEN_SUPER
  { NULL,     		 NULL,    	    PREC_NONE },       // TOKEN_THIS
  { NULL,     		 NULL,    	    PREC_NONE },       // TOKEN_TRUE
  { NULL,     		 NULL,    	    PREC_NONE },       // TOKEN_VAR
  { NULL,     		 NULL,    	    PREC_NONE },       // TOKEN_WHILE
  { NULL,     		 NULL,    	    PREC_NONE },       // TOKEN_ERROR
  { NULL,     		 NULL,    	    PREC_NONE },       // TOKEN_EOF
};

static ParseRule* getRule(Token::TokenType type) {
  return &rules[type];
}

bool Compiler::compile(const char *source, Chunk &chunk) {
  scanner = Scanner(source);
  compilingChunk = &chunk;
  hadError = panicMode = false;
  advance();
  // cout << "current token = " << current.start << "\n";
  expression();
  consume(Token::TOKEN_EOF, "Expect end of expression.");
  endCompiler();
  return !hadError;
}

void Compiler::parsePrecedence(Precedence precedence) {
  // cout << "In parsePrecedence with precedence " << precedence << "\n";
  advance();
  // cout << "previous token = " << previous.start << "\n";
  // cout << "current token = " << current.start << "\n";
  ParseFn prefixRule = getRule(previous.type)->prefix;
  if(prefixRule == NULL) {
    error("Expect expression.");
    return;
  }

  (this->*prefixRule)();

  // cout << "After prefixRule.\n";
  // cout << "Current Precedence is " << getRule(current.type)->precedence << "\n";

  while(precedence <= getRule(current.type)->precedence) {
    advance();
    ParseFn infixRule = getRule(previous.type)->infix;
    (this->*infixRule)();
  }
}

void Compiler::binary() {
  Token::TokenType operatorType = previous.type;
  ParseRule *rule = getRule(operatorType);
  parsePrecedence((Precedence)(rule->precedence + 1));

  switch(operatorType) {
    case Token::TOKEN_PLUS:	emitByte(Chunk::OpCode::OP_ADD);break;
    case Token::TOKEN_MINUS:	emitByte(Chunk::OpCode::OP_SUBTRACT); break;
    case Token::TOKEN_STAR:	emitByte(Chunk::OpCode::OP_MULTIPLY); break;
    case Token::TOKEN_SLASH:    emitByte(Chunk::OpCode::OP_DIVIDE); break;
    default:			assert(false);	// Unreachable
  }
}

void Compiler::expression() {
  // cout << "In expression.\n";
  parsePrecedence(PREC_ASSIGNMENT);
}

void Compiler::grouping() {
  expression();
  consume(Token::TOKEN_RIGHT_PAREN, "Expect ')' after expression");
}

void Compiler::number() {
  // cout << "In Number\n";
  double value = strtod(previous.start, NULL);
  emitConstant(value);
}

void Compiler::unary() {
  Token::TokenType operatorType = previous.type;
  parsePrecedence(PREC_UNARY);

  switch(operatorType) {
	case Token::TOKEN_MINUS:
	  emitByte(Chunk::OpCode::OP_NEGATE);break;
	default:
	  assert(false);	// unreachable
  }
}

void Compiler::advance() {
  previous = current;

  while(true) {
    current = scanner.scanToken();
    if(current.type != Token::TOKEN_ERROR)
      break;
    errorAtCurrent(current.start);
  }
}

void Compiler::consume(Token::TokenType type, const char *message) {
  assert(message != NULL);
  if(current.type == type) {
    advance();
    return;
  }
  errorAtCurrent(message);
}

Chunk& Compiler::currentChunk() {
  assert(compilingChunk != NULL);
  return *compilingChunk;
}

void Compiler::emitByte(uint8_t byte) {
  currentChunk().write(byte, previous.line);
}

void Compiler::emitBytes(uint8_t byte1, uint8_t byte2) {
  emitByte(byte1);
  emitByte(byte2);
}

void Compiler::emitConstant(Value value) {
  emitBytes(Chunk::OpCode::OP_CONSTANT, makeConstant(value));
}

void Compiler::emitReturn() {
  emitByte(Chunk::OpCode::OP_RETURN);
}

void Compiler::endCompiler() {
  emitReturn();
}

uint8_t Compiler::makeConstant(Value value) {
  int constant = currentChunk().addConstant(value);
  if(constant > UINT8_MAX) {
	error("Too many constants in one chunk.");
	return 0;
  }

  return (uint8_t)constant;
}

void Compiler::errorAt(const Token &token, const char *message) {
  assert(message != NULL);
  if(panicMode)
    return;
  panicMode = true;
  cerr << "[line " << token.line << "] Error";
  if(token.type == Token::TOKEN_EOF) {
    cerr << " at end";
  } else if(token.type != Token::TOKEN_ERROR) {
    cerr << " at '";
    for(int i=0; i<token.length; i++)
      cerr << token.start[i];
    cerr << "'";
  }
  cerr << ": " << message << "\n";
  hadError = true;
}

void Compiler::error(const char* message) {
  assert(message != NULL);
  errorAt(previous, message);
}

void Compiler::errorAtCurrent(const char* message) {
  assert(message != NULL);
  errorAt(current, message);
}
