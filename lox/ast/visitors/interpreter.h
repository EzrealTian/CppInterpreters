#ifndef LOX_AST_VISITORS_INTERPRETER_H_
#define LOX_AST_VISITORS_INTERPRETER_H_

#include "lox/ast/visitor.h"
#include "lox/ast/expr.h"
#include "lox/ast/stmt.h"
#include "lox/util/lox_object.h"
#include "lox/core/environment.h"

#include <vector>

namespace lox {

// 前向声明
class FunctionCallable;

class Interpreter : public ExprVisitor, public StmtVisitor {
  friend class FunctionCallable;

 public:
  Interpreter();

  void Interpret(std::vector<StmtPtr> statements);

  LoxObject Visit(LiteralExpr& expr) override;

  LoxObject Visit(GroupingExpr& expr) override;

  LoxObject Visit(UnaryExpr& expr) override;

  LoxObject Visit(BinaryExpr& expr) override;

  LoxObject Visit(VariableExpr& variable) override;

  LoxObject Visit(AssignExpr& assign) override;

  LoxObject Visit(LogicalExpr& logical) override;

  LoxObject Visit(CallExpr& call) override;

  void Visit(BlockStmt& block_stmt) override;

  void Visit(ExprStmt& expr_stmt) override;

  void Visit(PrintStmt& print_stmt) override;

  void Visit(VarStmt& var_stmt) override;

  void Visit(IfStmt& if_stmt) override;

  void Visit(WhileStmt& while_stmt) override;

  void Visit(BreakStmt& break_stmt) override;

  void Visit(FunctionStmt& function_stmt) override;

  void Visit(ReturnStmt& return_stmt) override;

 private:
  LoxObject Evaluate(ExprPtr& expr);

  void CheckNumberOperands(Token op, LoxObject left, LoxObject right);

  void Execute(StmtPtr& stmt);

  void ExecuteBlock(std::vector<StmtPtr>& statements,
                    std::shared_ptr<Environment> environment);

  std::shared_ptr<Environment> global_env_;
  std::shared_ptr<Environment> environment_;
};

}  // namespace lox
#endif