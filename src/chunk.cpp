#include "chunk.hpp"

int Chunk::addConstant(Value val) {
  constants.push_back(val);
  return constants.size() - 1;
}

void Chunk::write(uint8_t byte, int line) {
  code.push_back(byte);
  lines.push_back(line);
}
