#include "lox/core/scanner.h"
#include "lox/core/lox.h"
#include "lox/util/token_type.h"

namespace lox {

void Scanner::BlockComment() {
  int level = 1;
  while (level > 0 && !IsAtEnd()) {
    if (Match('/') && Match('*')) {
      ++level;
    } else if (Match('*') && Match('/')) {
      --level;
    } else if (Match('\n')) {
      ++line_;
    } else {
      Advance();
    }
  }
  if (level > 0) {
    Lox::Instance().Error(line_, "Unterminated block comment.");
    return;
  }
}

void Scanner::String() {
  while (Peek() != '"' && !IsAtEnd()) {
    if (Peek() == '\n') {
      ++line_;
    }
    Advance();
  }
  if (IsAtEnd()) {
    Lox::Instance().Error(line_, "Unterminated string.");
    return;
  }
  Advance();
  AddToken(TokenType::STRING,
           LoxObject(source_.substr(start_ + 1, current_ - start_ - 2)));
}

void Scanner::Number() {
  while (IsDigit(Peek())) {
    Advance();
  }
  if (Peek() == '.' && IsDigit(PeekNext())) {
    Advance();
    while (IsDigit(Peek())) {
      Advance();
    }
  }
  AddToken(TokenType::NUMBER,
           LoxObject(std::stod(source_.substr(start_, current_ - start_))));
}

void Scanner::Identifier() {
  while (IsAlpha(Peek()) || IsDigit(Peek())) {
    Advance();
  }
  std::string str = source_.substr(start_, current_ - start_);
  AddToken(stringToTokenType(str));
}

void Scanner::AddToken(TokenType type) { AddToken(type, nullptr); }

void Scanner::AddToken(TokenType type, const LoxObject& literal) {
  std::string str = source_.substr(start_, current_ - start_);
  tokens_.push_back(Token(type, str, literal, line_));
}

void Scanner::ScanToken() {
  char c = Advance();
  switch (c) {
    case '(':
      AddToken(TokenType::LEFT_PAREN);
      break;
    case ')':
      AddToken(TokenType::RIGHT_PAREN);
      break;
    case '{':
      AddToken(TokenType::LEFT_BRACE);
      break;
    case '}':
      AddToken(TokenType::RIGHT_BRACE);
      break;
    case ',':
      AddToken(TokenType::COMMA);
      break;
    case '.':
      AddToken(TokenType::DOT);
      break;
    case '-':
      AddToken(TokenType::MINUS);
      break;
    case '+':
      AddToken(TokenType::PLUS);
      break;
    case ';':
      AddToken(TokenType::SEMICOLON);
      break;
    case '*':
      AddToken(TokenType::STAR);
      break;
    case '!':
      AddToken(Match('=') ? TokenType::BANG_EQUAL : TokenType::BANG);
      break;
    case '=':
      AddToken(Match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL);
      break;
    case '<':
      AddToken(Match('=') ? TokenType::LESS_EQUAL : TokenType::LESS);
      break;
    case '>':
      AddToken(Match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER);
      break;
    case '/':
      if (Match('/')) {
        while (Peek() != '\n' && !IsAtEnd()) Advance();
      } else if (Match('*')) {
        BlockComment();
      } else {
        AddToken(TokenType::SLASH);
      }
      break;
    case '"':
      String();
      break;
    case ' ':
    case '\r':
    case '\t':
      break;
    case '\n':
      ++line_;
      break;
    default:
      if (IsDigit(c)) {
        Number();
      } else if (IsAlpha(c)) {
        Identifier();
      } else {
        Lox::Instance().Error(line_, "Unexpected character.");
      }
      break;
  }
}

std::vector<Token> Scanner::ScanTokens() {
  while (!IsAtEnd()) {
    start_ = current_;
    ScanToken();
  }
  tokens_.push_back(Token(TokenType::EEOF, "", nullptr, line_));

  return tokens_;
}

}  // namespace lox