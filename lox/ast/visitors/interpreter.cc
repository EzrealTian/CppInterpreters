#include "lox/ast/visitors/interpreter.h"

#include "lox/core/environment.h"
#include "lox/core/lox.h"
#include "lox/ast/visitor.h"
#include "lox/ast/expr.h"
#include "lox/ast/stmt.h"
#include "lox/util/lox_object.h"
#include "lox/util/runtime_error.h"
#include "lox/util/break_exception.h"
#include "lox/util/lox_callable.h"
#include "lox/util/return_exception.h"

#include <vector>

namespace lox {
Interpreter::Interpreter()
    : global_env_(std::make_shared<Environment>()), environment_(global_env_) {
  global_env_->Define("clock", LoxObject(std::make_shared<ClockCallable>()));
}

void Interpreter::Interpret(std::vector<StmtPtr> statements) {
  try {
    for (auto& statement : statements) {
      Execute(statement);
    }
  } catch (const RuntimeError& error) {
    Lox::Instance().RuntimeError(error);
  }
}

LoxObject Interpreter::Visit(LiteralExpr& expr) { return expr.value_; }

LoxObject Interpreter::Visit(GroupingExpr& expr) {
  return Evaluate(expr.expression_);
}

LoxObject Interpreter::Visit(UnaryExpr& expr) {
  LoxObject right = Evaluate(expr.right_);
  switch (expr.op_.type()) {
    case TokenType::MINUS:
      return -right.get<double>();
    case TokenType::BANG:
      return !right.isTruthy();
    default:
      return nullptr;
  }
}

LoxObject Interpreter::Visit(BinaryExpr& expr) {
  LoxObject left = Evaluate(expr.left_);
  LoxObject right = Evaluate(expr.right_);
  switch (expr.op_.type()) {
    case TokenType::PLUS:
      if (left.is<double>() && right.is<double>()) {
        return left.get<double>() + right.get<double>();
      }
      if (left.is<std::string>() && right.is<std::string>()) {
        return left.get<std::string>() + right.get<std::string>();
      }
      throw RuntimeError(expr.op_, "Operands must be numbers or strings.");
    case TokenType::MINUS:
      CheckNumberOperands(expr.op_, left, right);
      return left.get<double>() - right.get<double>();
    case TokenType::STAR:
      CheckNumberOperands(expr.op_, left, right);
      return left.get<double>() * right.get<double>();
    case TokenType::SLASH:
      CheckNumberOperands(expr.op_, left, right);
      if (right.get<double>() == 0) {
        throw RuntimeError(expr.op_, "Division by zero.");
      }
      return left.get<double>() / right.get<double>();
    case TokenType::GREATER:
      CheckNumberOperands(expr.op_, left, right);
      return left.get<double>() > right.get<double>();
    case TokenType::GREATER_EQUAL:
      CheckNumberOperands(expr.op_, left, right);
      return left.get<double>() >= right.get<double>();
    case TokenType::LESS:
      CheckNumberOperands(expr.op_, left, right);
      return left.get<double>() < right.get<double>();
    case TokenType::LESS_EQUAL:
      CheckNumberOperands(expr.op_, left, right);
      return left.get<double>() <= right.get<double>();
    case TokenType::EQUAL_EQUAL:
      return left == right;
    case TokenType::BANG_EQUAL:
      return left != right;
    default:
      return nullptr;
  }
}

LoxObject Interpreter::Visit(VariableExpr& variable) {
  return environment_->Get(variable.name_);
}

LoxObject Interpreter::Visit(AssignExpr& assign) {
  LoxObject value = Evaluate(assign.value_);
  environment_->Assign(assign.name_, value);
  return value;
}

LoxObject Interpreter::Visit(LogicalExpr& logical) {
  LoxObject left = Evaluate(logical.left_);
  if (logical.op_.type() == TokenType::AND) {
    if (!left.isTruthy()) {
      return left;
    }
  } else if (logical.op_.type() == TokenType::OR) {
    if (left.isTruthy()) {
      return left;
    }
  }
  return Evaluate(logical.right_);
}

void Interpreter::Visit(BlockStmt& block_stmt) {
  ExecuteBlock(block_stmt.statements_,
               std::make_shared<Environment>(environment_));
}

void Interpreter::Visit(ExprStmt& expr_stmt) { Evaluate(expr_stmt.expr_); }

void Interpreter::Visit(PrintStmt& print_stmt) {
  std::cout << Evaluate(print_stmt.expr_).ToString() << std::endl;
}

void Interpreter::Visit(VarStmt& var_stmt) {
  LoxObject value = nullptr;
  if (var_stmt.initializer_ != nullptr) {
    value = Evaluate(var_stmt.initializer_);
  }
  environment_->Define(var_stmt.name_.lexeme(), value);
}

void Interpreter::Visit(IfStmt& if_stmt) {
  if (Evaluate(if_stmt.condition_).isTruthy()) {
    Execute(if_stmt.then_branch_);
  } else if (if_stmt.else_branch_ != nullptr) {
    Execute(if_stmt.else_branch_);
  }
}

void Interpreter::Visit(WhileStmt& while_stmt) {
  while (Evaluate(while_stmt.condition_).isTruthy()) {
    try {
      Execute(while_stmt.body_);
    } catch (const BreakException&) {
      break;
    }
  }
}

void Interpreter::Visit(BreakStmt& break_stmt) {
  (void)break_stmt;  // 未使用参数
  throw BreakException();
}

void Interpreter::Visit(FunctionStmt& function_stmt) {
  std::string function_name = function_stmt.name_.lexeme();
  LoxObject function = LoxObject(std::make_shared<FunctionCallable>(
      std::move(function_stmt), environment_));
  environment_->Define(function_name, function);
}

void Interpreter::Visit(ReturnStmt& return_stmt) {
  LoxObject value = nullptr;
  if (return_stmt.value_ != nullptr) {
    value = Evaluate(return_stmt.value_);
  }
  throw ReturnException(value);
}

LoxObject Interpreter::Visit(CallExpr& call) {
  LoxObject callee = Evaluate(call.callee_);
  std::vector<LoxObject> arguments;
  for (auto& argument : call.arguments_) {
    arguments.push_back(Evaluate(argument));
  }
  if (!callee.is<LoxCallable>()) {
    throw RuntimeError(call.paren_, "Can only call functions and classes.");
  }

  LoxCallable& callable = *callee.get<std::shared_ptr<LoxCallable>>();
  if (callable.arity() != arguments.size()) {
    throw RuntimeError(call.paren_, "Expected " +
                                        std::to_string(callable.arity()) +
                                        " arguments but got " +
                                        std::to_string(arguments.size()));
  }
  return callable(*this, std::move(arguments));
}

LoxObject Interpreter::Evaluate(ExprPtr& expr) { return expr->Accept(*this); }

void Interpreter::CheckNumberOperands(Token op, LoxObject left,
                                      LoxObject right) {
  if (left.is<double>() && right.is<double>()) {
    return;
  }
  throw RuntimeError(op, "Operands must be numbers.");
}

void Interpreter::Execute(StmtPtr& stmt) { stmt->Accept(*this); }

void Interpreter::ExecuteBlock(std::vector<StmtPtr>& statements,
                               std::shared_ptr<Environment> environment) {
  std::shared_ptr<Environment> previous = environment_;
  try {
    environment_ = environment;
    for (auto& statement : statements) {
      Execute(statement);
    }
  } catch (...) {
    environment_ = previous;
    throw;
  }
  environment_ = previous;
}

}  // namespace lox