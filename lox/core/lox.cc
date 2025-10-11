#include <fstream>
#include <iostream>
#include <string>
#include <iterator>
#include <cstdlib>
#include <vector>

#include "lox/core/lox.h"
#include "lox/util/token_type.h"
#include "lox/core/token.h"
#include "lox/core/scanner.h"
#include "lox/core/parser.h"
#include "lox/ast/visitors/printer.h"

namespace lox {

void Lox::RunFile(const std::string& path) {
  std::ifstream file(path);
  std::string content((std::istreambuf_iterator<char>(file)),
                      std::istreambuf_iterator<char>());
  run(content);
  if (has_error_) {
    exit(65);
  }
}

void Lox::RunPrompt() {
  std::string line;
  while (std::getline(std::cin, line)) {
    std::cout << "> ";
    run(line);
    has_error_ = false;
  }
}

void Lox::run(const std::string& source) {
  Scanner scanner(source);
  std::vector<Token> tokens = scanner.ScanTokens();
  Parser parser(tokens);
  ExpressionPtr expression = parser.ParseExpression();
  
  if (has_error_) {
    return;
  }

  Printer printer;
  std::cout << printer.print(*expression) << std::endl;
}

void Lox::error(int line, const std::string& message) {
  report(line, "", message);
}

void Lox::error(Token token, const std::string& message) {
  if (token.type() == TokenType::EEOF) {
    report(token.line(), " at end", message);
  } else {
    report(token.line(), " at '" + token.lexeme() + "'", message);
  }
}

void Lox::report(int line, const std::string& where,
                 const std::string& message) {
  std::cout << "[line " << line << "] Error" << where << ": " << message
            << std::endl;
  has_error_ = true;
}

}  // namespace lox