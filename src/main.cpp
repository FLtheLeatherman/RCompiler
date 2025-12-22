#include <iostream>
#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include "parser/astprinter.hpp"

int main() {
    // freopen("test.in", "r", stdin);
    // freopen("test.out", "w", stdout);
    std::string code;
    char ch = getchar();
    while (ch != EOF) {
        code += ch;
        ch = getchar();
    }
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
    // ASTVisitor visitor;
    // visitor.visit(*root);
    ASTPrinter printer(std::cerr, true);
    printer.set_indent_level(0);
    printer.visit(*root);
}