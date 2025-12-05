#ifndef LOX_AST_VISITORS_INTERPRETER_H_
#define LOX_AST_VISITORS_INTERPRETER_H_

#include "lox/ast/visitor.h"
#include "lox/ast/expr.h"
#include "lox/ast/stmt.h"
#include "lox/util/lox_object.h"
#include "lox/core/environment.h"

#include <vector>

namespace lox {

class Interpreter : public ExprVisitor, public StmtVisitor {
 public:
  void Interpret(std::vector<StmtPtr> statements);

  LoxObject Visit(LiteralExpr& expr) override;

  LoxObject Visit(GroupingExpr& expr) override;

  LoxObject Visit(UnaryExpr& expr) override;

  LoxObject Visit(BinaryExpr& expr) override;

  LoxObject Visit(VariableExpr& variable) override;

  LoxObject Visit(AssignExpr& assign) override;

  void Visit(BlockStmt& block_stmt) override;

  void Visit(ExprStmt& expr_stmt) override;

  void Visit(PrintStmt& print_stmt) override;

  void Visit(VarStmt& var_stmt) override;

 private:
  LoxObject Evaluate(ExprPtr expr);

  void CheckNumberOperands(Token op, LoxObject left, LoxObject right);

  void Execute(StmtPtr stmt);

  void ExecuteBlock(std::vector<StmtPtr>& statements, Environment environment);

  Environment environment_;
};

}  // namespace lox
#endif