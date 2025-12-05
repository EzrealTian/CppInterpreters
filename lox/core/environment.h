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
  Environment() = default;
  explicit Environment(std::unique_ptr<Environment> enclosing)
      : enclosing_(std::move(enclosing)) {}
  Environment(const Environment& other) : values_(other.values_) {
    if (other.enclosing_) {
      enclosing_ = std::make_unique<Environment>(*other.enclosing_);
    }
  }
  Environment& operator=(const Environment& other) {
    if (this == &other) {
      return *this;
    }
    values_ = other.values_;
    if (other.enclosing_) {
      enclosing_ = std::make_unique<Environment>(*other.enclosing_);
    } else {
      enclosing_.reset();
    }
    return *this;
  }
  Environment(Environment&&) = default;
  Environment& operator=(Environment&&) = default;
  ~Environment() = default;

  LoxObject Get(const Token& name) {
    if (values_.find(name.lexeme()) != values_.end()) {
      return values_[name.lexeme()];
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
  std::unique_ptr<Environment> enclosing_;
};

}  // namespace lox

#endif  // LOX_CORE_ENVIRONMENT_H_