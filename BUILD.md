# 构建指南

## 快速开始

```bash
# 1. 创建构建目录
mkdir build
cd build

# 2. 配置
cmake ..

# 3. 编译
make

# 4. 运行
./lox/cpplox              # 主程序
./test/loxtest --all      # 测试程序
```

## 构建选项

### Release模式（默认）
```bash
cmake ..
# 或明确指定
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```

### Debug模式
```bash
cmake -DCMAKE_BUILD_TYPE=Debug ..
make
```

### 不构建测试
```bash
cmake -DBUILD_TESTS=OFF ..
make
```

## 构建特定目标

```bash
# 只构建主程序
make cpplox

# 只构建测试
make loxtest

# 查看所有可用目标
make help
```

## 清理构建

```bash
# 清理编译产物（保留CMake配置）
make clean

# 完全清理（需要重新cmake）
cd build
rm -rf *
cmake ..
```

## 构建输出

构建成功后，可执行文件位于：
```
build/
├── lox/
│   └── cpplox          # Lox解释器
└── test/
    └── loxtest         # 测试程序
```

### 创建便捷链接
```bash
cd build
ln -sf lox/cpplox cpplox
ln -sf test/loxtest loxtest

# 现在可以直接运行
./cpplox
./loxtest --all
```

## 常见问题

### 找不到头文件
```bash
# 确保从build目录运行cmake
cd build
cmake ..
```

### 需要重新扫描文件
```bash
# 添加新文件后
cd build
cmake ..
make
```

### 编译错误
```bash
# 查看详细编译信息
make VERBOSE=1
```

## 开发工作流

### 1. 编码
```bash
# 编辑代码...
vim lox/src/scanner.cc
```

### 2. 编译
```bash
cd build
make
```

### 3. 测试
```bash
./test/loxtest --scanner
```

### 4. 运行
```bash
./lox/cpplox test.lox
```

## 推荐别名

在`~/.bashrc`或`~/.zshrc`中添加：

```bash
# Lox开发别名
alias lox-build='cd build && cmake .. && make'
alias lox-clean='cd build && rm -rf * && cmake ..'
alias lox-test='cd build && ./test/loxtest --all'
alias lox-run='cd build && ./lox/cpplox'
```

使用：
```bash
lox-build        # 快速构建
lox-test         # 运行测试
lox-run test.lox # 运行文件
```
