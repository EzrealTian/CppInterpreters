#include "lox/util/lox_instance.h"
#include "lox/util/lox_class.h"

namespace lox {

LoxInstance::LoxInstance(std::shared_ptr<LoxClass> klass)
    : klass_(std::move(klass)) {}

std::string LoxInstance::ToString() const {
  return klass_->name() + " instance";
}

LoxObject LoxInstance::Get(Token name) {
  if (fields_.find(name.lexeme()) != fields_.end()) {
    return fields_.at(name.lexeme());
  }

  std::shared_ptr<FunctionCallable> method = klass_->FindMethod(name.lexeme());
  if (method != nullptr) {
    // 绑定 this，得到一个新的方法对象
    std::shared_ptr<FunctionCallable> bound_method =
        method->Bind(LoxObject(shared_from_this()));
    return LoxObject(bound_method);
  }
  throw RuntimeError(name, "Undefined property '" + name.lexeme() + "'.");
}

void LoxInstance::Set(Token name, LoxObject value) {
  fields_[name.lexeme()] = value;
}

}  // namespace lox
