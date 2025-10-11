#include <iostream>
#include <string>
#include <vector>
#include <cassert>

#include "lox/core/scanner.h"
#include "lox/core/token.h"
#include "lox/util/token_type.h"

namespace lox {
namespace test {

void testCase(const std::string& name, const std::string& source, 
              const std::vector<TokenType>& expectedTypes) {
    std::cout << "  测试: " << name << "\n";
    
    Scanner scanner(source);
    std::vector<Token> tokens = scanner.ScanTokens();
    
    // 检查token数量（包括EEOF）
    if (tokens.size() != expectedTypes.size()) {
        std::cout << "    ❌ 失败: 期望 " << expectedTypes.size() 
                  << " 个token，实际得到 " << tokens.size() << " 个\n";
        return;
    }
    
    // 检查每个token的类型
    for (size_t i = 0; i < expectedTypes.size(); ++i) {
        if (tokens[i].ToString().find(tokenTypeToString(expectedTypes[i])) == std::string::npos) {
            std::cout << "    ❌ 失败: 位置 " << i << " 期望 " 
                      << tokenTypeToString(expectedTypes[i]) 
                      << "，实际得到 " << tokens[i].ToString() << "\n";
            return;
        }
    }
    
    std::cout << "    ✓ 通过\n";
}

void testScanner() {
    std::cout << "\n1. 单字符Token测试\n";
    testCase("括号", "()", {
        TokenType::LEFT_PAREN, TokenType::RIGHT_PAREN, TokenType::EEOF
    });
    
    testCase("大括号", "{}", {
        TokenType::LEFT_BRACE, TokenType::RIGHT_BRACE, TokenType::EEOF
    });
    
    testCase("运算符", "+-*/", {
        TokenType::PLUS, TokenType::MINUS, TokenType::STAR, 
        TokenType::SLASH, TokenType::EEOF
    });
    
    std::cout << "\n2. 双字符Token测试\n";
    testCase("等于", "= ==", {
        TokenType::EQUAL, TokenType::EQUAL_EQUAL, TokenType::EEOF
    });
    
    testCase("不等于", "! !=", {
        TokenType::BANG, TokenType::BANG_EQUAL, TokenType::EEOF
    });
    
    testCase("比较运算符", "< <= > >=", {
        TokenType::LESS, TokenType::LESS_EQUAL,
        TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::EEOF
    });
    
    std::cout << "\n3. 字符串测试\n";
    testCase("简单字符串", "\"hello\"", {
        TokenType::STRING, TokenType::EEOF
    });
    
    testCase("空字符串", "\"\"", {
        TokenType::STRING, TokenType::EEOF
    });
    
    std::cout << "\n4. 数字测试\n";
    testCase("整数", "123", {
        TokenType::NUMBER, TokenType::EEOF
    });
    
    testCase("小数", "3.14", {
        TokenType::NUMBER, TokenType::EEOF
    });
    
    testCase("多个数字", "1 2.5 3", {
        TokenType::NUMBER, TokenType::NUMBER, TokenType::NUMBER, TokenType::EEOF
    });
    
    std::cout << "\n5. 标识符和关键字测试\n";
    testCase("标识符", "foo bar", {
        TokenType::IDENTIFIER, TokenType::IDENTIFIER, TokenType::EEOF
    });
    
    testCase("关键字if", "if", {
        TokenType::IF, TokenType::EEOF
    });
    
    testCase("关键字while", "while", {
        TokenType::WHILE, TokenType::EEOF
    });
    
    testCase("关键字var", "var", {
        TokenType::VAR, TokenType::EEOF
    });
    
    testCase("混合关键字和标识符", "var x", {
        TokenType::VAR, TokenType::IDENTIFIER, TokenType::EEOF
    });
    
    std::cout << "\n6. 注释测试\n";
    testCase("单行注释", "// comment\n+", {
        TokenType::PLUS, TokenType::EEOF
    });
    
    testCase("简单多行注释", "/* comment */ +", {
        TokenType::PLUS, TokenType::EEOF
    });
    
    testCase("跨行多行注释", "/* line1\nline2\nline3 */ -", {
        TokenType::MINUS, TokenType::EEOF
    });
    
    testCase("嵌套多行注释", "/* outer /* inner */ outer */ *", {
        TokenType::STAR, TokenType::EEOF
    });
    
    testCase("多行注释中的代码", "var x /* = 10 */ = 20;", {
        TokenType::VAR, TokenType::IDENTIFIER, TokenType::EQUAL,
        TokenType::NUMBER, TokenType::SEMICOLON, TokenType::EEOF
    });
    
    testCase("多个多行注释", "/* comment1 */ + /* comment2 */ -", {
        TokenType::PLUS, TokenType::MINUS, TokenType::EEOF
    });
    
    std::cout << "\n7. 复杂表达式测试\n";
    testCase("简单表达式", "2 + 3", {
        TokenType::NUMBER, TokenType::PLUS, TokenType::NUMBER, TokenType::EEOF
    });
    
    testCase("变量声明", "var x = 10;", {
        TokenType::VAR, TokenType::IDENTIFIER, TokenType::EQUAL,
        TokenType::NUMBER, TokenType::SEMICOLON, TokenType::EEOF
    });
    
    testCase("函数调用", "print(x);", {
        TokenType::PRINT, TokenType::LEFT_PAREN, TokenType::IDENTIFIER,
        TokenType::RIGHT_PAREN, TokenType::SEMICOLON, TokenType::EEOF
    });
    
    testCase("条件语句", "if (x > 5) { }", {
        TokenType::IF, TokenType::LEFT_PAREN, TokenType::IDENTIFIER,
        TokenType::GREATER, TokenType::NUMBER, TokenType::RIGHT_PAREN,
        TokenType::LEFT_BRACE, TokenType::RIGHT_BRACE, TokenType::EEOF
    });
    
    std::cout << "\n8. 综合测试\n";
    std::string complexSource = R"(
var name = "Lox";
fun greet() {
    print name;
}
)";
    
    Scanner scanner(complexSource);
    std::vector<Token> tokens = scanner.ScanTokens();
    std::cout << "  测试: 完整程序\n";
    std::cout << "    ✓ 成功扫描 " << tokens.size() << " 个token\n";
    
    std::cout << "\n✅ Scanner 所有测试完成！\n";
}

}  // namespace test
}  // namespace lox
