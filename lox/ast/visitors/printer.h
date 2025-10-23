#ifndef LOX_AST_VISITORS_PRINTER_H_
#define LOX_AST_VISITORS_PRINTER_H_

#include <string>
#include <initializer_list>

#include "lox/ast/visitor.h"
#include "lox/ast/expression.h"

namespace lox {

class Printer : public Visitor {
 public:
  std::string Print(Expression& expr) {
    result_.clear();
    expr.Accept(*this);
    return result_;
  }

  LoxObject Visit(BinaryExpr& binary) override {
    Parenthesize(binary.op_.lexeme(),
                 {binary.left_.get(), binary.right_.get()});
    return nullptr;
  }

  LoxObject Visit(UnaryExpr& unary) override {
    Parenthesize(unary.op_.lexeme(), {unary.right_.get()});
    return nullptr;
  }

  LoxObject Visit(LiteralExpr& literal) override {
    result_ += literal.value_.ToString();
    return nullptr;
  }

  LoxObject Visit(GroupingExpr& grouping) override {
    Parenthesize("group", {grouping.expression_.get()});
    return nullptr;
  }

 private:
  void Parenthesize(const std::string& name,
                    std::initializer_list<Expression*> exprs) {
    result_ += "(" + name;
    for (Expression* expr : exprs) {
      result_ += " ";
      expr->Accept(*this);
    }
    result_ += ")";
  }

  std::string result_;
};

}  // namespace lox

#endif  // LOX_AST_VISITORS_PRINTER_H_