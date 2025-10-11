#pragma once

#include <memory>
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
    void match(Token);

    std::unique_ptr<Crate> parseCrate();
    std::unique_ptr<Item> parseItem();

    std::unique_ptr<Function> parseFunction();
    std::unique_ptr<Struct> parseStruct();
    std::unique_ptr<Enumeration> parseEnumeration();
    std::unique_ptr<ConstantItem> parseConstantItem();
    std::unique_ptr<Trait> parseTrait();
    std::unique_ptr<Implementation> parseImplementation();

    std::unique_ptr<FunctionParameters> parseFunctionParameters();
    std::unique_ptr<SelfParam> parseSelfParam();
    std::unique_ptr<ShorthandSelf> parseShorthandSelf();
    std::unique_ptr<TypedSelf> parseTypedSelf();
    std::unique_ptr<FunctionParam> parseFunctionParam();
    std::unique_ptr<FunctionReturnType> parseFunctionReturnType();
    std::unique_ptr<BlockExpression> parseBlockExpression();

    std::unique_ptr<StructStruct> parseStructStruct();
    std::unique_ptr<StructFields> parseStructFields(); 
    std::unique_ptr<StructField> parseStructField();

    std::unique_ptr<EnumVariants> parseEnumVariants();
    std::unique_ptr<EnumVariant> parseEnumVariant();

    std::unique_ptr<AssociatedItem> parseAssociatedItem();

    std::unique_ptr<InherentImpl> parseInherentImpl();
    std::unique_ptr<TraitImpl> parseTraitImpl();
    
    std::unique_ptr<Expression> parseExpression();

    std::unique_ptr<PatternNoTopAlt> parsePatternNoTopAlt();

    std::unique_ptr<Type> parseType();
};