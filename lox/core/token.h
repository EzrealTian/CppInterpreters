#ifndef LOX_CORE_TOKEN_H_
#define LOX_CORE_TOKEN_H_

#include <string>

#include "lox/util/token_type.h"
#include "lox/util/lox_object.h"

namespace lox {

class Token {
 public:
  Token(TokenType type, const std::string& lexeme, const LoxObject& literal,
        int line)
      : type_(type), lexeme_(lexeme), literal_(literal), line_(line) {}

  std::string ToString() const;

  std::string lexeme() const { return lexeme_; }

  TokenType type() const { return type_; }
  
  LoxObject literal() const { return literal_; }
  
  int line() const { return line_; }

 private:
  TokenType type_;
  std::string lexeme_;
  LoxObject literal_;
  int line_;
};

}  // namespace lox

#endif  // LOX_CORE_TOKEN_H_