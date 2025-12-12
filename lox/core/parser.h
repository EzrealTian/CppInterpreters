#ifndef LOX_CORE_PARSER_H_
#define LOX_CORE_PARSER_H_

#include <vector>
#include <initializer_list>
#include <functional>
#include <stdexcept>

#include "lox/core/token.h"
#include "lox/ast/expr.h"
#include "lox/ast/stmt.h"
#include "lox/util/token_type.h"

namespace lox {

class Parser {
 public:
  // Parser error exception class
  class ParseError : public std::runtime_error {
   public:
    explicit ParseError(const std::string& message)
        : std::runtime_error(message) {}
  };

  Parser(const std::vector<Token>& tokens) : tokens_(std::move(tokens)) {}

  std::vector<StmtPtr> Parse();

 private:
  // Helper functions
  bool Match(std::initializer_list<TokenType> types);
  bool Check(TokenType type);
  Token Advance();
  bool IsAtEnd();
  Token Previous();
  Token Peek();
  Token Consume(TokenType type, const std::string& message);
  ParseError Error(Token token, const std::string& message);
  void Synchronize();

  // Grammar parsing functions
  ExprPtr Expression();
  ExprPtr ParseEquality();    // equality  →
                              // comparison ( ( "!=" | "==" ) comparison )* ;
  ExprPtr ParseComparison();  // comparison  →  term ( ( ">" | ">=" | "<"
                              // | "<=" ) term )* ;
  ExprPtr ParseTerm();        // term  →  factor ( ( "-" | "+" ) factor )* ;
  ExprPtr ParseFactor();      // factor  →  unary ( ( "/" | "*" ) unary )* ;
  ExprPtr ParseBinary(std::function<ExprPtr()> next,
                      std::initializer_list<TokenType> types);
  ExprPtr ParseUnary();       // unary  →  ( "!" | "-" ) unary | primary ;
  ExprPtr ParsePrimary();     // primary  →  NUMBER | STRING | "true" |
                              // "false" | "nil" | "(" expression ")" ;
  ExprPtr ParseAssignment();  // assignment  →  ( call "." )? IDENTIFIER "="
                              // assignment | call ;
  ExprPtr ParseOr();
  ExprPtr ParseAnd();

  std::vector<StmtPtr> Block();
  StmtPtr Statement();
  StmtPtr PrintStatement();
  StmtPtr ExprStatement();

  StmtPtr Declaration();
  StmtPtr VarDeclaration();
  StmtPtr IfStatement();
  StmtPtr WhileStatement();
  StmtPtr ForStatement();
 private:
  std::vector<Token> tokens_;
  int current_ = 0;
};

}  // namespace lox

#endif  // LOX_CORE_PARSER_H_