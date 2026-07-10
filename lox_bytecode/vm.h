#ifndef CLOX_VM_H_
#define CLOX_VM_H_

#include "lox_bytecode/common.h"
#include "lox_bytecode/chunk.h"

#include <vector>
#include <iostream>
#include <stack>

#define DEBUG_TRACE_EXECUTION

enum class InterpretResult {
  INTERPRET_OK,
  INTERPRET_COMPILE_ERROR,
  INTERPRET_RUNTIME_ERROR
};

class VM {
 public:
  void InitVM();

  void FreeVM();

  InterpretResult Interpret() { return Run(); }

 private:
  InterpretResult Run() {
    auto read_byte = [&]() { return chunk_.GetInstruction(ip_++); };
    auto read_constant = [&]() { return chunk_.GetConstant(read_byte()); };
    auto binary_op = [this](auto op) {
      Value b = stack_.top();
      stack_.pop();
      Value a = stack_.top();
      stack_.pop();
      stack_.push(op(a, b));
    };
    for (;;) {
#ifdef DEBUG_TRACE_EXECUTION
      std::cout << "          ";
      std::stack<Value> temp_stack = stack_;
      std::vector<Value> values;
      while (!temp_stack.empty()) {
        values.push_back(temp_stack.top());
        temp_stack.pop();
      }
      for (int i = values.size() - 1; i >= 0; i--) {
        std::cout << "[ " << values.at(i) << " ]" << std::endl;
      }
      chunk_.DisassembleInstruction(ip_);
#endif
      uint8_t instruction = read_byte();
      switch (static_cast<OpCode>(instruction)) {
        case OpCode::OP_CONSTANT: {
          Value constant = read_constant();
          stack_.push(constant);
          break;
        }
        case OpCode::OP_ADD:
          binary_op(std::plus<Value>{});
          break;
        case OpCode::OP_SUBTRACT:
          binary_op(std::minus<Value>{});
          break;
        case OpCode::OP_MULTIPLY:
          binary_op(std::multiplies<Value>{});
          break;
        case OpCode::OP_DIVIDE:
          binary_op(std::divides<Value>{});
          break;
        case OpCode::OP_NEGATE:
          stack_.top() = -stack_.top();
          break;
        case OpCode::OP_RETURN:
          std::cout << stack_.top() << std::endl;
          stack_.pop();
          return InterpretResult::INTERPRET_OK;
        default:
          break;
      }
    }
    return InterpretResult::INTERPRET_OK;
  }

 private:
  Chunk chunk_;
  int ip_;
  std::stack<Value> stack_;
};

#endif  // CLOX_VM_H_