#ifndef LOX_AST_STMT_H_
#define LOX_AST_STMT_H_

#include "lox/ast/expr.h"

#include <memory>
#include <vector>

namespace lox {

class Stmt {
 public:
  virtual ~Stmt() = default;
  virtual void Accept(StmtVisitor& visitor) = 0;
};

using StmtPtr = std::unique_ptr<Stmt>;

class BlockStmt : public Stmt {
 public:
  BlockStmt(std::vector<StmtPtr> statements)
      : statements_(std::move(statements)) {}

  void Accept(StmtVisitor& visitor) override { visitor.Visit(*this); }

  std::vector<StmtPtr> statements_;
};

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