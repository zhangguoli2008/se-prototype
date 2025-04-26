cmake_minimum_required(VERSION 3.10)
project(se_prototype)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_executable(se-prototype src/main.cpp src/client.cpp src/server.cpp)

find_package(OpenSSL REQUIRED)
target_link_libraries(se-prototype OpenSSL::Crypto)
