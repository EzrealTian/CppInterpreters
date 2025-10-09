cmake_minimum_required(VERSION 3.10)

# ==================== 项目配置 ====================
project(MiniInterpreter
    VERSION 1.0
    DESCRIPTION "C++ edition of CraftInterpreter"
    LANGUAGES CXX
)

# 生成 compile_commands.json 文件（用于IDE智能提示）
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 默认构建类型
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose Release or Debug" FORCE)
endif()

# 显示构建信息
message(STATUS "========================================")
message(STATUS "Project: ${PROJECT_NAME} v${PROJECT_VERSION}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard: C++${CMAKE_CXX_STANDARD}")
message(STATUS "C++ Compiler: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "========================================")

# ==================== 子项目 ====================
# 添加 lox 主程序
add_subdirectory(lox)

# 添加测试程序（可选）
option(BUILD_TESTS "Build test programs" ON)
if(BUILD_TESTS)
    message(STATUS "Tests enabled")
    add_subdirectory(test)
else()
    message(STATUS "Tests disabled")
endif()

# ==================== 总结信息 ====================
message(STATUS "========================================")
message(STATUS "Configuration complete!")
message(STATUS "Targets:")
message(STATUS "  - cpplox (Lox interpreter)")
if(BUILD_TESTS)
    message(STATUS "  - loxtest (Test suite)")
endif()
message(STATUS "")
message(STATUS "Build commands:")
message(STATUS "  cmake --build .")
message(STATUS "  cmake --build . --target cpplox")
if(BUILD_TESTS)
    message(STATUS "  cmake --build . --target loxtest")
endif()
message(STATUS "========================================")