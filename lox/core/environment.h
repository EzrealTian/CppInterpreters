#ifndef LOX_CORE_ENVIRONMENT_H_
#define LOX_CORE_ENVIRONMENT_H_

#include <unordered_map>

#include "lox/core/token.h"
#include "lox/util/lox_object.h"
#include "lox/util/runtime_error.h"

namespace lox {

class Environment {
 public:
  Environment() {}
  ~Environment() {}

  void Define(const std::string& name, const LoxObject& value) {
    values_[name] = value;
  }

  LoxObject Get(const Token& name) {
    if (values_.find(name.lexeme()) != values_.end()) {
      return values_[name.lexeme()];
    }
    throw RuntimeError(name, "Undefined variable '" + name.lexeme() + "'.");
  }

  //  private:
  std::unordered_map<std::string, LoxObject> values_;
};

};  // namespace lox

#endif  // LOX_CORE_ENVIRONMENT_H_