#ifndef LOX_AST_VISITORS_PRINTER_H_
#define LOX_AST_VISITORS_PRINTER_H_

#include <string>
#include <initializer_list>

#include "lox/ast/visitor.h"
#include "lox/ast/expression.h"

namespace lox {

class Printer : public Visitor {
 public:
  std::string print(Expression& expr) {
    result_.clear();
    expr.accept(*this);
    return result_;
  }

  void visit(BinaryExpr& binary) override {
    parenthesize(binary.op_.lexeme(),
                 {binary.left_.get(), binary.right_.get()});
  }

  void visit(UnaryExpr& unary) override {
    parenthesize(unary.op_.lexeme(), {unary.right_.get()});
  }

  void visit(LiteralExpr& literal) override {
    result_ += literal.value_.toString();
  }

  void visit(GroupingExpr& grouping) override {
    parenthesize("group", {grouping.expression_.get()});
  }

 private:
  void parenthesize(const std::string& name,
                    std::initializer_list<Expression*> exprs) {
    result_ += "(" + name;
    for (Expression* expr : exprs) {
      result_ += " ";
      expr->accept(*this);
    }
    result_ += ")";
  }

  std::string result_;
};

}  // namespace lox

#endif  // LOX_AST_VISITORS_PRINTER_H_