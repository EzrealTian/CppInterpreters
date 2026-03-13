#ifndef LOX_UTIL_LOX_CLASS_H_
#define LOX_UTIL_LOX_CLASS_H_

#include <string>
#include <memory>
#include <unordered_map>

#include "lox/util/lox_callable.h"
#include "lox/util/lox_instance.h"

namespace lox {

class LoxClass : public LoxCallable,
                 public std::enable_shared_from_this<LoxClass> {
 public:
  LoxClass(std::string name) : name_(std::move(name)) {}
  LoxClass(std::string name,
           std::unordered_map<std::string, std::shared_ptr<FunctionCallable>>
               methods)
      : name_(std::move(name)), methods_(std::move(methods)) {}
  ~LoxClass() = default;

  std::string name() const { return name_; }

  std::string ToString() override { return "<class " + name_ + ">"; }

  size_t arity() override {
    std::shared_ptr<FunctionCallable> init_method = FindMethod("init");
    if (init_method != nullptr) {
      return init_method->arity();
    }
    return 0;
  }

  LoxObject operator()(Interpreter& interpreter,
                       std::vector<LoxObject> arguments) override {
    // 使用当前类对象（包含所有方法）来创建实例
    std::shared_ptr<LoxInstance> instance =
        std::make_shared<LoxInstance>(shared_from_this());
    std::shared_ptr<FunctionCallable> init_method = FindMethod("init");
    if (init_method != nullptr) {
      std::shared_ptr<FunctionCallable> bound_init =
          init_method->Bind(LoxObject(instance));
      (*bound_init)(interpreter, arguments);
    }
    return LoxObject(instance);
  }

  std::shared_ptr<FunctionCallable> FindMethod(const std::string& name) {
    if (methods_.find(name) != methods_.end()) {
      return methods_.at(name);
    }
    return nullptr;
  }

 private:
  std::string name_;
  std::unordered_map<std::string, std::shared_ptr<FunctionCallable>> methods_;
};
}  // namespace lox

#endif  // LOX_UTIL_LOX_CLASS_H_