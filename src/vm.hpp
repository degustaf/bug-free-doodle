#ifndef SRC_VM_HPP
#define SRC_VM_HPP

#include <istream>

#include "chunk.hpp"
 
#define STACK_MAX 256	    // TODO Should this be larger? dynamic? stl stack?

class VM {
  public:
    VM() : stackTop(stack) {};
    enum InterpretResult {
      INTERPRET_OK,
      INTERPRET_COMPILE_ERROR,
      INTERPRET_RUNTIME_ERROR,
    };
    InterpretResult interpret(const char*);
  private:
    const uint8_t *ip;
    Value stack[STACK_MAX];
    Value* stackTop;

    InterpretResult run(const Chunk &chunk);
    void push(Value value);                 
    Value pop();
    Value peek(int distance);
};

#endif /* SRC_VM_HPP */
