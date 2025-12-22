cmake_minimum_required(VERSION 3.16)
project(RCompiler)
set(CMAKE_CXX_STANDARD 20)

add_compile_options(-O3)
add_compile_options(-Ofast)

include_directories(include)

add_executable(code
        src/lexer/lexer.cpp
        src/parser/visitor.cpp
        src/parser/astnode.cpp
        src/parser/parser.cpp
        src/parser/astprinter.cpp
        src/semantic/const_value.cpp
        src/semantic/symbol.cpp
        src/semantic/scope.cpp
        src/semantic/symbol_collector.cpp
        src/semantic/struct_checker.cpp
        src/main.cpp
)

#add_executable(parser_visualize_test
#        src/lexer/lexer.cpp
#        src/parser/visitor.cpp
#        src/parser/astnode.cpp
#        src/parser/parser.cpp
#        src/parser/astprinter.cpp
#        test/parser_visualize_test.cpp
#)

#add_executable(parser_test
#       src/lexer/lexer.cpp
#       src/parser/visitor.cpp
#       src/parser/astnode.cpp
#       src/parser/parser.cpp
#       src/parser/astprinter.cpp
#       test/parser_test.cpp
#)