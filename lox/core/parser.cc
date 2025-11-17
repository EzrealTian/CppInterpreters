#include "lox/core/parser.h"
#include "lox/ast/stmt.h"
#include "lox/core/lox.h"

namespace lox {

// ==================== Public Interface ====================

std::vector<StmtPtr> Parser::Parse() {
  std::vector<StmtPtr> statements;
  while (!IsAtEnd()) {
    statements.push_back(Declaration());
  }
  return statements;
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

bool Parser::IsAtEnd() { return Peek().type() == TokenType::EEOF; }

Token Parser::Previous() { return tokens_[current_ - 1]; }

Token Parser::Peek() { return tokens_[current_]; }

Token Parser::Consume(TokenType type, const std::string& message) {
  if (Check(type)) return Advance();
  throw Error(Peek(), message);
}

Parser::ParseError Parser::Error(Token token, const std::string& message) {
  Lox::Instance().Error(token, message);
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

// ==================== Grammar Parsing Expr Functions ====================

ExprPtr Parser::Expression() { return ParseEquality(); }

ExprPtr Parser::ParseEquality() {
  return ParseBinary([this]() { return ParseComparison(); },
                     {TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL});
}

ExprPtr Parser::ParseComparison() {
  return ParseBinary([this]() { return ParseTerm(); },
                     {TokenType::GREATER, TokenType::GREATER_EQUAL,
                      TokenType::LESS, TokenType::LESS_EQUAL});
}

ExprPtr Parser::ParseTerm() {
  return ParseBinary([this]() { return ParseFactor(); },
                     {TokenType::MINUS, TokenType::PLUS});
}

ExprPtr Parser::ParseFactor() {
  return ParseBinary([this]() { return ParseUnary(); },
                     {TokenType::SLASH, TokenType::STAR});
}

ExprPtr Parser::ParseBinary(std::function<ExprPtr()> next,
                            std::initializer_list<TokenType> types) {
  ExprPtr expr = next();
  while (Match(types)) {
    Token op = Previous();
    ExprPtr right = next();
    expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
  }
  return expr;
}

ExprPtr Parser::ParseUnary() {
  if (Match({TokenType::BANG, TokenType::MINUS})) {
    Token op = Previous();
    ExprPtr right = ParseUnary();
    return std::make_unique<UnaryExpr>(op, std::move(right));
  }
  return ParsePrimary();
}

ExprPtr Parser::ParsePrimary() {
  if (Match({TokenType::TRUE}))
    return std::make_unique<LiteralExpr>(LoxObject(true));

  if (Match({TokenType::FALSE}))
    return std::make_unique<LiteralExpr>(LoxObject(false));

  if (Match({TokenType::NIL}))
    return std::make_unique<LiteralExpr>(LoxObject(nullptr));

  if (Match({TokenType::NUMBER, TokenType::STRING}))
    return std::make_unique<LiteralExpr>(Previous().literal());

  if (Match({TokenType::IDENTIFIER})) {
    Token name = Previous();
    return std::make_unique<VariableExpr>(name);
  }

  if (Match({TokenType::LEFT_PAREN})) {
    ExprPtr expr = Expression();
    Consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
    return std::make_unique<GroupingExpr>(std::move(expr));
  }

  throw Error(Peek(), "Expect expression.");
}

// ==================== Grammar Parsing Stmt Functions ====================

StmtPtr Parser::Statement() {
  if (Match({TokenType::PRINT})) return PrintStatement();
  return ExprStatement();
}

StmtPtr Parser::PrintStatement() {
  ExprPtr value = Expression();
  Consume(TokenType::SEMICOLON, "Expect ';' after value.");
  return std::make_unique<PrintStmt>(std::move(value));
}

StmtPtr Parser::ExprStatement() {
  ExprPtr expr = Expression();
  Consume(TokenType::SEMICOLON, "Expect ';' after expression.");
  return std::make_unique<ExprStmt>(std::move(expr));
}

StmtPtr Parser::Declaration() {
  try {
    if (Match({TokenType::VAR})) return VarDeclaration();
    return Statement();
  } catch (const ParseError& error) {
    Synchronize();
    return nullptr;
  }
}

StmtPtr Parser::VarDeclaration() {
  Token name = Consume(TokenType::IDENTIFIER, "Expect variable name.");
  ExprPtr initializer = nullptr;
  if (Match({TokenType::EQUAL})) {
    initializer = Expression();
  }
  Consume(TokenType::SEMICOLON, "Expect ';' after variable declaration.");
  return std::make_unique<VarStmt>(name, std::move(initializer));
}
}  // namespace lox