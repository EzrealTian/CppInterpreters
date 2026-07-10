#ifndef CLOX_CHUNK_H_
#define CLOX_CHUNK_H_

#include <iostream>
#include <vector>
#include <string>
#include <iomanip>

#include "lox_bytecode/common.h"
#include "lox_bytecode/value.h"

enum class OpCode : uint8_t {
  OP_CONSTANT,
  OP_RETURN,
};

class Chunk {
 public:
  void Write(uint8_t byte, int line) {
    code_.push_back(byte);
    lines_.push_back(line);
  }
  void Write(OpCode op, int line) {
    code_.push_back(static_cast<uint8_t>(op));
    lines_.push_back(line);
  }

  int AddConstant(Value value) {
    constants_.push_back(value);
    return constants_.size() - 1;
  }

  void Disassemble(std::string name) {
    std::cout << "== " << name << " ==" << std::endl;

    for (size_t offset = 0; offset < code_.size();) {
      offset = DisassembleInstruction(offset);
    }
  }

 private:
  size_t DisassembleInstruction(size_t offset) {
    std::cout << std::setfill('0') << std::setw(4) << offset << " ";

    if (offset > 0 && lines_.at(offset) == lines_.at(offset - 1)) {
      std::cout << "   | ";
    } else {
      std::cout << std::setfill(' ') << std::setw(4) << lines_.at(offset) << " ";
    }

    uint8_t instruction = code_.at(offset);
    switch (static_cast<OpCode>(instruction)) {
      case OpCode::OP_CONSTANT:
        return ConstantInstruction("OP_CONSTANT", offset);
      case OpCode::OP_RETURN:
        return SimpleInstruction("OP_RETURN", offset);
      default:
        std::cout << "Unknown opcode " << instruction << std::endl;
    }
    return offset + 1;
  }

  size_t SimpleInstruction(std::string name, size_t offset) {
    std::cout << name << std::endl;
    return offset + 1;
  }

  size_t ConstantInstruction(std::string name, size_t offset) {
    uint8_t index = code_.at(offset + 1);
    std::cout << std::left << std::setfill(' ') << std::setw(16) << name
              << std::right << std::setw(4) << static_cast<int>(index)
              << " '";
    PrintValue(constants_.at(index));
    std::cout << "'" << std::endl;
    return offset + 2;
  }

  void PrintValue(Value value) { std::cout << value; }

 private:
  std::vector<uint8_t> code_;
  std::vector<int> lines_;
  ValueArray constants_;
};
#endif  // CLOX_CHUNK_H_