#include <iostream>
#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include "parser/astprinter.hpp"
#include "semantic/symbol_collector.hpp"
#include "semantic/const_evaluator.hpp"
#include "semantic/struct_checker.hpp"
#include "semantic/type_checker.hpp"

int main() {
    freopen("test.in", "r", stdin);
    // freopen("test.out", "w", stdout);
    std::string code;
    char ch = getchar();
    while (ch != EOF) {
        code += ch;
        ch = getchar();
    }
    // std::cout << code << std::endl;
    Lexer lexer;
    auto tokens = lexer.lex(code);
    std::cout << tokens.size() << std::endl;
    int id = 0;
    for (auto token: tokens) {
        std::cout << id << ' ' << tokenToString(token.first) << ' ' << token.second << std::endl;
        id++;
    }
    std::cout.flush();
    Parser parser(std::move(tokens));
    auto root = parser.parseCrate();
    ASTPrinter printer(std::cout, true);
    printer.set_indent_level(0);
    printer.visit(*root);
    SymbolCollector symbol_collector;
    symbol_collector.visit(*root);
    auto root_scope = symbol_collector.getRootScope();
    // root_scope->printScope();
    ConstEvaluator const_evaluator(root_scope);
    const_evaluator.visit(*root);
    // root_scope->printScope();
    root_scope->clearPos();
    StructChecker struct_checker(root_scope);
    struct_checker.visit(*root);
    root_scope->printScope();
    root_scope->clearPos();
    TypeChecker type_checker(root_scope);
    type_checker.visit(*root);
    root_scope->printScope();
    root_scope->clearPos();
}