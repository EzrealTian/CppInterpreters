#ifndef LOX_CORE_SCANNER_H_
#define LOX_CORE_SCANNER_H_

#include "lox/core/token.h"

#include <string>
#include <vector>

namespace lox {

class Scanner {
 public:
  Scanner(const std::string& source) : source_(source) {}

  std::vector<Token> ScanTokens();

 private:
  inline bool IsAtEnd() const { return current_ >= source_.size(); }

  inline bool Match(char expected) {
    if (IsAtEnd()) {
      return false;
    }
    if (source_[current_] != expected) return false;
    ++current_;
    return true;
  }

  inline char Peek() {
    if (IsAtEnd()) {
      return '\0';
    }
    return source_[current_];
  }

  inline char PeekNext() {
    if (current_ + 1 >= source_.size()) {
      return '\0';
    }
    return source_[current_ + 1];
  }

  inline char Advance() { return source_[current_++]; }

  inline bool IsDigit(char c) { return c >= '0' && c <= '9'; }

  inline bool IsAlpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
  }

  void BlockComment();

  void String();

  void Number();

  void Identifier();

  void ScanToken();

  void AddToken(TokenType type);

  void AddToken(TokenType type, const LoxObject& literal);

 private:
  std::string source_;
  std::vector<Token> tokens_;

  size_t start_ = 0;
  size_t current_ = 0;
  size_t line_ = 1;
};

}  // namespace lox

#endif  // LOX_CORE_SCANNER_H_