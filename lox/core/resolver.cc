#include "lox/core/resolver.h"
#include "lox/ast/stmt.h"
#include "lox/core/lox.h"
#include "lox/util/lox_object.h"

namespace lox {

LoxObject Resolver::Visit(VariableExpr& variable_expr) {
  if (!scopes_.empty() &&
      scopes_.back().find(variable_expr.name_.lexeme()) !=
          scopes_.back().end() &&
      !scopes_.back()[variable_expr.name_.lexeme()]) {
    Lox::Instance().Error(variable_expr.name_,
                          "Cannot read local variable in its own initializer.");
  }
  ResolveLocal(variable_expr, variable_expr.name_);
  return nullptr;
}

LoxObject Resolver::Visit(AssignExpr& assign_expr) {
  Resolve(assign_expr.value_);
  ResolveLocal(assign_expr, assign_expr.name_);
  return nullptr;
}

LoxObject Resolver::Visit(CallExpr& call_expr) {
  Resolve(call_expr.callee_);
  for (auto& argument : call_expr.arguments_) {
    Resolve(argument);
  }
  return nullptr;
}

LoxObject Resolver::Visit(BinaryExpr& binary_expr) {
  Resolve(binary_expr.left_);
  Resolve(binary_expr.right_);
  return nullptr;
}

LoxObject Resolver::Visit(GroupingExpr& grouping_expr) {
  Resolve(grouping_expr.expression_);
  return nullptr;
}

LoxObject Resolver::Visit(LiteralExpr& literal_expr) {
  (void)literal_expr;
  return nullptr;
}

LoxObject Resolver::Visit(LogicalExpr& logical_expr) {
  Resolve(logical_expr.left_);
  Resolve(logical_expr.right_);
  return nullptr;
}

LoxObject Resolver::Visit(UnaryExpr& unary_expr) {
  Resolve(unary_expr.right_);
  return nullptr;
}

void Resolver::Visit(BlockStmt& block_stmt) {
  BeginScope();
  Resolve(block_stmt.statements_);
  EndScope();
}

void Resolver::Visit(VarStmt& var_stmt) {
  Declare(var_stmt.name_);
  if (var_stmt.initializer_ != nullptr) {
    Resolve(var_stmt.initializer_);
  }
  Define(var_stmt.name_);
}

void Resolver::Visit(FunctionStmt& function_stmt) {
  Declare(function_stmt.name_);
  Define(function_stmt.name_);
  ResolveFunction(function_stmt, FunctionType::FUNCTION);
}

void Resolver::Visit(ExprStmt& expr_stmt) { Resolve(expr_stmt.expr_); }

void Resolver::Visit(IfStmt& if_stmt) {
  Resolve(if_stmt.condition_);
  Resolve(if_stmt.then_branch_);
  if (if_stmt.else_branch_ != nullptr) {
    Resolve(if_stmt.else_branch_);
  }
}

void Resolver::Visit(PrintStmt& print_stmt) { Resolve(print_stmt.expr_); }

void Resolver::Visit(ReturnStmt& return_stmt) {
  if (current_function_ == FunctionType::NONE) {
    Lox::Instance().Error(return_stmt.keyword_,
                          "Cannot return from top-level code.");
  }
  if (return_stmt.value_ != nullptr) {
    Resolve(return_stmt.value_);
  }
}

void Resolver::Visit(WhileStmt& while_stmt) {
  Resolve(while_stmt.condition_);
  Resolve(while_stmt.body_);
}

void Resolver::Visit(BreakStmt& break_stmt) {
  (void)break_stmt;  // Break 语句不需要解析
}

void Resolver::Resolve(const std::vector<StmtPtr>& statements) {
  for (auto& statement : statements) {
    Resolve(statement);
  }
}

void Resolver::Resolve(const StmtPtr& statement) { statement->Accept(*this); }

void Resolver::Resolve(const ExprPtr& expression) { expression->Accept(*this); }

void Resolver::ResolveLocal(const Expr& expression, const Token& name) {
  for (int i = scopes_.size() - 1; i >= 0; --i) {
    if (scopes_.at(i).find(name.lexeme()) != scopes_.at(i).end()) {
      interpreter_.Resolve(expression, scopes_.size() - i - 1);
      return;
    }
  }
}

void Resolver::ResolveFunction(const FunctionStmt& function_stmt,
                               FunctionType type) {
  FunctionType enclosing_function = current_function_;
  current_function_ = type;
  BeginScope();
  for (auto& param : function_stmt.parameters_) {
    Declare(param);
    Define(param);
  }
  Resolve(function_stmt.body_);
  EndScope();
  current_function_ = enclosing_function;
}

void Resolver::BeginScope() {
  scopes_.emplace_back(std::unordered_map<std::string, bool>());
}

void Resolver::EndScope() { scopes_.pop_back(); }

void Resolver::Declare(const Token& name) {
  if (scopes_.empty()) return;
  if (scopes_.back().find(name.lexeme()) != scopes_.back().end()) {
    Lox::Instance().Error(
        name, "Variable with this name already declared in this scope.");
  }
  scopes_.back()[name.lexeme()] = false;
}

void Resolver::Define(const Token& name) {
  scopes_.back()[name.lexeme()] = true;
}
}  // namespace lox