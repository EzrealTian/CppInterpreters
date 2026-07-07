#ifndef LOX_CORE_LOX_H_
#define LOX_CORE_LOX_H_

#include <string>

#include "lox_interpreter/core/token.h"
#include "lox_interpreter/util/runtime_error.h"

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

  // --- 以下三个方法仅用于测试 ---
  bool HadError() const { return has_error_; }
  bool HadRuntimeError() const { return has_runtime_error_; }
  void ResetErrors() {
    has_error_ = false;
    has_runtime_error_ = false;
  }

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