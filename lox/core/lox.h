#ifndef LOX_CORE_LOX_H_
#define LOX_CORE_LOX_H_

#include <string>

#include "lox/core/token.h"
#include "lox/util/runtime_error.h"

namespace lox {

class Lox {
 public:
  static Lox& Instance() {
    static Lox instance;
    return instance;
  }

  void RunFile(const std::string& path);
  void RunPrompt();

  void Error(int line, const std::string& message);
  void Error(Token token, const std::string& message);
  void RuntimeError(const RuntimeError& error);

 private:
  Lox() = default;

  void run(const std::string& source);

  void Report(int line, const std::string& where, const std::string& message);

 private:
  bool has_error_ = false;
  bool has_runtime_error_ = false;
};

}  // namespace lox

#endif  // LOX_CORE_LOX_H_