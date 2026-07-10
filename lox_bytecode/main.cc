#include <iostream>
#include "lox_bytecode/common.h"
#include "lox_bytecode/chunk.h"

int main(int argc, char const* argv[]) {
  Chunk chunk;
  int constant = chunk.AddConstant(1.2);
  chunk.Write(OpCode::OP_CONSTANT, 123);
  chunk.Write(constant, 123);
  chunk.Write(OpCode::OP_RETURN, 123);
  chunk.Disassemble("test");
  return 0;
}
