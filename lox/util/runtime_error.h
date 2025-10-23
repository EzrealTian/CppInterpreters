#ifndef LOX_UTIL_RUNTIME_ERROR_H_
#define LOX_UTIL_RUNTIME_ERROR_H_

#include <string>
#include <stdexcept>

#include "lox/core/token.h"

namespace lox {

class RuntimeError : public std::runtime_error {
 public:
  explicit RuntimeError(Token token, const std::string& message)
      : std::runtime_error(message), token_(token) {}
  Token token_;
};

}  // namespace lox

#endif  // LOX_UTIL_RUNTIME_ERROR_H_