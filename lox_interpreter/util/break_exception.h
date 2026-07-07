#ifndef LOX_UTIL_BREAK_EXCEPTION_H_
#define LOX_UTIL_BREAK_EXCEPTION_H_

#include <exception>

namespace lox {

class BreakException : public std::exception {
 public:
  BreakException() = default;
  const char* what() const noexcept override {
    return "Error: Use break statement outside of loop";
  }
};

}  // namespace lox

#endif  // LOX_UTIL_BREAK_EXCEPTION_H_
