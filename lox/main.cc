#include <iostream>
#include <cstdlib>
#include "src/lox.h"

int main(int argc, char const *argv[]) {
  if (argc > 2) {
    std::cout << "Usage: cpplox [script] [--demo]" << std::endl;
    exit(64);
  } else if (argc == 2) {
    lox::Lox::Instance().RunFile(argv[1]);
  } else {
    lox::Lox::Instance().RunPrompt();
  }
  return 0;
}