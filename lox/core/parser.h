#ifndef LOX_CORE_PARSER_H_
#define LOX_CORE_PARSER_H_

#include <vector>
#include <initializer_list>
#include <functional>
#include <stdexcept>
#include "lox/core/token.h"
#include "lox/ast/expression.h"
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

  ExpressionPtr ParseExpression();

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
  ExpressionPtr ParseEquality();     // equality  →  comparison ( ( "!=" | "==" ) comparison )* ;
  ExpressionPtr ParseComparison();   // comparison  →  term ( ( ">" | ">=" | "<" | "<=" ) term )* ;
  ExpressionPtr ParseTerm();         // term  →  factor ( ( "-" | "+" ) factor )* ;
  ExpressionPtr ParseFactor();       // factor  →  unary ( ( "/" | "*" ) unary )* ;
  ExpressionPtr ParseBinary(std::function<ExpressionPtr()> next, std::initializer_list<TokenType> types);
  ExpressionPtr ParseUnary();        // unary  →  ( "!" | "-" ) unary | primary ;
  ExpressionPtr ParsePrimary();      // primary  →  NUMBER | STRING | "true" | "false" | "nil" | "(" expression ")" ;

private:
  std::vector<Token> tokens_;
  int current_ = 0;
};

}  // namespace lox

#endif  // LOX_CORE_PARSER_H_