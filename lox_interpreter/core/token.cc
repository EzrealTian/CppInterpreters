#include "lox_interpreter/core/token.h"
#include "lox_interpreter/util/token_type.h"
#include "lox_interpreter/util/lox_object.h"

namespace lox {

std::string Token::ToString() const {
  return tokenTypeToString(type_) + " " + lexeme_ + " " + literal_.ToString();
}

}  // namespace lox