#ifndef LOX_UTIL_LOX_INSTANCE_H_
#define LOX_UTIL_LOX_INSTANCE_H_

#include <memory>
#include <string>
#include <unordered_map>

#include "lox/util/lox_object.h"
#include "lox/core/token.h"

namespace lox {

class LoxClass;

class LoxInstance : public std::enable_shared_from_this<LoxInstance> {
 public:
  LoxInstance(std::shared_ptr<LoxClass> klass);
  ~LoxInstance() = default;

  std::string ToString() const;

  LoxObject Get(Token name);

  void Set(Token name, LoxObject value);

 private:
  std::shared_ptr<LoxClass> klass_;
  std::unordered_map<std::string, LoxObject> fields_;
};
}  // namespace lox

#endif  // LOX_UTIL_LOX_INSTANCE_H_