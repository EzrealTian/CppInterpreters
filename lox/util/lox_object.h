#ifndef LOX_UTIL_LOX_OBJECT_H_
#define LOX_UTIL_LOX_OBJECT_H_

#include <string>
#include <variant>
#include <iostream>

namespace lox {

using LoxValue = std::variant<std::string,    // string
                              double,         // number
                              bool,           // boolean
                              std::nullptr_t  // nil
                              >;

// type index constant
namespace TypeIndex {
constexpr size_t STRING = 0;
constexpr size_t NUMBER = 1;
constexpr size_t BOOLEAN = 2;
constexpr size_t NIL = 3;
}  // namespace TypeIndex

class LoxObject {
 public:
  // constructor
  LoxObject() : value_(nullptr) {}

  // constructor from various types
  LoxObject(const std::string& str) : value_(str) {}
  LoxObject(const char* str) : value_(std::string(str)) {}
  LoxObject(double num) : value_(num) {}
  LoxObject(int num) : value_(static_cast<double>(num)) {}
  LoxObject(bool b) : value_(b) {}
  LoxObject(std::nullptr_t) : value_(nullptr) {}

  // assignment operator
  LoxObject& operator=(const std::string& str) {
    value_ = str;
    return *this;
  }

  LoxObject& operator=(const char* str) {
    value_ = std::string(str);
    return *this;
  }

  LoxObject& operator=(double num) {
    value_ = num;
    return *this;
  }

  LoxObject& operator=(int num) {
    value_ = static_cast<double>(num);
    return *this;
  }

  LoxObject& operator=(bool b) {
    value_ = b;
    return *this;
  }

  LoxObject& operator=(std::nullptr_t) {
    value_ = nullptr;
    return *this;
  }

  // get value (type safe)
  template <typename T>
  T get() const {
    return std::get<T>(value_);
  }

  // check type
  template <typename T>
  bool is() const {
    return std::holds_alternative<T>(value_);
  }

  // get index (faster)
  size_t index() const { return value_.index(); }

  // convert to string(use constant to improve maintainability)
  std::string toString() const;

  // check if it is a truthy value
  bool isTruthy() const;

  // get type name
  std::string typeName() const;

  // compare operator
  bool operator==(const LoxObject& other) const {
    return value_ == other.value_;
  }

  bool operator!=(const LoxObject& other) const {
    return value_ != other.value_;
  }

  // output stream operator
  friend std::ostream& operator<<(std::ostream& os, const LoxObject& obj) {
    os << obj.toString();
    return os;
  }

 private:
  LoxValue value_;
};

}  // namespace lox

#endif  // LOX_UTIL_LOX_OBJECT_H_
