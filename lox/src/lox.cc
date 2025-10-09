#include <fstream>
#include <iostream>
#include <string>
#include <iterator>
#include <cstdlib>

#include "lox/src/lox.h"

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

void Lox::run(const std::string& source) { std::cout << source << std::endl; }

void Lox::error(int line, const std::string& message) {
  report(line, "", message);
}

void Lox::report(int line, const std::string& where,
                 const std::string& message) {
  std::cout << "[line " << line << "] Error" << where << ": " << message
            << std::endl;
  has_error_ = true;
}

}  // namespace lox