#pragma once

#include "lexer.hpp"
#include "astnode.hpp"

class Parser {
private:
    std::vector<std::pair<Token, std::string>> tokens;
    size_t pos = 0;
public:
    Parser(std::vector<std::pair<Token, std::string>>&& tokens)
        : tokens(std::move(tokens)) {}

    Token peek();
    void consume();

    std::unique_ptr<Crate> parseCrate();
};