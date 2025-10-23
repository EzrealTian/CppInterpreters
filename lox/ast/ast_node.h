#ifndef LOX_AST_AST_NODE_H_
#define LOX_AST_AST_NODE_H_

#include "lox/ast/visitor.h"

namespace lox {

class AstNode {
 public:
  virtual ~AstNode() = default;

  virtual LoxObject Accept(Visitor& visitor) = 0;
};

}  // namespace lox

#endif  // LOX_AST_AST_NODE_H_