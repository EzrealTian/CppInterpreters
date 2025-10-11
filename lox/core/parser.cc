#include "lox/core/parser.h"
#include "lox/core/lox.h"

namespace lox {

// ==================== Public Interface ====================

ExpressionPtr Parser::ParseExpression() {
  try {
    return ParseEquality();
  } catch (const ParseError& e) {
    // Synchronize();
    return nullptr;
  }
}

// ==================== Helper Functions ====================

bool Parser::Match(std::initializer_list<TokenType> types) {
  for (TokenType type : types) {
    if (Check(type)) {
      Advance();
      return true;
    }
  }
  return false;
}

bool Parser::Check(TokenType type) {
  if (IsAtEnd()) return false;
  return Peek().type() == type;
}

Token Parser::Advance() {
  if (!IsAtEnd()) ++current_;
  return Previous();
}

bool Parser::IsAtEnd() {
  return Peek().type() == TokenType::EEOF;
}

Token Parser::Previous() {
  return tokens_[current_ - 1];
}

Token Parser::Peek() {
  return tokens_[current_];
}

Token Parser::Consume(TokenType type, const std::string& message) {
  if (Check(type)) return Advance();
  throw Error(Peek(), message);
}

Parser::ParseError Parser::Error(Token token, const std::string& message) {
  Lox::Instance().error(token, message);
  return ParseError(message);
}

void Parser::Synchronize() {
  Advance();
  while (!IsAtEnd()) {
    if (Previous().type() == TokenType::SEMICOLON) return;
    switch (Peek().type()) {
      case TokenType::CLASS:
      case TokenType::FUN:
      case TokenType::VAR:
      case TokenType::FOR:
      case TokenType::IF:
      case TokenType::WHILE:
      case TokenType::PRINT:
      case TokenType::RETURN:
      default:
        return;
    }
    Advance();
  }
}

// ==================== Grammar Parsing Functions ====================

ExpressionPtr Parser::ParseEquality() {
  return ParseBinary([this]() { return ParseComparison(); }, 
                     {TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL});
}

ExpressionPtr Parser::ParseComparison() {
  return ParseBinary([this]() { return ParseTerm(); },
                     {TokenType::GREATER, TokenType::GREATER_EQUAL, 
                      TokenType::LESS, TokenType::LESS_EQUAL});
}

ExpressionPtr Parser::ParseTerm() {
  return ParseBinary([this]() { return ParseFactor(); },
                     {TokenType::MINUS, TokenType::PLUS});
}

ExpressionPtr Parser::ParseFactor() {
  return ParseBinary([this]() { return ParseUnary(); },
                     {TokenType::SLASH, TokenType::STAR});
}

ExpressionPtr Parser::ParseBinary(std::function<ExpressionPtr()> next, std::initializer_list<TokenType> types) {
  ExpressionPtr expr = next();
  while (Match(types)) {
    Token op = Previous();
    ExpressionPtr right = next();
    expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
  }
  return expr;
}

ExpressionPtr Parser::ParseUnary() {
  if (Match({TokenType::BANG, TokenType::MINUS})) {
    Token op = Previous();
    ExpressionPtr right = ParseUnary();
    return std::make_unique<UnaryExpr>(op, std::move(right));
  }
  return ParsePrimary();
}

ExpressionPtr Parser::ParsePrimary() {
  if (Match({TokenType::TRUE})) 
    return std::make_unique<LiteralExpr>(LoxObject(true));
  
  if (Match({TokenType::FALSE})) 
    return std::make_unique<LiteralExpr>(LoxObject(false));
  
  if (Match({TokenType::NIL})) 
    return std::make_unique<LiteralExpr>(LoxObject(nullptr));

  if (Match({TokenType::NUMBER, TokenType::STRING})) 
    return std::make_unique<LiteralExpr>(Previous().literal());

  if (Match({TokenType::LEFT_PAREN})) {
    ExpressionPtr expr = ParseExpression();
    Consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
    return std::make_unique<GroupingExpr>(std::move(expr));
  }

  throw Error(Peek(), "Expect expression.");
}

}  // namespace lox