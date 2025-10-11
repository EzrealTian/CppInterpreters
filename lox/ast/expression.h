#ifndef LOX_AST_EXPRESSION_H_
#define LOX_AST_EXPRESSION_H_

#include <memory>

#include "lox/ast/visitor.h"
#include "lox/core/token.h"
#include "lox/ast/ast_node.h"
#include "lox/util/lox_object.h"

namespace lox {

class Expression : public AstNode {
 public:
  virtual ~Expression() = default;

  virtual void accept(Visitor& visitor) = 0;
};

using ExpressionPtr = std::unique_ptr<Expression>;

class BinaryExpr : public Expression {
 public:
  BinaryExpr(ExpressionPtr left, Token op, ExpressionPtr right)
      : left_(std::move(left)), op_(std::move(op)), right_(std::move(right)) {}

  void accept(Visitor& visitor) override { visitor.visit(*this); }

  ExpressionPtr left_;
  Token op_;
  ExpressionPtr right_;
};

class UnaryExpr : public Expression {
 public:
  UnaryExpr(Token op, ExpressionPtr right)
      : op_(std::move(op)), right_(std::move(right)) {}

  void accept(Visitor& visitor) override { visitor.visit(*this); }

  Token op_;
  ExpressionPtr right_;
};

class LiteralExpr : public Expression {
 public:
  explicit LiteralExpr(LoxObject value) : value_(std::move(value)) {}

  void accept(Visitor& visitor) override { visitor.visit(*this); }

  LoxObject value_;
};

class GroupingExpr : public Expression {
 public:
  explicit GroupingExpr(ExpressionPtr expression)
      : expression_(std::move(expression)) {}

  void accept(Visitor& visitor) override { visitor.visit(*this); }

  ExpressionPtr expression_;
};

}  // namespace lox

#endif  // LOX_AST_EXPRESSION_H_