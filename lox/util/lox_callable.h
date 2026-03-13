#ifndef LOX_UTIL_LOX_CALLABLE_H_
#define LOX_UTIL_LOX_CALLABLE_H_

#include <vector>
#include <chrono>

#include "lox/util/lox_object.h"
#include "lox/ast/visitors/interpreter.h"
#include "lox/util/return_exception.h"
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
  FunctionCallable(FunctionStmt&& function_stmt,
                   std::shared_ptr<Environment> closure, bool is_initializer)
      : function_stmt_(
            std::make_shared<FunctionStmt>(std::move(function_stmt))),
        closure_(std::move(closure)),
        is_initializer_(is_initializer) {}

  // 使用已有的函数声明指针（用于 Bind 创建的新函数对象）
  FunctionCallable(std::shared_ptr<FunctionStmt> function_stmt,
                   std::shared_ptr<Environment> closure, bool is_initializer)
      : function_stmt_(std::move(function_stmt)),
        closure_(std::move(closure)),
        is_initializer_(is_initializer) {}

  ~FunctionCallable() = default;
  LoxObject operator()(Interpreter& interpreter,
                       std::vector<LoxObject> arguments) override {
    auto environment = std::make_shared<Environment>(closure_);
    for (size_t i = 0; i < function_stmt_->parameters_.size(); i++) {
      environment->Define(function_stmt_->parameters_[i].lexeme(),
                          arguments[i]);
    }
    try {
      interpreter.ExecuteBlock(function_stmt_->body_, environment);
    } catch (const ReturnException& return_exception) {
      if (is_initializer_) {
        return closure_->GetAt("this", 0);
      }
      return return_exception.value_;
    }
    return nullptr;
  }

  size_t arity() override { return function_stmt_->parameters_.size(); }

  std::string ToString() override {
    return "<fn " + function_stmt_->name_.lexeme() + "()>";
  }

  std::shared_ptr<FunctionCallable> Bind(LoxObject instance) {
    std::shared_ptr<Environment> environment =
        std::make_shared<Environment>(closure_);
    environment->Define("this", instance);
    return std::make_shared<FunctionCallable>(function_stmt_, environment,
                                              is_initializer_);
  }

 private:
  std::shared_ptr<FunctionStmt> function_stmt_;
  std::shared_ptr<Environment> closure_;
  bool is_initializer_ = false;
};

}  // namespace lox

#endif  // LOX_UTIL_LOX_CALLABLE_H_