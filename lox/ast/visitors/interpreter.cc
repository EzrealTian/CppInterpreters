#include "lox/ast/visitors/interpreter.h"

#include "lox/core/environment.h"
#include "lox/core/lox.h"
#include "lox/ast/visitor.h"
#include "lox/ast/expr.h"
#include "lox/ast/stmt.h"
#include "lox/util/lox_object.h"
#include "lox/util/runtime_error.h"

#include <vector>

namespace lox {

void Interpreter::Interpret(std::vector<StmtPtr> statements) {
  try {
    for (auto& statement : statements) {
      Execute(std::move(statement));
    }
  } catch (const RuntimeError& error) {
    Lox::Instance().RuntimeError(error);
  }
}

LoxObject Interpreter::Visit(LiteralExpr& expr) { return expr.value_; }

LoxObject Interpreter::Visit(GroupingExpr& expr) {
  return Evaluate(std::move(expr.expression_));
}

LoxObject Interpreter::Visit(UnaryExpr& expr) {
  LoxObject right = Evaluate(std::move(expr.right_));
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
  LoxObject left = Evaluate(std::move(expr.left_));
  LoxObject right = Evaluate(std::move(expr.right_));
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
  return environment_.Get(variable.name_);
}

LoxObject Interpreter::Visit(AssignExpr& assign) {
  LoxObject value = Evaluate(std::move(assign.value_));
  environment_.Assign(assign.name_, value);
  return value;
}

LoxObject Interpreter::Visit(LogicalExpr& logical) {
  LoxObject left = Evaluate(std::move(logical.left_));
  if (logical.op_.type() == TokenType::AND) {
    if (!left.isTruthy()) {
      return left;
    }
  } else if (logical.op_.type() == TokenType::OR) {
    if (left.isTruthy()) {
      return left;
    }
  }
  return Evaluate(std::move(logical.right_));
}

void Interpreter::Visit(BlockStmt& block_stmt) {
  ExecuteBlock(block_stmt.statements_, Environment());
}

void Interpreter::Visit(ExprStmt& expr_stmt) {
  Evaluate(std::move(expr_stmt.expr_));
}

void Interpreter::Visit(PrintStmt& print_stmt) {
  std::cout << Evaluate(std::move(print_stmt.expr_)).ToString() << std::endl;
}

void Interpreter::Visit(VarStmt& var_stmt) {
  LoxObject value = nullptr;
  if (var_stmt.initializer_ != nullptr) {
    value = Evaluate(std::move(var_stmt.initializer_));
  }
  environment_.Define(var_stmt.name_.lexeme(), value);
}

void Interpreter::Visit(IfStmt& if_stmt) {
  if (Evaluate(std::move(if_stmt.condition_)).isTruthy()) {
    Execute(std::move(if_stmt.then_branch_));
  } else if (if_stmt.else_branch_ != nullptr) {
    Execute(std::move(if_stmt.else_branch_));
  }
}

void Interpreter::Visit(WhileStmt& while_stmt) {
  while (Evaluate(std::move(while_stmt.condition_)).isTruthy()) {
    Execute(std::move(while_stmt.body_));
  }
}

LoxObject Interpreter::Evaluate(ExprPtr expr) { return expr->Accept(*this); }

void Interpreter::CheckNumberOperands(Token op, LoxObject left,
                                      LoxObject right) {
  if (left.is<double>() && right.is<double>()) {
    return;
  }
  throw RuntimeError(op, "Operands must be numbers.");
}

void Interpreter::Execute(StmtPtr stmt) { stmt->Accept(*this); }

void Interpreter::ExecuteBlock(std::vector<StmtPtr>& statements,
                               Environment environment) {
  Environment previous = environment_;
  try {
    environment_ = environment;
    for (auto& statement : statements) {
      Execute(std::move(statement));
    }
  } catch (...) {
    environment_ = previous;
  }
}

}  // namespace lox