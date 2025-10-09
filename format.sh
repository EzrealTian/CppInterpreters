#!/bin/bash

# clang-format 批量格式化脚本

echo "🎨 开始格式化代码..."

# 格式化lox目录下的所有.cc和.h文件
find lox -type f \( -name "*.cc" -o -name "*.h" \) -exec clang-format -i {} +

echo "✅ 格式化完成！"

# 显示格式化的文件数量
echo ""
echo "格式化的文件: $(find lox -type f \( -name "*.cc" -o -name "*.h" \) | wc -l) 个文件"
