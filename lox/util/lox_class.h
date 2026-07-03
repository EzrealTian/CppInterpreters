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
  LoxClass(std::string name) : LoxInstance(nullptr), name_(std::move(name)) {}

  LoxClass(std::string name, std::shared_ptr<LoxClass> super_class,
           std::unordered_map<std::string, std::shared_ptr<FunctionCallable>>
               methods,
           std::unordered_map<std::string, std::shared_ptr<FunctionCallable>>
               static_methods = {},
           std::unordered_map<std::string, std::shared_ptr<FunctionCallable>>
               getters = {},
           std::unordered_map<std::string, std::shared_ptr<FunctionCallable>>
               static_getters = {})
      : LoxInstance(nullptr),
        name_(std::move(name)),
        super_class_(std::move(super_class)),
        methods_(std::move(methods)),
        static_methods_(std::move(static_methods)),
        getters_(std::move(getters)),
        static_getters_(std::move(static_getters)) {}

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
    if (super_class_ != nullptr) {
      return super_class_->FindMethod(name);
    }
    return nullptr;
  }

  std::shared_ptr<FunctionCallable> FindStaticMethod(const std::string& name) {
    if (static_methods_.find(name) != static_methods_.end()) {
      return static_methods_.at(name);
    }
    if (super_class_ != nullptr) {
      return super_class_->FindStaticMethod(name);
    }
    return nullptr;
  }

  std::shared_ptr<FunctionCallable> FindGetter(const std::string& name) {
    if (getters_.find(name) != getters_.end()) {
      return getters_.at(name);
    }
    if (super_class_ != nullptr) {
      return super_class_->FindGetter(name);
    }
    return nullptr;
  }

  std::shared_ptr<FunctionCallable> FindStaticGetter(const std::string& name) {
    if (static_getters_.find(name) != static_getters_.end()) {
      return static_getters_.at(name);
    }
    if (super_class_ != nullptr) {
      return super_class_->FindStaticGetter(name);
    }
    return nullptr;
  }

  // Override Get: for a class object, look up static getters and static
  // methods. Fields (set on the class) take priority.
  LoxObject Get(Token name, Interpreter& interpreter) override {
    // 1. Check fields (allows setting arbitrary properties on the class)
    if (fields_.find(name.lexeme()) != fields_.end()) {
      return fields_.at(name.lexeme());
    }

    // 2. Check static getters — auto-invoke, return result
    std::shared_ptr<FunctionCallable> static_getter =
        FindStaticGetter(name.lexeme());
    if (static_getter != nullptr) {
      std::shared_ptr<FunctionCallable> bound_getter =
          static_getter->Bind(LoxObject(SelfAsClass()));
      return (*bound_getter)(interpreter, {});
    }

    // 3. Check static methods — bind and return callable
    std::shared_ptr<FunctionCallable> static_method =
        FindStaticMethod(name.lexeme());
    if (static_method != nullptr) {
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
  std::shared_ptr<LoxClass> super_class_;
  std::unordered_map<std::string, std::shared_ptr<FunctionCallable>> methods_;
  std::unordered_map<std::string, std::shared_ptr<FunctionCallable>>
      static_methods_;
  std::unordered_map<std::string, std::shared_ptr<FunctionCallable>> getters_;
  std::unordered_map<std::string, std::shared_ptr<FunctionCallable>>
      static_getters_;
};
}  // namespace lox

#endif  // LOX_UTIL_LOX_CLASS_H_
