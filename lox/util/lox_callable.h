#ifndef LOX_UTIL_LOX_CALLABLE_H_
#define LOX_UTIL_LOX_CALLABLE_H_

#include <vector>
#include <chrono>

#include "lox/util/lox_object.h"
#include "lox/ast/visitors/interpreter.h"
// #include <memory>

namespace lox {

// 前向声明
// class LoxObject;
// class Interpreter;

class LoxCallable {
 public:
  // virtual LoxCallable() = default;
  virtual ~LoxCallable() = default;
  virtual LoxObject operator()(Interpreter& interpreter,
                               std::vector<LoxObject> arguments) = 0;
  virtual size_t arity() = 0;
  virtual std::string ToString() = 0;
};

class ClockCallable : public LoxCallable {
 public:
  ClockCallable() = default;
  ~ClockCallable() = default;
  LoxObject operator()(Interpreter& interpreter,
                       std::vector<LoxObject> arguments) override {
    (void)interpreter;
    (void)arguments;
    return LoxObject(
        std::chrono::system_clock::now().time_since_epoch().count() /
        1000000.0);
  }
  size_t arity() override { return 0; }
  std::string ToString() override { return "<fn clock()>"; }
};

class FunctionCallable : public LoxCallable {
 public:
  FunctionCallable(FunctionStmt&& function_stmt)
      : function_stmt_(std::move(function_stmt)) {}
  ~FunctionCallable() = default;
  LoxObject operator()(Interpreter& interpreter,
                       std::vector<LoxObject> arguments) override {
    Environment environment(interpreter.global_env_);
    for (size_t i = 0; i < function_stmt_.parameters_.size(); i++) {
      environment.Define(function_stmt_.parameters_[i].lexeme(), arguments[i]);
    }
    interpreter.ExecuteBlock(function_stmt_.body_, environment);
    return nullptr;
  }
  size_t arity() override { return function_stmt_.parameters_.size(); }
  std::string ToString() override {
    return "<fn " + function_stmt_.name_.lexeme() + "()>";
  }

 private:
  FunctionStmt function_stmt_;
};

}  // namespace lox

#endif  // LOX_UTIL_LOX_CALLABLE_H_