# Lox 字节码解释器库和主程序

# 设置可执行文件名称
set(EXECUTABLE_NAME lox_bytecode)

# 自动收集源文件和头文件
file(GLOB_RECURSE LOX_BYTECODE_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cc"
)

file(GLOB_RECURSE LOX_BYTECODE_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

# 显示收集到的文件
list(LENGTH LOX_BYTECODE_SOURCES SOURCE_COUNT)
message(STATUS "[LoxBytecode] Found ${SOURCE_COUNT} source files")

# 创建可执行文件
add_executable(${EXECUTABLE_NAME} ${LOX_BYTECODE_SOURCES} ${LOX_BYTECODE_HEADERS})

# 设置头文件目录
target_include_directories(${EXECUTABLE_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}
)

# 编译器优化和警告
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        target_compile_options(${EXECUTABLE_NAME} PRIVATE -O3 -DNDEBUG)
    else()
        target_compile_options(${EXECUTABLE_NAME} PRIVATE -O0 -g -Wall -Wextra -Wpedantic)
    endif()
endif()

# 安装规则
install(TARGETS ${EXECUTABLE_NAME} RUNTIME DESTINATION bin)

message(STATUS "[LoxBytecode] Executable '${EXECUTABLE_NAME}' configured")
