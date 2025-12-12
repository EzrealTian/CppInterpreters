#ifndef LOX_AST_EXPR_H_
#define LOX_AST_EXPR_H_

#include <memory>

#include "lox/ast/visitor.h"
#include "lox/core/token.h"
#include "lox/util/lox_object.h"

namespace lox {

class Expr {
 public:
  virtual ~Expr() = default;
  virtual LoxObject Accept(ExprVisitor& visitor) = 0;
};

using ExprPtr = std::unique_ptr<Expr>;

class BinaryExpr : public Expr {
 public:
  BinaryExpr(ExprPtr left, Token op, ExprPtr right)
      : left_(std::move(left)), op_(std::move(op)), right_(std::move(right)) {}

  LoxObject Accept(ExprVisitor& visitor) override {
    return visitor.Visit(*this);
  }

  ExprPtr left_;
  Token op_;
  ExprPtr right_;
};

class UnaryExpr : public Expr {
 public:
  UnaryExpr(Token op, ExprPtr right)
      : op_(std::move(op)), right_(std::move(right)) {}

  LoxObject Accept(ExprVisitor& visitor) override {
    return visitor.Visit(*this);
  }

  Token op_;
  ExprPtr right_;
};

class LiteralExpr : public Expr {
 public:
  explicit LiteralExpr(LoxObject value) : value_(std::move(value)) {}

  LoxObject Accept(ExprVisitor& visitor) override {
    return visitor.Visit(*this);
  }

  LoxObject value_;
};

class GroupingExpr : public Expr {
 public:
  explicit GroupingExpr(ExprPtr expression)
      : expression_(std::move(expression)) {}

  LoxObject Accept(ExprVisitor& visitor) override {
    return visitor.Visit(*this);
  }

  ExprPtr expression_;
};

class VariableExpr : public Expr {
 public:
  VariableExpr(Token name) : name_(std::move(name)) {}

  LoxObject Accept(ExprVisitor& visitor) override {
    return visitor.Visit(*this);
  }

  Token name_;
};

class AssignExpr : public Expr {
 public:
  AssignExpr(Token name, ExprPtr value)
      : name_(std::move(name)), value_(std::move(value)) {}

  LoxObject Accept(ExprVisitor& visitor) override {
    return visitor.Visit(*this);
  }

  Token name_;
  ExprPtr value_;
};

class LogicalExpr : public Expr {
 public:
  LogicalExpr(ExprPtr left, Token op, ExprPtr right)
      : left_(std::move(left)), op_(std::move(op)), right_(std::move(right)) {}

  LoxObject Accept(ExprVisitor& visitor) override {
    return visitor.Visit(*this);
  }

  ExprPtr left_;
  Token op_;
  ExprPtr right_;
};

}  // namespace lox

#endif  // LOX_AST_EXPR_H_