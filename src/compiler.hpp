#ifndef SRC_COMPILER_HPP
#define SRC_COMPILER_HPP

#include "chunk.hpp"
#include "scanner.hpp"

enum Precedence {
  PREC_NONE,
  PREC_ASSIGNMENT,  // =
  PREC_OR,          // or
  PREC_AND,         // and
  PREC_EQUALITY,    // == !=
  PREC_COMPARISON,  // < > <= >
  PREC_TERM,        // + -
  PREC_FACTOR,      // * /
  PREC_UNARY,       // ! - +
  PREC_CALL,        // . () []
  PREC_PRIMARY
};

class Compiler {
  public:
    Compiler() {};
    bool compile(const char *source, Chunk &chunk);

    void binary(void);
    void grouping(void);
    void number(void);
    void unary(void);
  private:
    Token current;
    Token previous;
    bool hadError;
    bool panicMode;
    Scanner scanner;
    Chunk *compilingChunk;

    void parsePrecedence(Precedence precedence);
    void expression(void);

    void advance(void);
    void consume(Token::TokenType type, const char *message);
    Chunk& currentChunk(void);
    void emitByte(uint8_t byte);
    void emitBytes(uint8_t byte1, uint8_t byte2);
    void emitConstant(Value value);
    void emitReturn(void);
    void endCompiler(void);
    uint8_t makeConstant(Value value);

    void errorAtCurrent(const char* message);
    void error(const char* message);
    void errorAt(const Token &token, const char *message);
};

#endif /* SRC_COMPILER_HPP */
