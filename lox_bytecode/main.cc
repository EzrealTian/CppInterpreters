#include <iostream>
#include "lox_bytecode/common.h"
#include "lox_bytecode/chunk.h"
#include "lox_bytecode/vm.h"

int main(int argc, char const* argv[]) {
  VM vm;
  vm.InitVM();
  Chunk chunk;
  int constant = chunk.AddConstant(1.2);
  chunk.Write(OpCode::OP_CONSTANT, 123);
  chunk.Write(constant, 123);
  chunk.Write(OpCode::OP_RETURN, 123);
  chunk.Disassemble("test");
  vm.FreeVM();
  return 0;
}
