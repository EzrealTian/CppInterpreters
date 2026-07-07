#include "lox_interpreter/ast/visitors/interpreter.h"

#include "lox_interpreter/core/environment.h"
#include "lox_interpreter/core/lox.h"
#include "lox_interpreter/ast/visitor.h"
#include "lox_interpreter/ast/expr.h"
#include "lox_interpreter/ast/stmt.h"
#include "lox_interpreter/util/lox_object.h"
#include "lox_interpreter/util/runtime_error.h"
#include "lox_interpreter/util/break_exception.h"
#include "lox_interpreter/util/lox_callable.h"
#include "lox_interpreter/util/return_exception.h"
#include "lox_interpreter/util/lox_class.h"

#include <vector>

namespace lox {
Interpreter::Interpreter()
    : global_env_(std::make_shared<Environment>()), environment_(global_env_) {
  global_env_->Define("clock", LoxObject(std::make_shared<ClockCallable>()));
}

void Interpreter::Interpret(std::vector<StmtPtr> statements) {
  try {
    for (auto& statement : statements) {
      Execute(statement);
    }
  } catch (const RuntimeError& error) {
    Lox::Instance().RuntimeError(error);
  }
}

LoxObject Interpreter::Visit(LiteralExpr& expr) { return expr.value_; }

LoxObject Interpreter::Visit(GroupingExpr& expr) {
  return Evaluate(expr.expression_);
}

LoxObject Interpreter::Visit(UnaryExpr& expr) {
  LoxObject right = Evaluate(expr.right_);
  switch (expr.op_.type()) {
    case TokenType::MINUS:
      return -right.get<double>();
    case TokenType::BANG:
      return !right.isTruthy();
    default:
      return nullptr;
  }
}

LoxObject Interpreter::Visit(BinaryExpr& expr) {
  LoxObject left = Evaluate(expr.left_);
  LoxObject right = Evaluate(expr.right_);
  switch (expr.op_.type()) {
    case TokenType::PLUS:
      if (left.is<double>() && right.is<double>()) {
        return left.get<double>() + right.get<double>();
      }
      if (left.is<std::string>() && right.is<std::string>()) {
        return left.get<std::string>() + right.get<std::string>();
      }
      throw RuntimeError(expr.op_, "Operands must be numbers or strings.");
    case TokenType::MINUS:
      CheckNumberOperands(expr.op_, left, right);
      return left.get<double>() - right.get<double>();
    case TokenType::STAR:
      CheckNumberOperands(expr.op_, left, right);
      return left.get<double>() * right.get<double>();
    case TokenType::SLASH:
      CheckNumberOperands(expr.op_, left, right);
      if (right.get<double>() == 0) {
        throw RuntimeError(expr.op_, "Division by zero.");
      }
      return left.get<double>() / right.get<double>();
    case TokenType::GREATER:
      CheckNumberOperands(expr.op_, left, right);
      return left.get<double>() > right.get<double>();
    case TokenType::GREATER_EQUAL:
      CheckNumberOperands(expr.op_, left, right);
      return left.get<double>() >= right.get<double>();
    case TokenType::LESS:
      CheckNumberOperands(expr.op_, left, right);
      return left.get<double>() < right.get<double>();
    case TokenType::LESS_EQUAL:
      CheckNumberOperands(expr.op_, left, right);
      return left.get<double>() <= right.get<double>();
    case TokenType::EQUAL_EQUAL:
      return left == right;
    case TokenType::BANG_EQUAL:
      return left != right;
    default:
      return nullptr;
  }
}

LoxObject Interpreter::Visit(VariableExpr& variable) {
  return LookUpVariable(variable.name_, &variable);
}

LoxObject Interpreter::Visit(AssignExpr& assign) {
  LoxObject value = Evaluate(assign.value_);
  if (locals_.find(&assign) != locals_.end()) {
    int distance = locals_.at(&assign);
    environment_->AssignAt(assign.name_.lexeme(), value, distance);
  } else {
    global_env_->Assign(assign.name_, value);
  }
  return value;
}

LoxObject Interpreter::Visit(LogicalExpr& logical) {
  LoxObject left = Evaluate(logical.left_);
  if (logical.op_.type() == TokenType::AND) {
    if (!left.isTruthy()) {
      return left;
    }
  } else if (logical.op_.type() == TokenType::OR) {
    if (left.isTruthy()) {
      return left;
    }
  }
  return Evaluate(logical.right_);
}

void Interpreter::Visit(BlockStmt& block_stmt) {
  ExecuteBlock(block_stmt.statements_,
               std::make_shared<Environment>(environment_));
}

void Interpreter::Visit(ExprStmt& expr_stmt) { Evaluate(expr_stmt.expr_); }

void Interpreter::Visit(PrintStmt& print_stmt) {
  std::cout << Evaluate(print_stmt.expr_).ToString() << std::endl;
}

void Interpreter::Visit(VarStmt& var_stmt) {
  LoxObject value = nullptr;
  if (var_stmt.initializer_ != nullptr) {
    value = Evaluate(var_stmt.initializer_);
  }
  environment_->Define(var_stmt.name_.lexeme(), value);
}

void Interpreter::Visit(IfStmt& if_stmt) {
  if (Evaluate(if_stmt.condition_).isTruthy()) {
    Execute(if_stmt.then_branch_);
  } else if (if_stmt.else_branch_ != nullptr) {
    Execute(if_stmt.else_branch_);
  }
}

void Interpreter::Visit(WhileStmt& while_stmt) {
  while (Evaluate(while_stmt.condition_).isTruthy()) {
    try {
      Execute(while_stmt.body_);
    } catch (const BreakException&) {
      break;
    }
  }
}

void Interpreter::Visit(BreakStmt& break_stmt) {
  (void)break_stmt;  // 未使用参数
  throw BreakException();
}

void Interpreter::Visit(FunctionStmt& function_stmt) {
  std::string function_name = function_stmt.name_.lexeme();
  LoxObject function = LoxObject(std::make_shared<FunctionCallable>(
      std::move(function_stmt), environment_, false));
  environment_->Define(function_name, function);
}

void Interpreter::Visit(ReturnStmt& return_stmt) {
  LoxObject value = nullptr;
  if (return_stmt.value_ != nullptr) {
    value = Evaluate(return_stmt.value_);
  }
  throw ReturnException(value);
}

void Interpreter::Visit(ClassStmt& class_stmt) {
  std::shared_ptr<LoxClass> super_class = nullptr;
  if (class_stmt.superclass_ != nullptr) {
    LoxObject super_class_obj = Evaluate(class_stmt.superclass_);
    if (!super_class_obj.is<LoxClass>()) {
      throw RuntimeError(
          static_cast<VariableExpr&>(*class_stmt.superclass_).name_,
          "Superclass must be a class.");
    }
    super_class = super_class_obj.get<std::shared_ptr<LoxClass>>();
  }

  environment_->Define(class_stmt.name_.lexeme(), nullptr);

  if (class_stmt.superclass_ != nullptr) {
    environment_ = std::shared_ptr<Environment>(environment_);
    environment_->Define("super", super_class);
  }

  std::unordered_map<std::string, std::shared_ptr<FunctionCallable>> methods;
  std::unordered_map<std::string, std::shared_ptr<FunctionCallable>>
      static_methods;
  std::unordered_map<std::string, std::shared_ptr<FunctionCallable>> getters;
  std::unordered_map<std::string, std::shared_ptr<FunctionCallable>>
      static_getters;
  for (auto& method : class_stmt.methods_) {
    std::string method_name = method.name_.lexeme();
    bool is_static = method.is_static_;
    bool is_getter = method.is_getter_;
    // Getters are not initializers and have no parameters
    auto func = std::make_shared<FunctionCallable>(
        std::move(method), environment_, !is_getter && method_name == "init");
    if (is_getter) {
      if (is_static) {
        static_getters[method_name] = func;
      } else {
        getters[method_name] = func;
      }
    } else if (is_static) {
      static_methods[method_name] = func;
    } else {
      methods[method_name] = func;
    }
  }

  // 直接创建 shared_ptr，避免不必要的拷贝
  std::shared_ptr<LoxClass> kClass = std::make_shared<LoxClass>(
      class_stmt.name_.lexeme(), std::move(super_class), std::move(methods),
      std::move(static_methods), std::move(getters), std::move(static_getters));

  if (super_class != nullptr) {
    environment_ = environment_->Enclosing();
  }

  environment_->Assign(class_stmt.name_, LoxObject(kClass));
}

LoxObject Interpreter::Visit(CallExpr& call) {
  LoxObject callee = Evaluate(call.callee_);
  std::vector<LoxObject> arguments;
  for (auto& argument : call.arguments_) {
    arguments.push_back(Evaluate(argument));
  }
  if (!callee.is<LoxCallable>() && !callee.is<LoxClass>()) {
    throw RuntimeError(call.paren_, "Can only call functions and classes.");
  }

  LoxCallable& callable = callee.is<LoxCallable>()
                              ? *callee.get<std::shared_ptr<LoxCallable>>()
                              : *callee.get<std::shared_ptr<LoxClass>>();
  if (callable.arity() != arguments.size()) {
    throw RuntimeError(call.paren_, "Expected " +
                                        std::to_string(callable.arity()) +
                                        " arguments but got " +
                                        std::to_string(arguments.size()));
  }
  return callable(*this, std::move(arguments));
}

LoxObject Interpreter::Visit(GetExpr& expr) {
  LoxObject object = Evaluate(expr.object_);
  if (!object.is<LoxInstance>()) {
    throw RuntimeError(expr.name_, "Only instances have properties.");
  }
  return object.get<std::shared_ptr<LoxInstance>>()->Get(expr.name_, *this);
}

LoxObject Interpreter::Visit(SetExpr& expr) {
  LoxObject object = Evaluate(expr.object_);
  if (!object.is<LoxInstance>()) {
    throw RuntimeError(expr.name_, "Only instances have fields.");
  }

  LoxObject value = Evaluate(expr.value_);
  object.get<std::shared_ptr<LoxInstance>>()->Set(expr.name_, value);
  return value;
}

LoxObject Interpreter::Visit(ThisExpr& this_expr) {
  return LookUpVariable(this_expr.keyword_, &this_expr);
}

LoxObject Interpreter::Visit(SuperExpr& super_expr) {
  int distance = locals_.at(&super_expr);
  auto super_class =
      environment_->GetAt("super", distance).get<std::shared_ptr<LoxClass>>();
  auto object = environment_->GetAt("this", distance - 1)
                    .get<std::shared_ptr<LoxInstance>>();
  auto method = super_class->FindMethod(super_expr.method_.lexeme());

  if (method != nullptr) {
    return LoxObject(method->Bind(LoxObject(object)));
  }

  auto getter = super_class->FindGetter(super_expr.method_.lexeme());
  if (getter != nullptr) {
    auto bound_getter = getter->Bind(LoxObject(object));
    return (*bound_getter)(*this, {});
  }

  throw RuntimeError(
      super_expr.method_,
      "Undefined property '" + super_expr.method_.lexeme() + "'.");
}

LoxObject Interpreter::Evaluate(ExprPtr& expr) { return expr->Accept(*this); }

void Interpreter::CheckNumberOperands(Token op, LoxObject left,
                                      LoxObject right) {
  if (left.is<double>() && right.is<double>()) {
    return;
  }
  throw RuntimeError(op, "Operands must be numbers.");
}

void Interpreter::Execute(StmtPtr& stmt) { stmt->Accept(*this); }

void Interpreter::ExecuteBlock(std::vector<StmtPtr>& statements,
                               std::shared_ptr<Environment> environment) {
  std::shared_ptr<Environment> previous = environment_;
  try {
    environment_ = environment;
    for (auto& statement : statements) {
      Execute(statement);
    }
  } catch (...) {
    environment_ = previous;
    throw;
  }
  environment_ = previous;
}

void Interpreter::Resolve(const Expr& expression, int depth) {
  locals_[&expression] = depth;
}

LoxObject Interpreter::LookUpVariable(const Token& name, const Expr* expr) {
  if (locals_.find(expr) != locals_.end()) {
    int distance = locals_.at(expr);
    return environment_->GetAt(name.lexeme(), distance);
  }
  return global_env_->Get(name);
}

}  // namespace lox