#ifndef LOX_CORE_ENVIRONMENT_H_
#define LOX_CORE_ENVIRONMENT_H_

#include <unordered_map>
#include <memory>

#include "lox/core/token.h"
#include "lox/util/lox_object.h"
#include "lox/util/runtime_error.h"

namespace lox {

class Environment {
 public:
  Environment() : enclosing_(nullptr) {}
  explicit Environment(std::shared_ptr<Environment> enclosing)
      : enclosing_(std::move(enclosing)) {}

  // 使用默认的拷贝和移动语义，shared_ptr 会处理引用计数
  Environment(const Environment&) = default;
  Environment& operator=(const Environment&) = default;
  Environment(Environment&&) = default;
  Environment& operator=(Environment&&) = default;
  ~Environment() = default;

  LoxObject Get(const Token& name) {
    if (values_.find(name.lexeme()) != values_.end()) {
      return values_.at(name.lexeme());
    }
    if (enclosing_ != nullptr) {
      return enclosing_->Get(name);
    }
    throw RuntimeError(name, "Undefined variable '" + name.lexeme() + "'.");
  }

  void Assign(const Token& name, const LoxObject& value) {
    if (values_.find(name.lexeme()) != values_.end()) {
      values_[name.lexeme()] = value;
      return;
    }
    if (enclosing_ != nullptr) {
      enclosing_->Assign(name, value);
      return;
    }
    throw RuntimeError(name, "Undefined variable '" + name.lexeme() + "'.");
  }

  void Define(const std::string& name, const LoxObject& value) {
    values_[name] = value;
  }

 private:
  std::unordered_map<std::string, LoxObject> values_;
  std::shared_ptr<Environment> enclosing_;
};

}  // namespace lox

#endif  // LOX_CORE_ENVIRONMENT_H_