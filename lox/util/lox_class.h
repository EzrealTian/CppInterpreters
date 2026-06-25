#ifndef LOX_UTIL_LOX_CLASS_H_
#define LOX_UTIL_LOX_CLASS_H_

#include <string>
#include <memory>
#include <unordered_map>

#include "lox/util/lox_callable.h"
#include "lox/util/lox_instance.h"

namespace lox {

class LoxClass : public LoxCallable, public LoxInstance {
 public:
  LoxClass(std::string name)
      : LoxInstance(nullptr), name_(std::move(name)) {}

  LoxClass(std::string name,
           std::unordered_map<std::string, std::shared_ptr<FunctionCallable>>
               methods,
           std::unordered_map<std::string, std::shared_ptr<FunctionCallable>>
               static_methods = {})
      : LoxInstance(nullptr),
        name_(std::move(name)),
        methods_(std::move(methods)),
        static_methods_(std::move(static_methods)) {}

  ~LoxClass() override = default;

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
        std::make_shared<LoxInstance>(SelfAsClass());
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

  std::shared_ptr<FunctionCallable> FindStaticMethod(const std::string& name) {
    if (static_methods_.find(name) != static_methods_.end()) {
      return static_methods_.at(name);
    }
    return nullptr;
  }

  // Override Get: for a class object, look up static methods instead of
  // instance methods. Fields (set on the class) take priority over static
  // methods.
  LoxObject Get(Token name) override {
    // 1. Check fields (allows setting arbitrary properties on the class)
    if (fields_.find(name.lexeme()) != fields_.end()) {
      return fields_.at(name.lexeme());
    }

    // 2. Check static methods
    std::shared_ptr<FunctionCallable> static_method =
        FindStaticMethod(name.lexeme());
    if (static_method != nullptr) {
      // Bind 'this' to the class itself
      std::shared_ptr<FunctionCallable> bound_method =
          static_method->Bind(LoxObject(SelfAsClass()));
      return LoxObject(bound_method);
    }

    throw RuntimeError(name,
                       "Undefined static property '" + name.lexeme() + "'.");
  }

 private:
  // Get shared_ptr<LoxClass> to self via the single enable_shared_from_this
  // base (LoxInstance).
  std::shared_ptr<LoxClass> SelfAsClass() {
    return std::static_pointer_cast<LoxClass>(shared_from_this());
  }

  std::string name_;
  std::unordered_map<std::string, std::shared_ptr<FunctionCallable>> methods_;
  std::unordered_map<std::string, std::shared_ptr<FunctionCallable>>
      static_methods_;
};
}  // namespace lox

#endif  // LOX_UTIL_LOX_CLASS_H_
