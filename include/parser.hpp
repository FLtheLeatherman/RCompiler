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

    std::shared_ptr<Crate> parseCrate();

    std::shared_ptr<Item> parseItem();
    std::shared_ptr<Function> parseFunction();
    std::shared_ptr<Struct> parseStruct();
    std::shared_ptr<Enumeration> parseEnumeration();
    std::shared_ptr<ConstantItem> parseConstantItem();
    std::shared_ptr<Trait> parseTrait();
    std::shared_ptr<Implementation> parseImplementation();
    std::shared_ptr<FunctionParameters> parseFunctionParameters();
    std::shared_ptr<SelfParam> parseSelfParam();
    std::shared_ptr<ShorthandSelf> parseShorthandSelf();
    std::shared_ptr<TypedSelf> parseTypedSelf();
    std::shared_ptr<FunctionParam> parseFunctionParam();
    std::shared_ptr<FunctionReturnType> parseFunctionReturnType();
    std::shared_ptr<StructStruct> parseStructStruct();
    std::shared_ptr<StructFields> parseStructFields(); 
    std::shared_ptr<StructField> parseStructField();
    std::shared_ptr<EnumVariants> parseEnumVariants();
    std::shared_ptr<EnumVariant> parseEnumVariant();
    std::shared_ptr<AssociatedItem> parseAssociatedItem();
    std::shared_ptr<InherentImpl> parseInherentImpl();
    std::shared_ptr<TraitImpl> parseTraitImpl();
    
    std::shared_ptr<Statement> parseStatement();
    std::shared_ptr<LetStatement> parseLetStatement();
    std::shared_ptr<ExpressionStatement> parseExpressionStatement();
    std::shared_ptr<Expression> parseExpression();
    std::shared_ptr<ExpressionWithoutBlock> parseExpressionWithoutBlock();
    std::shared_ptr<ExpressionWithBlock> parseExpressionWithBlock();
    std::shared_ptr<BlockExpression> parseBlockExpression();

    std::shared_ptr<PatternNoTopAlt> parsePatternNoTopAlt();

    std::shared_ptr<Type> parseType();

    std::shared_ptr<PathInExpression> parsePathInExpression();
    std::shared_ptr<PathIdentSegment> parsePathIdentSegment();
};