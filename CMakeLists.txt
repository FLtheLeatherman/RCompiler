cmake_minimum_required(VERSION 3.16)
project(RCompiler)
set(CMAKE_CXX_STANDARD 20)

include_directories(include)

add_executable(code
        src/lexer/lexer.cpp
        src/parser/visitor.cpp
        src/parser/astnode.cpp
        src/parser/parser.cpp
        src/parser/astprinter.cpp
        src/main.cpp
)