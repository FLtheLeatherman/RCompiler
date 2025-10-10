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
    std::string get_string();
    void consume();

    std::unique_ptr<Crate> parseCrate();
    std::unique_ptr<Item> parseItem();

    std::unique_ptr<Function> parseFunction();
    std::unique_ptr<Struct> parseStruct();
    std::unique_ptr<Enumeration> parseEnumeration();
    std::unique_ptr<ConstantItem> parseConstantItem();
    std::unique_ptr<Trait> parseTrait();
    std::unique_ptr<Implementation> parseImplementation();

    std::unique_ptr<FunctionParameters> parseFunctionParameters();
    std::unique_ptr<FunctionReturnType> parseFunctionReturnType();
    std::unique_ptr<BlockExpression> parseBlockExpression();

    std::unique_ptr<StructStruct> parseStructStruct();
    std::unique_ptr<StructFields> parseStructFields(); 
};