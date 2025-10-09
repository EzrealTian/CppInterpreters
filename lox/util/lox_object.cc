#include "lox/util/lox_object.h"

namespace lox {

std::string LoxObject::toString() const {
  switch (value_.index()) {
    case TypeIndex::STRING:
      return std::get<std::string>(value_);
    case TypeIndex::NUMBER:
      return std::to_string(std::get<double>(value_));
    case TypeIndex::BOOLEAN:
      return std::get<bool>(value_) ? "true" : "false";
    case TypeIndex::NIL:
      return "nil";
    default:
      return "";
  }
}

bool LoxObject::isTruthy() const {
  switch (value_.index()) {
    case TypeIndex::STRING:
      return !std::get<std::string>(value_).empty();
    case TypeIndex::NUMBER:
      return std::get<double>(value_) != 0.0;
    case TypeIndex::BOOLEAN:
      return std::get<bool>(value_);
    case TypeIndex::NIL:
      return false;
    default:
      return false;
  }
}

std::string LoxObject::typeName() const {
  switch (value_.index()) {
    case TypeIndex::STRING:
      return "string";
    case TypeIndex::NUMBER:
      return "number";
    case TypeIndex::BOOLEAN:
      return "boolean";
    case TypeIndex::NIL:
      return "nil";
    default:
      return "unknown";
  }
}

}  // namespace lox
