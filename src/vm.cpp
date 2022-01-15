#include "vm.hpp"

#include<cassert>
#include<iostream>

#include "compiler.hpp"

using namespace std;

VM::InterpretResult VM::interpret(const char* source) {
  assert(source != NULL);
  Compiler compiler;
  Chunk chunk;
  if(!compiler.compile(source, chunk))
    return VM::INTERPRET_COMPILE_ERROR;
  ip = &chunk.code[0];
  return run(chunk);
}

#define READ_BYTE() (*ip++)
#define READ_CONSTANT() (chunk.constants[READ_BYTE()])
VM::InterpretResult VM::run(const Chunk &chunk) {
  while(true) {		// TODO Convert this to generated goto.
    uint8_t instruction;               
    switch(instruction = READ_BYTE()) {
      case Chunk::OP_CONSTANT:
	push(READ_CONSTANT());
	break;
      case Chunk::OP_NEGATE:
	if (!IS_NUMBER(peek(0))) {                
	  cerr << "Operand must be a number.";
	  return INTERPRET_RUNTIME_ERROR;          
	}
	push(NUMBER_VAL(-AS_NUMBER(pop())));       
	break;
      case Chunk::OP_RETURN:
	cout << to_string(pop()) << "\n";
	return INTERPRET_OK;           
      default:
	cout << "Unknown instruction: " << (uint)instruction << "\n";
        return INTERPRET_RUNTIME_ERROR;
    }                                   
  }
}
#undef READ_BYTE
#undef READ_CONSTANT

void VM::push(Value value) {
  *stackTop = value;
  stackTop++;
}

Value VM::pop() {
  stackTop--;
  return *stackTop;
}

Value VM::peek(int distance) {
  return stackTop[-1-distance];
}
