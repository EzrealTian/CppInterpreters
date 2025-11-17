#ifndef LOX_AST_VISITOR_H_
#define LOX_AST_VISITOR_H_

#include "lox/util/lox_object.h"
namespace lox {

class BinaryExpr;
class UnaryExpr;
class LiteralExpr;
class GroupingExpr;
class VariableExpr;
class ExprVisitor {
 public:
  virtual ~ExprVisitor() = default;

  virtual LoxObject Visit(BinaryExpr& binary) = 0;
  virtual LoxObject Visit(UnaryExpr& unary) = 0;
  virtual LoxObject Visit(LiteralExpr& literal) = 0;
  virtual LoxObject Visit(GroupingExpr& grouping) = 0;
  virtual LoxObject Visit(VariableExpr& variable) = 0;
};

class ExprStmt;
class PrintStmt;
class VarStmt;
class StmtVisitor {
 public:
  virtual ~StmtVisitor() = default;

  virtual void Visit(ExprStmt& expr_stmt) = 0;
  virtual void Visit(PrintStmt& print_stmt) = 0;
  virtual void Visit(VarStmt& var_stmt) = 0;
};

}  // namespace lox

#endif  // LOX_AST_VISITOR_H_