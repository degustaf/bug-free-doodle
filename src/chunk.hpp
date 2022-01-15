#ifndef SRC_CHUNK_HPP
#define SRC_CHUNK_HPP

#include <cstdint>
#include <vector>

#include "value.hpp"


struct Chunk {
  enum OpCode {
    OP_CONSTANT,
    OP_ADD,     
    OP_SUBTRACT,
    OP_MULTIPLY,
    OP_DIVIDE,
    OP_NEGATE,
    OP_RETURN,
  };
  std::vector<uint8_t> code;
  std::vector<Value> constants;
  std::vector<int> lines;
  // TODO convert tracking of line for op codes to more space efficient algorithm
  // e.g. run length encoding.

  int addConstant(Value);
  void write(uint8_t byte, int line);
};

#endif /* SRC_CHUNK_HPP */
