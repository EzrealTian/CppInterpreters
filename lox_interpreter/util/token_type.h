#ifndef LOX_UTIL_TOKEN_TYPE_H_
#define LOX_UTIL_TOKEN_TYPE_H_

#include <string>
#include <iostream>

namespace lox {

enum class TokenType {
  // single-character tokens
  LEFT_PAREN,
  RIGHT_PAREN,
  LEFT_BRACE,
  RIGHT_BRACE,
  COMMA,
  DOT,
  MINUS,
  PLUS,
  SEMICOLON,
  SLASH,
  STAR,

  // one or two character tokens
  BANG,
  BANG_EQUAL,
  EQUAL,
  EQUAL_EQUAL,
  GREATER,
  GREATER_EQUAL,
  LESS,
  LESS_EQUAL,

  // literals
  IDENTIFIER,
  STRING,
  NUMBER,

  // keywords
  AND,
  CLASS,
  ELSE,
  FALSE,
  FUN,
  FOR,
  IF,
  NIL,
  OR,
  PRINT,
  RETURN,
  SUPER,
  THIS,
  TRUE,
  VAR,
  WHILE,
  BREAK,

  // end of file
  EEOF
};

// TokenType转字符串函数声明
std::string tokenTypeToString(TokenType type);

TokenType stringToTokenType(const std::string& str);

// 运算符重载（方便输出）
inline std::ostream& operator<<(std::ostream& os, TokenType type) {
  os << tokenTypeToString(type);
  return os;
}

}  // namespace lox

#endif  // LOX_UTIL_TOKEN_TYPE_H_