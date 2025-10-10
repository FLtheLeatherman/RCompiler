cmake_minimum_required(VERSION 3.16)
project(RCompiler)
set(CMAKE_CXX_STANDARD 20)

include_directories(include)

add_executable(code
        src/lexer.cpp
        src/visitor.cpp
        src/astnode.cpp
        src/parser.cpp
        src/main.cpp
)