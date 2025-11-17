#ifndef LOX_AST_STMT_H_
#define LOX_AST_STMT_H_

#include "lox/ast/expr.h"

#include <memory>

namespace lox {

class Stmt {
 public:
  virtual ~Stmt() = default;
  virtual void Accept(StmtVisitor& visitor) = 0;
};

using StmtPtr = std::unique_ptr<Stmt>;

class ExprStmt : public Stmt {
 public:
  ExprStmt(ExprPtr expr) : expr_(std::move(expr)) {}

  void Accept(StmtVisitor& visitor) override { visitor.Visit(*this); }

  ExprPtr expr_;
};

class PrintStmt : public Stmt {
 public:
  PrintStmt(ExprPtr expr) : expr_(std::move(expr)) {}

  void Accept(StmtVisitor& visitor) override { visitor.Visit(*this); }

  ExprPtr expr_;
};

class VarStmt : public Stmt {
 public:
  VarStmt(Token name, ExprPtr initializer)
      : name_(std::move(name)), initializer_(std::move(initializer)) {}

  void Accept(StmtVisitor& visitor) override { visitor.Visit(*this); }

  Token name_;
  ExprPtr initializer_;
};

}  // namespace lox

#endif  // LOX_AST_STMT_H_