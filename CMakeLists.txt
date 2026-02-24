cmake_minimum_required(VERSION 3.16)
project(RCompiler)
set(CMAKE_CXX_STANDARD 20)

add_compile_options(-O3)
add_compile_options(-Ofast)

find_package(Boost 1.83.0 REQUIRED COMPONENTS regex)

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
        src/semantic/const_evaluator.cpp
        src/semantic/struct_checker.cpp
        src/semantic/type_checker.cpp
        src/ir/type.cpp
        src/ir/value.cpp
        src/ir/ir_builder.cpp
        src/ir/pre_generator.cpp
        src/ir/ir_generator.cpp
        src/main.cpp
)

# Test runner executable
# add_executable(run_test1
#         src/lexer/lexer.cpp
#         src/parser/visitor.cpp
#         src/parser/astnode.cpp
#         src/parser/parser.cpp
#         src/parser/astprinter.cpp
#         src/semantic/const_value.cpp
#         src/semantic/symbol.cpp
#         src/semantic/scope.cpp
#         src/semantic/symbol_collector.cpp
#         src/semantic/const_evaluator.cpp
#         src/semantic/struct_checker.cpp
#         src/semantic/type_checker.cpp
#         test/run_test1.cpp
# )
# 
# add_executable(run_test2
#         src/lexer/lexer.cpp
#         src/parser/visitor.cpp
#         src/parser/astnode.cpp
#         src/parser/parser.cpp
#         src/parser/astprinter.cpp
#         src/semantic/const_value.cpp
#         src/semantic/symbol.cpp
#         src/semantic/scope.cpp
#         src/semantic/symbol_collector.cpp
#         src/semantic/const_evaluator.cpp
#         src/semantic/struct_checker.cpp
#         src/semantic/type_checker.cpp
#         test/run_test2.cpp
# )

add_executable(ir_test
        src/lexer/lexer.cpp
        src/parser/visitor.cpp
        src/parser/astnode.cpp
        src/parser/parser.cpp
        src/parser/astprinter.cpp
        src/semantic/const_value.cpp
        src/semantic/symbol.cpp
        src/semantic/scope.cpp
        src/semantic/symbol_collector.cpp
        src/semantic/const_evaluator.cpp
        src/semantic/struct_checker.cpp
        src/semantic/type_checker.cpp
        src/ir/type.cpp
        src/ir/value.cpp
        src/ir/ir_builder.cpp
        src/ir/pre_generator.cpp
        src/ir/ir_generator.cpp
        test/ir_test/ir_test.cpp
)