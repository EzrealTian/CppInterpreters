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

class Binary : public Expression {
 public:
  Binary(ExpressionPtr left, Token op, ExpressionPtr right)
      : left_(std::move(left)), op_(std::move(op)), right_(std::move(right)) {}

  void accept(Visitor& visitor) override { visitor.visit(*this); }

  ExpressionPtr left_;
  Token op_;
  ExpressionPtr right_;
};

class Unary : public Expression {
 public:
  Unary(Token op, ExpressionPtr right)
      : op_(std::move(op)), right_(std::move(right)) {}

  void accept(Visitor& visitor) override { visitor.visit(*this); }

  Token op_;
  ExpressionPtr right_;
};

class Literal : public Expression {
 public:
  explicit Literal(LoxObject value) : value_(std::move(value)) {}

  void accept(Visitor& visitor) override { visitor.visit(*this); }

  LoxObject value_;
};

class Grouping : public Expression {
 public:
  explicit Grouping(ExpressionPtr expression)
      : expression_(std::move(expression)) {}

  void accept(Visitor& visitor) override { visitor.visit(*this); }

  ExpressionPtr expression_;
};

}  // namespace lox

#endif  // LOX_AST_EXPRESSION_H_