#ifndef LOX_CORE_LOX_H_
#define LOX_CORE_LOX_H_

#include <string>

namespace lox {

class Lox {
 public:
  static Lox& Instance() {
    static Lox instance;
    return instance;
  }

  void RunFile(const std::string& path);
  void RunPrompt();

  void error(int line, const std::string& message);

 private:
  Lox() = default;

  void run(const std::string& source);

  void report(int line, const std::string& where, const std::string& message);

 private:
  bool has_error_ = false;
};

}  // namespace lox

#endif  // LOX_CORE_LOX_H_