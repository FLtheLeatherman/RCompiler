#include <iostream>
#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include "parser/astprinter.hpp"
#include "semantic/symbol.hpp"
#include "semantic/scope.hpp"
#include "semantic/symbol_collector.hpp"
#include "semantic/const_evaluator.hpp"
#include "semantic/struct_checker.hpp"
#include "semantic/type_checker.hpp"

int main() {
    freopen("test.in", "r", stdin);
    freopen("test.out", "w", stdout);
    
    std::string code;
    char ch = getchar();
    while (ch != EOF) {
        code += ch;
        ch = getchar();
    }
    // std::cout << code << std::endl;

    Lexer lexer;
    auto tokens = lexer.lex(code);
    // std::cout << tokens.size() << std::endl;
    // int id = 0;
    // for (auto token: tokens) {
    //     std::cout << id << ' ' << tokenToString(token.first) << ' ' << token.second << std::endl;
    //     id++;
    // }
    
    Parser parser(std::move(tokens));
    auto root = parser.parseCrate();
    ASTPrinter printer(std::cout, false);
    printer.set_indent_level(0);
    printer.visit(*root);

    SymbolCollector symbol_collector;
    symbol_collector.visit(*root);
    auto root_scope = symbol_collector.getRootScope();

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

    root_scope->printScope();

    ConstEvaluator const_evaluator(root_scope);
    const_evaluator.visit(*root);
    // root_scope->printScope();
    root_scope->clearPos();

    StructChecker struct_checker(root_scope);
    struct_checker.visit(*root);
    root_scope->printScope();
    root_scope->clearPos();

    // std::cout << u32_struct->hasMethod("to_string") << std::endl;

    TypeChecker type_checker(root_scope);
    type_checker.visit(*root);
    root_scope->printScope();
    root_scope->clearPos();
}