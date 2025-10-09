#include <iostream>
#include <string>
#include "lox/util/token_type.h"

namespace lox {
namespace test {

void testTokenType() {
    std::cout << "\n1. TokenType转字符串测试\n";
    
    struct TestCase {
        TokenType type;
        std::string expected;
    };
    
    TestCase cases[] = {
        {TokenType::LEFT_PAREN, "LEFT_PAREN"},
        {TokenType::RIGHT_PAREN, "RIGHT_PAREN"},
        {TokenType::PLUS, "PLUS"},
        {TokenType::MINUS, "MINUS"},
        {TokenType::STAR, "STAR"},
        {TokenType::SLASH, "SLASH"},
        {TokenType::EQUAL, "EQUAL"},
        {TokenType::EQUAL_EQUAL, "EQUAL_EQUAL"},
        {TokenType::BANG, "BANG"},
        {TokenType::BANG_EQUAL, "BANG_EQUAL"},
        {TokenType::LESS, "LESS"},
        {TokenType::LESS_EQUAL, "LESS_EQUAL"},
        {TokenType::GREATER, "GREATER"},
        {TokenType::GREATER_EQUAL, "GREATER_EQUAL"},
        {TokenType::STRING, "STRING"},
        {TokenType::NUMBER, "NUMBER"},
        {TokenType::IDENTIFIER, "IDENTIFIER"},
        {TokenType::IF, "IF"},
        {TokenType::ELSE, "ELSE"},
        {TokenType::WHILE, "WHILE"},
        {TokenType::FOR, "FOR"},
        {TokenType::VAR, "VAR"},
        {TokenType::FUN, "FUN"},
        {TokenType::RETURN, "RETURN"},
        {TokenType::CLASS, "CLASS"},
        {TokenType::TRUE, "TRUE"},
        {TokenType::FALSE, "FALSE"},
        {TokenType::NIL, "NIL"},
        {TokenType::AND, "AND"},
        {TokenType::OR, "OR"},
        {TokenType::EEOF, "EEOF"},
    };
    
    int passed = 0;
    int total = sizeof(cases) / sizeof(cases[0]);
    
    for (const auto& tc : cases) {
        std::string result = tokenTypeToString(tc.type);
        if (result == tc.expected) {
            std::cout << "  ✓ " << tc.expected << "\n";
            passed++;
        } else {
            std::cout << "  ❌ " << tc.expected << " (得到: " << result << ")\n";
        }
    }
    
    std::cout << "\n2. 字符串转TokenType测试\n";
    
    struct KeywordTestCase {
        std::string keyword;
        TokenType expected;
    };
    
    KeywordTestCase kwCases[] = {
        {"and", TokenType::AND},
        {"or", TokenType::OR},
        {"if", TokenType::IF},
        {"else", TokenType::ELSE},
        {"while", TokenType::WHILE},
        {"for", TokenType::FOR},
        {"var", TokenType::VAR},
        {"fun", TokenType::FUN},
        {"return", TokenType::RETURN},
        {"class", TokenType::CLASS},
        {"true", TokenType::TRUE},
        {"false", TokenType::FALSE},
        {"nil", TokenType::NIL},
        {"print", TokenType::PRINT},
        {"super", TokenType::SUPER},
        {"this", TokenType::THIS},
    };
    
    int kwPassed = 0;
    int kwTotal = sizeof(kwCases) / sizeof(kwCases[0]);
    
    for (const auto& tc : kwCases) {
        TokenType result = stringToTokenType(tc.keyword);
        if (result == tc.expected) {
            std::cout << "  ✓ \"" << tc.keyword << "\" -> " 
                      << tokenTypeToString(tc.expected) << "\n";
            kwPassed++;
        } else {
            std::cout << "  ❌ \"" << tc.keyword << "\" (期望: " 
                      << tokenTypeToString(tc.expected) 
                      << ", 得到: " << tokenTypeToString(result) << ")\n";
        }
    }
    
    std::cout << "\n3. 非关键字测试\n";
    TokenType result = stringToTokenType("notKeyword");
    if (result == TokenType::IDENTIFIER) {
        std::cout << "  ✓ 非关键字正确返回 IDENTIFIER\n";
    } else {
        std::cout << "  ❌ 非关键字应返回 IDENTIFIER\n";
    }
    
    std::cout << "\n测试总结:\n";
    std::cout << "  TokenType->String: " << passed << "/" << total << " 通过\n";
    std::cout << "  String->TokenType: " << kwPassed << "/" << kwTotal << " 通过\n";
    
    if (passed == total && kwPassed == kwTotal) {
        std::cout << "\n✅ TokenType 所有测试完成！\n";
    } else {
        throw std::runtime_error("TokenType 测试失败");
    }
}

}  // namespace test
}  // namespace lox
