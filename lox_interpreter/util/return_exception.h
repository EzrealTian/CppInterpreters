#ifndef LOX_UTIL_RETURN_EXCEPTION_H_
#define LOX_UTIL_RETURN_EXCEPTION_H_

#include <exception>

#include "lox_interpreter/util/lox_object.h"

namespace lox {

class ReturnException : public std::exception {
 public:
  ReturnException(LoxObject value) : value_(std::move(value)) {}

  LoxObject value_;
};

}  // namespace lox

#endif  // LOX_UTIL_RETURN_EXCEPTION_H_