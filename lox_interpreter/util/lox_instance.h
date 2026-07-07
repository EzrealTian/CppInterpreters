#ifndef LOX_UTIL_LOX_INSTANCE_H_
#define LOX_UTIL_LOX_INSTANCE_H_

#include <memory>
#include <string>
#include <unordered_map>

#include "lox_interpreter/util/lox_object.h"
#include "lox_interpreter/core/token.h"

namespace lox {

class LoxClass;
class Interpreter;

class LoxInstance : public std::enable_shared_from_this<LoxInstance> {
 public:
  LoxInstance(std::shared_ptr<LoxClass> klass);
  virtual ~LoxInstance() = default;

  virtual std::string ToString();

  virtual LoxObject Get(Token name, Interpreter& interpreter);

  virtual void Set(Token name, LoxObject value);

 protected:
  std::shared_ptr<LoxClass> klass_;
  std::unordered_map<std::string, LoxObject> fields_;
};
}  // namespace lox

#endif  // LOX_UTIL_LOX_INSTANCE_H_