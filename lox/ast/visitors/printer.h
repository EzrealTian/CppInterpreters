#ifndef LOX_AST_VISITORS_PRINTER_H_
#define LOX_AST_VISITORS_PRINTER_H_

#include <string>
#include <initializer_list>

#include "lox/ast/visitor.h"
#include "lox/ast/expr.h"

namespace lox {

class Printer : public ExprVisitor {
 public:
  std::string Print(Expr& expr) {
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

  LoxObject Visit(VariableExpr& variable) override { return nullptr; }

  LoxObject Visit(AssignExpr& assign) override { return nullptr; }

 private:
  void Parenthesize(const std::string& name,
                    std::initializer_list<Expr*> exprs) {
    result_ += "(" + name;
    for (Expr* expr : exprs) {
      result_ += " ";
      expr->Accept(*this);
    }
    result_ += ")";
  }

  std::string result_;
};

}  // namespace lox

#endif  // LOX_AST_VISITORS_PRINTER_H_