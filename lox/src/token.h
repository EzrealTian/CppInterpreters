#ifndef LOX_SRC_TOKEN_H_
#define LOX_SRC_TOKEN_H_

#include <string>

#include "lox/util/token_type.h"
#include "lox/util/lox_object.h"

namespace lox {

class Token {
 public:
  Token(TokenType type, const std::string& lexeme, const LoxObject& literal,
        int line)
      : type_(type), lexeme_(lexeme), literal_(literal), line_(line) {}

  std::string toString() const;

 private:
  TokenType type_;
  std::string lexeme_;
  LoxObject literal_;
  int line_;
};

}  // namespace lox

#endif  // LOX_SRC_TOKEN_H_