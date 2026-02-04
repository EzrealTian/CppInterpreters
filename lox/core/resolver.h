#ifndef LOX_CORE_RESOLVER_H
#define LOX_CORE_RESOLVER_H

#include "lox/ast/expr.h"
#include "lox/ast/stmt.h"
#include "lox/ast/visitors/interpreter.h"
// #include "lox/core/environment.h"
// #include "lox/core/token.h"

#include <vector>
#include <unordered_map>

namespace lox {

class Resolver : public StmtVisitor, public ExprVisitor {
 public:
  Resolver(Interpreter& interpreter) : interpreter_(interpreter) {}

  void Resolve(const std::vector<StmtPtr>& statements);

  void Visit(BlockStmt& block_stmt) override;
  void Visit(ExprStmt& expr_stmt) override;
  void Visit(PrintStmt& print_stmt) override;
  void Visit(VarStmt& var_stmt) override;
  void Visit(IfStmt& if_stmt) override;
  void Visit(WhileStmt& while_stmt) override;
  void Visit(BreakStmt& break_stmt) override;
  void Visit(FunctionStmt& function_stmt) override;
  void Visit(ReturnStmt& return_stmt) override;

  LoxObject Visit(BinaryExpr& binary) override;
  LoxObject Visit(UnaryExpr& unary) override;
  LoxObject Visit(LiteralExpr& literal) override;
  LoxObject Visit(GroupingExpr& grouping) override;
  LoxObject Visit(VariableExpr& variable) override;
  LoxObject Visit(AssignExpr& assign) override;
  LoxObject Visit(LogicalExpr& logical) override;
  LoxObject Visit(CallExpr& call) override;

 private:
  enum class FunctionType {
    NONE,
    FUNCTION,
    INITIALIZER,
    METHOD,
  };

  void Resolve(const StmtPtr& statement);
  void Resolve(const ExprPtr& expression);
  void ResolveLocal(const Expr& expression, const Token& name);
  void ResolveFunction(const FunctionStmt& function_stmt, FunctionType type);

  void BeginScope();
  void EndScope();

  void Declare(const Token& name);
  void Define(const Token& name);

 private:
  Interpreter& interpreter_;
  std::vector<std::unordered_map<std::string, bool>> scopes_;
  FunctionType current_function_ = FunctionType::NONE;
};

}  // namespace lox

#endif  // LOX_CORE_RESOLVER_H