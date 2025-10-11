#include "lox/core/token.h"
#include "lox/util/token_type.h"
#include "lox/util/lox_object.h"

namespace lox {

std::string Token::ToString() const {
  return tokenTypeToString(type_) + " " + lexeme_ + " " + literal_.toString();
}

}  // namespace lox