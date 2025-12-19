#include <unordered_map>

#include "lox/util/token_type.h"

namespace lox {

std::string tokenTypeToString(TokenType type) {
  switch (type) {
    // single-character tokens
    case TokenType::LEFT_PAREN:
      return "LEFT_PAREN";
    case TokenType::RIGHT_PAREN:
      return "RIGHT_PAREN";
    case TokenType::LEFT_BRACE:
      return "LEFT_BRACE";
    case TokenType::RIGHT_BRACE:
      return "RIGHT_BRACE";
    case TokenType::COMMA:
      return "COMMA";
    case TokenType::DOT:
      return "DOT";
    case TokenType::MINUS:
      return "MINUS";
    case TokenType::PLUS:
      return "PLUS";
    case TokenType::SEMICOLON:
      return "SEMICOLON";
    case TokenType::SLASH:
      return "SLASH";
    case TokenType::STAR:
      return "STAR";

    // one or two character tokens
    case TokenType::BANG:
      return "BANG";
    case TokenType::BANG_EQUAL:
      return "BANG_EQUAL";
    case TokenType::EQUAL:
      return "EQUAL";
    case TokenType::EQUAL_EQUAL:
      return "EQUAL_EQUAL";
    case TokenType::GREATER:
      return "GREATER";
    case TokenType::GREATER_EQUAL:
      return "GREATER_EQUAL";
    case TokenType::LESS:
      return "LESS";
    case TokenType::LESS_EQUAL:
      return "LESS_EQUAL";

    // literals
    case TokenType::IDENTIFIER:
      return "IDENTIFIER";
    case TokenType::STRING:
      return "STRING";
    case TokenType::NUMBER:
      return "NUMBER";

    // keywords
    case TokenType::AND:
      return "AND";
    case TokenType::CLASS:
      return "CLASS";
    case TokenType::ELSE:
      return "ELSE";
    case TokenType::FALSE:
      return "FALSE";
    case TokenType::FUN:
      return "FUN";
    case TokenType::FOR:
      return "FOR";
    case TokenType::IF:
      return "IF";
    case TokenType::NIL:
      return "NIL";
    case TokenType::OR:
      return "OR";
    case TokenType::PRINT:
      return "PRINT";
    case TokenType::RETURN:
      return "RETURN";
    case TokenType::SUPER:
      return "SUPER";
    case TokenType::THIS:
      return "THIS";
    case TokenType::TRUE:
      return "TRUE";
    case TokenType::VAR:
      return "VAR";
    case TokenType::WHILE:
      return "WHILE";
    case TokenType::BREAK:
      return "BREAK";
    // end of file
    case TokenType::EEOF:
      return "EOF";

    default:
      return "UNKNOWN";
  }
}

TokenType stringToTokenType(const std::string& str) {
  static std::unordered_map<std::string, TokenType> string_to_token_type = {
      {"and", TokenType::AND},     {"or", TokenType::OR},
      {"fun", TokenType::FUN},     {"return", TokenType::RETURN},
      {"super", TokenType::SUPER}, {"this", TokenType::THIS},
      {"class", TokenType::CLASS}, {"if", TokenType::IF},
      {"else", TokenType::ELSE},   {"true", TokenType::TRUE},
      {"false", TokenType::FALSE}, {"for", TokenType::FOR},
      {"while", TokenType::WHILE}, {"nil", TokenType::NIL},
      {"print", TokenType::PRINT}, {"var", TokenType::VAR},
      {"break", TokenType::BREAK},
  };
  return string_to_token_type.find(str) != string_to_token_type.end()
             ? string_to_token_type[str]
             : TokenType::IDENTIFIER;
}

}  // namespace lox
