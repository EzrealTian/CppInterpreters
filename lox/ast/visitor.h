#ifndef LOX_AST_VISITOR_H_
#define LOX_AST_VISITOR_H_

namespace lox {

class BinaryExpr;
class UnaryExpr;
class LiteralExpr;
class GroupingExpr;

class Visitor {
 public:
  virtual ~Visitor() = default;

  virtual void visit(BinaryExpr& binary) = 0;
  virtual void visit(UnaryExpr& unary) = 0;
  virtual void visit(LiteralExpr& literal) = 0;
  virtual void visit(GroupingExpr& grouping) = 0;
};

}  // namespace lox

#endif  // LOX_AST_VISITOR_H_