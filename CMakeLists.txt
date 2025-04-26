cmake_minimum_required(VERSION 3.10)
project(se_prototype LANGUAGES CXX)

# 使用 C++17 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 指定源码文件
add_executable(se-prototype src/main.cpp "sse_index.c" "sse_search.c")