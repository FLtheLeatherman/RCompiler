#include <iostream>
#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include "parser/astprinter.hpp"

int main() {
    freopen("test.in", "r", stdin);
    freopen("test.out", "w", stdout);
    std::string code;
    char ch = getchar();
    while (ch != EOF) {
        code += ch;
        ch = getchar();
    }
    Lexer lexer;
    auto tokens = lexer.lex(code);
    for (auto token: tokens) {
        std::cerr << (int)token.first << ' ' << token.second << std::endl;
    }
    Parser parser(std::move(tokens));
    auto root = parser.parseCrate();
    // ASTVisitor visitor;
    // visitor.visit(*root);
    ASTPrinter printer;
    printer.set_use_colors(true);
    printer.set_indent_level(0);
    printer.visit(*root);
}