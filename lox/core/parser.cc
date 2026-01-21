#include "lox/core/parser.h"
#include "lox/ast/expr.h"
#include "lox/ast/stmt.h"
#include "lox/core/lox.h"
#include "lox/util/token_type.h"

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

ExprPtr Parser::FinishCall(ExprPtr callee) {
  std::vector<ExprPtr> arguments;
  if (!Check(TokenType::RIGHT_PAREN)) {
    do {
      if (arguments.size() >= 255) {
        Error(Peek(), "Can't have more than 255 arguments.");
      }
      arguments.push_back(Expression());
    } while (Match({TokenType::COMMA}));
  }
  Token paren = Consume(TokenType::RIGHT_PAREN, "Expect ')' after arguments.");
  return std::make_unique<CallExpr>(std::move(callee), paren,
                                    std::move(arguments));
}

// ==================== Grammar Parsing Expr Functions ====================

ExprPtr Parser::Expression() { return ParseAssignment(); }

ExprPtr Parser::ParseEquality() {
  ExprPtr expr = ParseComparison();
  while (Match({TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL})) {
    Token op = Previous();
    ExprPtr right = ParseComparison();
    expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
  }
  return expr;
}

ExprPtr Parser::ParseComparison() {
  ExprPtr expr = ParseTerm();
  while (Match({TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS,
                TokenType::LESS_EQUAL})) {
    Token op = Previous();
    ExprPtr right = ParseTerm();
    expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
  }
  return expr;
}

ExprPtr Parser::ParseTerm() {
  ExprPtr expr = ParseFactor();
  while (Match({TokenType::MINUS, TokenType::PLUS})) {
    Token op = Previous();
    ExprPtr right = ParseFactor();
    expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
  }
  return expr;
}

ExprPtr Parser::ParseFactor() {
  ExprPtr expr = ParseUnary();
  while (Match({TokenType::SLASH, TokenType::STAR})) {
    Token op = Previous();
    ExprPtr right = ParseUnary();
    expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
  }
  return expr;
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
  return ParseCall();
}

ExprPtr Parser::ParseCall() {
  ExprPtr expr = ParsePrimary();
  while (true) {
    if (Match({TokenType::LEFT_PAREN})) {
      expr = FinishCall(std::move(expr));
    } else {
      break;
    }
  }
  return expr;
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

ExprPtr Parser::ParseAssignment() {
  ExprPtr expr = ParseOr();

  if (Match({TokenType::EQUAL})) {
    Token equals_token = Previous();
    ExprPtr value = ParseAssignment();

    VariableExpr* variable = dynamic_cast<VariableExpr*>(expr.get());
    if (variable != nullptr) {
      return std::make_unique<AssignExpr>(variable->name_, std::move(value));
    }

    throw Error(equals_token, "Invalid assignment target.");
  }

  return expr;
}

ExprPtr Parser::ParseOr() {
  ExprPtr expr = ParseAnd();
  while (Match({TokenType::OR})) {
    Token op = Previous();
    ExprPtr right = ParseAnd();
    expr = std::make_unique<LogicalExpr>(std::move(expr), op, std::move(right));
  }
  return expr;
}

ExprPtr Parser::ParseAnd() {
  ExprPtr expr = ParseEquality();
  while (Match({TokenType::AND})) {
    Token op = Previous();
    ExprPtr right = ParseEquality();
    expr = std::make_unique<LogicalExpr>(std::move(expr), op, std::move(right));
  }
  return expr;
}

// ==================== Grammar Parsing Stmt Functions ====================

std::vector<StmtPtr> Parser::Block() {
  std::vector<StmtPtr> statements;
  while (!Check(TokenType::RIGHT_BRACE) && !IsAtEnd()) {
    statements.push_back(Declaration());
  }
  Consume(TokenType::RIGHT_BRACE, "Expect '}' after block.");
  return statements;
}

StmtPtr Parser::Statement() {
  if (Match({TokenType::IF})) return IfStatement();
  if (Match({TokenType::FOR})) return ForStatement();
  if (Match({TokenType::WHILE})) return WhileStatement();
  if (Match({TokenType::BREAK})) return BreakStatement();
  if (Match({TokenType::PRINT})) return PrintStatement();
  if (Match({TokenType::RETURN})) return ReturnStatement();
  if (Match({TokenType::LEFT_BRACE}))
    return std::make_unique<BlockStmt>(Block());
  return ExprStatement();
}

StmtPtr Parser::PrintStatement() {
  ExprPtr value = Expression();
  Consume(TokenType::SEMICOLON, "Expect ';' after value.");
  return std::make_unique<PrintStmt>(std::move(value));
}

StmtPtr Parser::ReturnStatement() {
  Token keyword = Previous();
  ExprPtr value = nullptr;
  if (!Check(TokenType::SEMICOLON)) {
    value = Expression();
  }
  Consume(TokenType::SEMICOLON, "Expect ';' after return value.");
  return std::make_unique<ReturnStmt>(keyword, std::move(value));
}

StmtPtr Parser::ExprStatement() {
  ExprPtr expr = Expression();
  Consume(TokenType::SEMICOLON, "Expect ';' after expression.");
  return std::make_unique<ExprStmt>(std::move(expr));
}

StmtPtr Parser::BreakStatement() {
  Consume(TokenType::SEMICOLON, "Expect ';' after 'break'.");
  return std::make_unique<BreakStmt>();
}

StmtPtr Parser::Declaration() {
  try {
    if (Match({TokenType::FUN})) return FuncDeclaration("function");
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

StmtPtr Parser::FuncDeclaration(std::string kind) {
  Token name = Consume(TokenType::IDENTIFIER, "Expect " + kind + " name.");
  Consume(TokenType::LEFT_PAREN, "Expect '(' after " + kind + " name.");
  std::vector<Token> parameters;
  if (!Check(TokenType::RIGHT_PAREN)) {
    do {
      if (parameters.size() >= 255) {
        Error(Peek(), "Can't have more than 255 parameters.");
      }
      parameters.push_back(
          Consume(TokenType::IDENTIFIER, "Expect parameter name."));
    } while (Match({TokenType::COMMA}));
  }
  Consume(TokenType::RIGHT_PAREN, "Expect ')' after parameters.");
  Consume(TokenType::LEFT_BRACE, "Expect '{' before " + kind + " body.");
  std::vector<StmtPtr> body = Block();
  return std::make_unique<FunctionStmt>(std::move(name), std::move(parameters),
                                        std::move(body));
}

StmtPtr Parser::IfStatement() {
  Consume(TokenType::LEFT_PAREN, "Expect '(' after 'if'.");
  ExprPtr condition = Expression();
  Consume(TokenType::RIGHT_PAREN, "Expect ')' after condition.");
  StmtPtr then_branch = Statement();
  StmtPtr else_branch = nullptr;
  if (Match({TokenType::ELSE})) else_branch = Statement();
  return std::make_unique<IfStmt>(std::move(condition), std::move(then_branch),
                                  std::move(else_branch));
}

StmtPtr Parser::WhileStatement() {
  Consume(TokenType::LEFT_PAREN, "Expect '(' after 'while'.");
  ExprPtr condition = Expression();
  Consume(TokenType::RIGHT_PAREN, "Expect ')' after condition.");
  StmtPtr body = Statement();
  return std::make_unique<WhileStmt>(std::move(condition), std::move(body));
}

StmtPtr Parser::ForStatement() {
  Consume(TokenType::LEFT_PAREN, "Expect '(' after 'for'.");
  StmtPtr initializer = nullptr;
  if (Match({TokenType::SEMICOLON})) {
    initializer = nullptr;
  } else if (Match({TokenType::VAR})) {
    initializer = VarDeclaration();
  } else {
    initializer = Statement();
  }

  ExprPtr condition = nullptr;
  if (!Check(TokenType::SEMICOLON)) {
    condition = Expression();
  }
  Consume(TokenType::SEMICOLON, "Expect ';' after loop condition.");

  ExprPtr increment = nullptr;
  if (!Check(TokenType::RIGHT_PAREN)) {
    increment = Expression();
  }
  Consume(TokenType::RIGHT_PAREN, "Expect ')' after for clauses.");

  StmtPtr body = Statement();
  if (increment != nullptr) {
    std::vector<StmtPtr> block;
    block.push_back(std::move(body));
    block.push_back(std::make_unique<ExprStmt>(std::move(increment)));
    body = std::make_unique<BlockStmt>(std::move(block));
  }
  if (condition == nullptr) {
    condition = std::make_unique<LiteralExpr>(LoxObject(true));
  }
  body = std::make_unique<WhileStmt>(std::move(condition), std::move(body));
  if (initializer != nullptr) {
    std::vector<StmtPtr> block;
    block.push_back(std::move(initializer));
    block.push_back(std::move(body));
    body = std::make_unique<BlockStmt>(std::move(block));
  }
  return body;
}
}  // namespace lox