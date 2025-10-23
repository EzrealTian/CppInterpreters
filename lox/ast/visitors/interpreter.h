#ifndef LOX_AST_VISITORS_INTERPRETER_H_
#define LOX_AST_VISITORS_INTERPRETER_H_

#include "lox/core/lox.h"
#include "lox/ast/visitor.h"
#include "lox/ast/expression.h"
#include "lox/util/lox_object.h"
#include "lox/util/runtime_error.h"

namespace lox {

class Interpreter : public Visitor {
 public:
  void Interpret(ExpressionPtr expr) {
    try {
      LoxObject result = Evaluate(std::move(expr));
      std::cout << result.ToString() << std::endl;
    } catch (const RuntimeError& error) {
      Lox::Instance().RuntimeError(error);
    }
  }

  LoxObject Visit(LiteralExpr& expr) override { return expr.value_; }

  LoxObject Visit(GroupingExpr& expr) override {
    return Evaluate(std::move(expr.expression_));
  }

  LoxObject Visit(UnaryExpr& expr) override {
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

  LoxObject Visit(BinaryExpr& expr) override {
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

 private:
  LoxObject Evaluate(ExpressionPtr expr) { return expr->Accept(*this); }

  void CheckNumberOperands(Token op, LoxObject left, LoxObject right) {
    if (left.is<double>() && right.is<double>()) {
      return;
    }
    throw RuntimeError(op, "Operands must be numbers.");
  }
};

}  // namespace lox
#endif