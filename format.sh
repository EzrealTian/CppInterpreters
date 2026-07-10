#!/bin/bash

# clang-format 批量格式化脚本

echo "🎨 开始格式化代码..."

# 格式化lox_interpreter和lox_bytecode目录下的所有.cc和.h文件
find lox_interpreter -type f \( -name "*.cc" -o -name "*.h" \) -exec clang-format -i {} +
find lox_bytecode -type f \( -name "*.cc" -o -name "*.h" \) -exec clang-format -i {} +

echo "✅ 格式化完成！"

# 显示格式化的文件数量
echo ""
echo "格式化的文件: $(find lox_interpreter -type f \( -name "*.cc" -o -name "*.h" \) | wc -l) 个文件"
echo "格式化的文件: $(find lox_bytecode -type f \( -name "*.cc" -o -name "*.h" \) | wc -l) 个文件"
