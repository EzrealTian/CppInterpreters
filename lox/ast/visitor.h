#ifndef LOX_AST_VISITOR_H_
#define LOX_AST_VISITOR_H_

#include "lox/util/lox_object.h"
namespace lox {

class BinaryExpr;
class UnaryExpr;
class LiteralExpr;
class GroupingExpr;

class Visitor {
 public:
  virtual ~Visitor() = default;

  virtual LoxObject Visit(BinaryExpr& binary) = 0;
  virtual LoxObject Visit(UnaryExpr& unary) = 0;
  virtual LoxObject Visit(LiteralExpr& literal) = 0;
  virtual LoxObject Visit(GroupingExpr& grouping) = 0;
};

}  // namespace lox

#endif  // LOX_AST_VISITOR_H_