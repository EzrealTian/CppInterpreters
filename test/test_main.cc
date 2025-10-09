#include <iostream>
#include <string>

// 前向声明测试函数
namespace lox {
namespace test {
void testScanner();
void testTokenType();
// 未来可以添加更多测试
// void testParser();
// void testInterpreter();
}  // namespace test
}  // namespace lox

void printUsage(const char* program) {
    std::cout << "用法: " << program << " [选项]\n\n";
    std::cout << "选项:\n";
    std::cout << "  --all           运行所有测试\n";
    std::cout << "  --scanner       测试Scanner（词法分析器）\n";
    std::cout << "  --token-type    测试TokenType转换\n";
    // std::cout << "  --parser        测试Parser（语法分析器）\n";
    // std::cout << "  --interpreter   测试Interpreter（解释器）\n";
    std::cout << "  --help, -h      显示帮助信息\n";
    std::cout << "\n示例:\n";
    std::cout << "  " << program << " --all\n";
    std::cout << "  " << program << " --scanner\n";
    std::cout << "  " << program << " --scanner --token-type\n";
}

int main(int argc, char* argv[]) {
    if (argc == 1) {
        std::cout << "❌ 错误: 需要指定测试选项\n\n";
        printUsage(argv[0]);
        return 1;
    }

    bool runAll = false;
    bool runScanner = false;
    bool runTokenType = false;

    // 解析命令行参数
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            printUsage(argv[0]);
            return 0;
        } else if (arg == "--all") {
            runAll = true;
        } else if (arg == "--scanner") {
            runScanner = true;
        } else if (arg == "--token-type") {
            runTokenType = true;
        } else {
            std::cout << "❌ 未知选项: " << arg << "\n\n";
            printUsage(argv[0]);
            return 1;
        }
    }

    // 如果指定了--all，运行所有测试
    if (runAll) {
        runScanner = true;
        runTokenType = true;
    }

    std::cout << "🧪 Lox 测试套件\n";
    std::cout << "===============\n\n";

    int testCount = 0;
    int passedCount = 0;

    // 运行Scanner测试
    if (runScanner) {
        testCount++;
        std::cout << "▶️  运行 Scanner 测试...\n";
        std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
        try {
            lox::test::testScanner();
            std::cout << "✅ Scanner 测试通过\n\n";
            passedCount++;
        } catch (const std::exception& e) {
            std::cout << "❌ Scanner 测试失败: " << e.what() << "\n\n";
        }
    }

    // 运行TokenType测试
    if (runTokenType) {
        testCount++;
        std::cout << "▶️  运行 TokenType 测试...\n";
        std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
        try {
            lox::test::testTokenType();
            std::cout << "✅ TokenType 测试通过\n\n";
            passedCount++;
        } catch (const std::exception& e) {
            std::cout << "❌ TokenType 测试失败: " << e.what() << "\n\n";
        }
    }

    // 总结
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    std::cout << "测试总结: " << passedCount << "/" << testCount << " 通过\n";
    
    if (passedCount == testCount) {
        std::cout << "🎉 所有测试通过！\n";
        return 0;
    } else {
        std::cout << "😞 有 " << (testCount - passedCount) << " 个测试失败\n";
        return 1;
    }
}
