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
#include "lox/core/resolver.h"
#include "lox/ast/visitors/interpreter.h"

namespace lox {

void Lox::RunFile(const std::string& path) {
  std::ifstream file(path);
  std::string content((std::istreambuf_iterator<char>(file)),
                      std::istreambuf_iterator<char>());
  run(content);
  if (has_error_) {
    exit(65);
  }
  if (has_runtime_error_) {
    exit(70);
  }
}

void Lox::RunPrompt() {
  std::string line;
  std::cout << "> ";
  while (std::getline(std::cin, line)) {
    run(line);
    has_error_ = false;
    std::cout << "> ";
  }
}

void Lox::run(const std::string& source) {
  Scanner scanner(source);
  std::vector<Token> tokens = scanner.ScanTokens();
  Parser parser(tokens);
  std::vector<StmtPtr> statements = parser.Parse();
  if (has_error_) {
    return;
  }

  static Interpreter interpreter;

  Resolver resolver(interpreter);
  resolver.Resolve(statements);
  if (has_error_) {
    return;
  }

  interpreter.Interpret(std::move(statements));
}

void Lox::Error(int line, const std::string& message) {
  Report(line, "", message);
}

void Lox::Error(Token token, const std::string& message) {
  if (token.type() == TokenType::EEOF) {
    Report(token.line(), " at end", message);
  } else {
    Report(token.line(), " at '" + token.lexeme() + "'", message);
  }
}

void Lox::RuntimeError(const lox::RuntimeError& error) {
  std::cout << "[line " << error.token_.line() << "] Runtime Error" << ": "
            << error.what() << std::endl;
  has_runtime_error_ = true;
}

void Lox::Report(int line, const std::string& where,
                 const std::string& message) {
  std::cout << "[line " << line << "] Error" << where << ": " << message
            << std::endl;
  has_error_ = true;
}

}  // namespace lox