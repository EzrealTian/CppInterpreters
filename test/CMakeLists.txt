# Lox 测试程序

# 设置测试可执行文件名称
set(TEST_EXECUTABLE_NAME loxtest)

# 收集测试源文件
file(GLOB TEST_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cc"
)

# 收集lox库源文件（排除main.cc）
file(GLOB_RECURSE LOX_LIB_SOURCES 
    "${CMAKE_SOURCE_DIR}/lox/core/*.cc"
    "${CMAKE_SOURCE_DIR}/lox/util/*.cc"
    "${CMAKE_SOURCE_DIR}/lox/ast/*.cc"
)

file(GLOB_RECURSE LOX_HEADERS 
    "${CMAKE_SOURCE_DIR}/lox/*.h"
)

# 显示信息
list(LENGTH TEST_SOURCES TEST_COUNT)
message(STATUS "[Test] Found ${TEST_COUNT} test files")

# 创建测试可执行文件
add_executable(${TEST_EXECUTABLE_NAME} 
    ${TEST_SOURCES} 
    ${LOX_LIB_SOURCES}
    ${LOX_HEADERS}
)

# 设置头文件目录
target_include_directories(${TEST_EXECUTABLE_NAME} PRIVATE 
    ${CMAKE_SOURCE_DIR}  # 项目根目录，这样可以用 lox/core/xxx.h
)

# 编译器选项
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        target_compile_options(${TEST_EXECUTABLE_NAME} PRIVATE -O3 -DNDEBUG)
    else()
        target_compile_options(${TEST_EXECUTABLE_NAME} PRIVATE -O0 -g -Wall -Wextra -Wpedantic)
    endif()
endif()

# 安装测试可执行文件
install(TARGETS ${TEST_EXECUTABLE_NAME} RUNTIME DESTINATION bin)

message(STATUS "[Test] Test executable '${TEST_EXECUTABLE_NAME}' configured")
