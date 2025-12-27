#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include "parser/astprinter.hpp"
#include "semantic/symbol.hpp"
#include "semantic/scope.hpp"
#include "semantic/symbol_collector.hpp"
#include "semantic/const_evaluator.hpp"
#include "semantic/struct_checker.hpp"
#include "semantic/type_checker.hpp"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <test_name>" << std::endl;
        return 1;
    }
    
    std::string test_name = argv[1];
    std::string test_file_path = "../.RCompiler-Testcases/semantic-2/src/" + test_name + "/" + test_name + ".rx";
    
    // 检查文件是否存在
    if (!std::filesystem::exists(test_file_path)) {
        std::cerr << "Error: Test file not found: " << test_file_path << std::endl;
        return 1;
    }
    
    // 读取测试文件内容
    std::ifstream test_file(test_file_path);
    if (!test_file.is_open()) {
        std::cerr << "Error: Cannot open test file: " << test_file_path << std::endl;
        return 1;
    }
    
    std::string code;
    std::string line;
    while (std::getline(test_file, line)) {
        code += line + "\n";
    }
    test_file.close();
    
    std::cout << "Running test: " << test_name << std::endl;
    std::cout << "Test file: " << test_file_path << std::endl;
    std::cout << "========================================" << std::endl;
    
    try {
        // 词法分析
        Lexer lexer;
        auto tokens = lexer.lex(code);
        std::cout << "Tokens: " << tokens.size() << std::endl;
        
        // 语法分析
        Parser parser(std::move(tokens));
        auto root = parser.parseCrate();
        
        // AST 打印
        ASTPrinter printer(std::cout, false);
        printer.set_indent_level(0);
        printer.visit(*root);
        
        // 符号收集
        SymbolCollector symbol_collector;
        symbol_collector.visit(*root);
        auto root_scope = symbol_collector.getRootScope();
        
        // 添加内建函数（与 main.cpp 相同的逻辑）
        auto s_var_symbol = std::make_shared<VariableSymbol>("s", "&str", false, false);
        auto print_symbol = std::make_shared<FuncSymbol>("print", "()", false, MethodType::NOT_METHOD);
        auto println_symbol = std::make_shared<FuncSymbol>("println", "()", false, MethodType::NOT_METHOD);
        print_symbol->addParameter(s_var_symbol), println_symbol->addParameter(s_var_symbol);
        auto n_int_symbol = std::make_shared<VariableSymbol>("n", "i32", false, false);
        auto print_int_symbol = std::make_shared<FuncSymbol>("printInt", "()", false, MethodType::NOT_METHOD);
        auto println_int_symbol = std::make_shared<FuncSymbol>("printlnInt", "()", false, MethodType::NOT_METHOD);
        print_int_symbol->addParameter(n_int_symbol), println_int_symbol->addParameter(n_int_symbol);
        root_scope->addFuncSymbol("print", print_symbol);
        root_scope->addFuncSymbol("println", println_symbol);
        root_scope->addFuncSymbol("printInt", print_int_symbol);
        root_scope->addFuncSymbol("printlnInt", println_int_symbol);
        
        // 添加全局内建函数
        auto get_string_symbol = std::make_shared<FuncSymbol>("getString", "String", false, MethodType::NOT_METHOD);
        auto get_int_symbol = std::make_shared<FuncSymbol>("getInt", "i32", false, MethodType::NOT_METHOD);
        auto code_param_symbol = std::make_shared<VariableSymbol>("code", "i32", false, false);
        auto exit_symbol = std::make_shared<FuncSymbol>("exit", "()", false, MethodType::NOT_METHOD);
        exit_symbol->addParameter(code_param_symbol);
        
        root_scope->addFuncSymbol("getString", get_string_symbol);
        root_scope->addFuncSymbol("getInt", get_int_symbol);
        root_scope->addFuncSymbol("exit", exit_symbol);
        
        // 创建 u32 结构体并添加 to_string 方法
        auto u32_struct = std::make_shared<StructSymbol>("u32", "u32");
        auto u32_to_string_method = std::make_shared<FuncSymbol>("to_string", "String", false, MethodType::SELF_REF);
        u32_struct->addMethod(u32_to_string_method);
        
        // 创建 usize 结构体并添加 to_string 方法
        auto usize_struct = std::make_shared<StructSymbol>("usize", "usize");
        auto usize_to_string_method = std::make_shared<FuncSymbol>("to_string", "String", false, MethodType::SELF_REF);
        usize_struct->addMethod(usize_to_string_method);
        
        // 创建 String 结构体并添加方法
        auto string_struct = std::make_shared<StructSymbol>("String", "String");
        auto string_as_str_method = std::make_shared<FuncSymbol>("as_str", "&str", false, MethodType::SELF_REF);
        auto string_len_method = std::make_shared<FuncSymbol>("len", "u32", false, MethodType::SELF_REF);
        string_struct->addMethod(string_as_str_method);
        string_struct->addMethod(string_len_method);
        
        // 创建 &str 结构体并添加 len 方法
        auto str_struct = std::make_shared<StructSymbol>("str", "str");
        auto str_len_method = std::make_shared<FuncSymbol>("len", "u32", false, MethodType::SELF_REF);
        str_struct->addMethod(str_len_method);
        
        // 将结构体添加到根作用域
        root_scope->addStructSymbol("u32", u32_struct);
        root_scope->addStructSymbol("usize", usize_struct);
        root_scope->addStructSymbol("String", string_struct);
        root_scope->addStructSymbol("str", str_struct);
        
        // 常量求值
        ConstEvaluator const_evaluator(root_scope);
        const_evaluator.visit(*root);
        root_scope->clearPos();
        
        // 结构体检查
        StructChecker struct_checker(root_scope);
        struct_checker.visit(*root);
        root_scope->clearPos();
        
        std::cout << "\n========================================" << std::endl;
        std::cout << "After Struct Checker:" << std::endl;
        root_scope->printScope();
        std::cout << "========================================" << std::endl;
        
        // 类型检查
        root_scope->clearPos();
        TypeChecker type_checker(root_scope);
        type_checker.visit(*root);
        root_scope->clearPos();
        
        std::cout << "\n========================================" << std::endl;
        std::cout << "After Type Checker:" << std::endl;
        root_scope->printScope();
        std::cout << "========================================" << std::endl;
        
        std::cout << "Test completed successfully!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error during test execution: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}