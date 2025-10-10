#ifndef LOX_AST_VISITOR_H_
#define LOX_AST_VISITOR_H_

namespace lox {

class Binary;
class Unary;
class Literal;
class Grouping;

class Visitor {
 public:
  virtual ~Visitor() = default;

  virtual void visit(Binary& binary) = 0;
  virtual void visit(Unary& unary) = 0;
  virtual void visit(Literal& literal) = 0;
  virtual void visit(Grouping& grouping) = 0;
};

}  // namespace lox

#endif  // LOX_AST_VISITOR_H_