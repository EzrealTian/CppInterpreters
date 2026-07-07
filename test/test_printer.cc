#include <iostream>
#include <memory>
#include "lox_interpreter/ast/expr.h"
#include "lox_interpreter/ast/visitors/printer.h"
#include "lox_interpreter/core/token.h"
#include "lox_interpreter/util/token_type.h"

namespace lox {
namespace test {

void testPrinter() {
  std::cout << "\n🎨 表达式打印器测试\n";
  std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n";

  Printer printer;

  // 测试 1: Literal 字面量
  std::cout << "测试 1: 字面量\n";
  auto literal = std::make_unique<LiteralExpr>(LoxObject(123.1));
  std::cout << "  表达式: 123.1\n";
  std::cout << "  输出: " << printer.Print(*literal) << "\n\n";

  // 测试 2: Unary 一元表达式: -123
  std::cout << "测试 2: 一元表达式\n";
  Token minus(TokenType::MINUS, "-", nullptr, 1);
  auto unary = std::make_unique<UnaryExpr>(
      minus,
      std::make_unique<LiteralExpr>(LoxObject(123.0))
  );
  std::cout << "  表达式: -123\n";
  std::cout << "  输出: " << printer.Print(*unary) << "\n\n";

  // 测试 3: Binary 二元表达式: 1 + 2
  std::cout << "测试 3: 二元表达式\n";
  Token plus(TokenType::PLUS, "+", nullptr, 1);
  auto binary = std::make_unique<BinaryExpr>(
      std::make_unique<LiteralExpr>(LoxObject(1.0)),
      plus,
      std::make_unique<LiteralExpr>(LoxObject(2.0))
  );
  std::cout << "  表达式: 1 + 2\n";
  std::cout << "  输出: " << printer.Print(*binary) << "\n\n";

  // 测试 4: 复杂表达式: (1 + 2) * (4 - 3)
  std::cout << "测试 4: 复杂表达式\n";
  Token star(TokenType::STAR, "*", nullptr, 1);
  Token minus2(TokenType::MINUS, "-", nullptr, 1);
  auto complex = std::make_unique<BinaryExpr>(
      std::make_unique<GroupingExpr>(
          std::make_unique<BinaryExpr>(
              std::make_unique<LiteralExpr>(LoxObject(1.0)),
              plus,
              std::make_unique<LiteralExpr>(LoxObject(2.0))
          )
      ),
      star,
      std::make_unique<GroupingExpr>(
          std::make_unique<BinaryExpr>(
              std::make_unique<LiteralExpr>(LoxObject(4.0)),
              minus2,
              std::make_unique<LiteralExpr>(LoxObject(3.0))
          )
      )
  );
  std::cout << "  表达式: (1 + 2) * (4 - 3)\n";
  std::cout << "  输出: " << printer.Print(*complex) << "\n\n";

  // 测试 5: 嵌套一元表达式: -(-5)
  std::cout << "测试 5: 嵌套一元表达式\n";
  Token minus3(TokenType::MINUS, "-", nullptr, 1);
  Token minus4(TokenType::MINUS, "-", nullptr, 1);
  auto nested = std::make_unique<UnaryExpr>(
      minus3,
      std::make_unique<UnaryExpr>(
          minus4,
          std::make_unique<LiteralExpr>(LoxObject(5.0))
      )
  );
  std::cout << "  表达式: -(-5)\n";
  std::cout << "  输出: " << printer.Print(*nested) << "\n\n";

  std::cout << "✅ 表达式打印器测试完成！\n";
}

}  // namespace test
}  // namespace lox

