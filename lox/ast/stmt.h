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

class IfStmt : public Stmt {
 public:
  IfStmt(ExprPtr condition, StmtPtr then_branch, StmtPtr else_branch)
      : condition_(std::move(condition)),
        then_branch_(std::move(then_branch)),
        else_branch_(std::move(else_branch)) {}

  void Accept(StmtVisitor& visitor) override { visitor.Visit(*this); }

  ExprPtr condition_;
  StmtPtr then_branch_;
  StmtPtr else_branch_;
};

class WhileStmt : public Stmt {
 public:
  WhileStmt(ExprPtr condition, StmtPtr body)
      : condition_(std::move(condition)), body_(std::move(body)) {}

  void Accept(StmtVisitor& visitor) override { visitor.Visit(*this); }

  ExprPtr condition_;
  StmtPtr body_;
};

class BreakStmt : public Stmt {
 public:
  BreakStmt() = default;

  void Accept(StmtVisitor& visitor) override { visitor.Visit(*this); }
};

class FunctionStmt : public Stmt {
 public:
  FunctionStmt(Token name, std::vector<Token> parameters,
               std::vector<StmtPtr> body)
      : name_(std::move(name)),
        parameters_(std::move(parameters)),
        body_(std::move(body)) {}

  void Accept(StmtVisitor& visitor) override { visitor.Visit(*this); }

  Token name_;
  std::vector<Token> parameters_;
  std::vector<StmtPtr> body_;
};

}  // namespace lox

#endif  // LOX_AST_STMT_H_