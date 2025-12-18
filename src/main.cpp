#include <iostream>
#include "lexer/lexer.hpp"
#include "parser/parser.hpp"

int main() {
    // freopen("test.in", "r", stdin);
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
    ASTVisitor visitor;
    visitor.visit(*root);
}