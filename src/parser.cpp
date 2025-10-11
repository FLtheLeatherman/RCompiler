#include "parser.hpp"

Token Parser::peek() {
    if (pos < tokens.size()) return tokens[pos].first;
    else return Token::kEOF;
}
std::string Parser::get_string() {
    if (pos < tokens.size()) return tokens[pos].second;
    else return "";
}
void Parser::consume() {
    pos++;
}
void Parser::match(Token token) {
    if (peek() == token) {
        consume();
    } else {
        throw std::runtime_error("parse failed!");
    }
}

std::unique_ptr<Crate> Parser::parseCrate() {
    // std::cerr << "Crate: {\n";
    std::vector<std::unique_ptr<Item>> items;
    while (1) {
        auto curItem = parseItem();
        if (curItem == nullptr) break;
        items.push_back(std::move(curItem));
    }
    // std::cerr << "}\n";
    return std::make_unique<Crate>(std::move(items));
}

std::unique_ptr<Item> Parser::parseItem() {
    // std::cerr << "Item: {\n";
    // std::cerr << (int)peek() << std::endl;
    if (peek() == Token::kEOF) {
        return nullptr;
    } else if (peek() == Token::kFn) {
        return std::make_unique<Item>(std::move(parseFunction()));
    } else if (peek() == Token::kStruct) {
        return std::make_unique<Item>(std::move(parseStruct()));
    } else if (peek() == Token::kEnum) {
        return std::make_unique<Item>(std::move(parseEnumeration()));
    } else if (peek() == Token::kConst) {
        if (pos < tokens.size() && tokens[pos + 1].first == Token::kFn) {
            return std::make_unique<Item>(std::move(parseFunction()));
        } else {
            return std::make_unique<Item>(std::move(parseConstantItem()));
        }
    } else if (peek() == Token::kTrait) {
        return std::make_unique<Item>(std::move(parseTrait()));
    } else if (peek() == Token::kImpl) {
        return std::make_unique<Item>(std::move(parseImplementation()));
    } else {
        throw std::runtime_error("parse failed!");
    }
    // std::cerr << "}\n";
}
std::unique_ptr<Function> Parser::parseFunction() {
    // std::cerr << "Function: {\n";
    bool is_const = false;
    std::string identifier;
    std::unique_ptr<FunctionParameters> function_parameters = nullptr;
    std::unique_ptr<FunctionReturnType> function_return_type = nullptr;
    std::unique_ptr<BlockExpression> block_expression = nullptr;
    if (peek() == Token::kConst) {
        consume();
        is_const = true;
    }
    match(Token::kFn);
    if (peek() == Token::kIdentifier) {
        identifier = get_string();
        consume();
    } else {
        throw std::runtime_error("parse failed!");
    }
    // std::cerr << "function name: " << identifier << std::endl;
    match(Token::kLParenthese);
    if (peek() == Token::kRParenthese) {
        consume();
        function_parameters = nullptr;
    } else {
        function_parameters = std::move(parseFunctionParameters());
        match(Token::kRParenthese);
    }
    function_return_type = std::move(parseFunctionReturnType());
    if (peek() == Token::kSemi) {
        consume();
    } else {
        block_expression = std::move(parseBlockExpression());
    }
    // std::cerr << "}\n";
    return std::make_unique<Function>(is_const, 
        std::move(identifier), 
        std::move(function_parameters), 
        std::move(function_return_type), 
        std::move(block_expression));
}
std::unique_ptr<Struct> Parser::parseStruct() {
    return std::make_unique<Struct>(std::move(parseStructStruct()));
}
std::unique_ptr<Enumeration> Parser::parseEnumeration() {
    std::string identifier;
    std::unique_ptr<EnumVariants> enum_variants;
    match(Token::kEnum);
    if (peek() == Token::kIdentifier) {
        identifier = get_string();
        consume();
    } else {
        throw std::runtime_error("parse failed!");
    }
    match(Token::kLCurly);
    if (peek() == Token::kRCurly) {
        consume();
    } else {
        enum_variants = std::move(parseEnumVariants());
        match(Token::kRCurly);
    }
    return std::make_unique<Enumeration>(std::move(identifier), std::move(enum_variants));
}
std::unique_ptr<ConstantItem> Parser::parseConstantItem() {
    std::string identifier;
    std::unique_ptr<Type> type;
    std::unique_ptr<Expression> expression;
    match(Token::kConst);
    if (peek() == Token::kIdentifier) {
        identifier = get_string();
        consume();
    } else {
        throw std::runtime_error("parse failed!");
    }
    match(Token::kColon);
    type = std::move(parseType());
    if (peek() == Token::kEq) {
        consume();
        expression = std::move(parseExpression());
    }
    match(Token::kSemi);
    return std::make_unique<ConstantItem>(std::move(identifier), std::move(type), std::move(expression));
}
std::unique_ptr<Trait> Parser::parseTrait() {
    std::string identifier;
    std::vector<std::unique_ptr<AssociatedItem>> associated_item;
    match(Token::kTrait);
    if (peek() == Token::kIdentifier) {
        identifier = get_string();
        consume();
    } else {
        throw std::runtime_error("parse failed!");
    }
    match(Token::kLCurly);
    while (1) {
        if (peek() == Token::kRCurly) {
            consume();
            break;
        } else {
            auto tmp = std::move(parseAssociatedItem());
            associated_item.push_back(std::move(tmp));
        }
    }
    return std::make_unique<Trait>(std::move(identifier), std::move(associated_item));
}
std::unique_ptr<Implementation> Parser::parseImplementation() {
    match(Token::kImpl);
    if (peek() == Token::kType) {
        return std::make_unique<Implementation>(std::move(parseInherentImpl()));
    } else if (peek() == Token::kIdentifier) {
        return std::make_unique<Implementation>(std::move(parseTraitImpl()));
    } else {
        throw std::runtime_error("parse failed!");
    }
}
std::unique_ptr<FunctionParameters> Parser::parseFunctionParameters() {
    // std::cerr << "FunctionParameters: {";
    bool has_self = false;
    std::unique_ptr<SelfParam> self_param = nullptr;
    std::vector<std::unique_ptr<FunctionParam>> function_param;
    size_t tmp = pos;
    for (size_t _ = 0; _ < 3; ++_) {
        if (peek() != Token::kSelf) consume();
        else has_self = true;
    }
    pos = tmp;
    if (has_self) {
        self_param = std::move(parseSelfParam());
    }
    while (1) {
        if (peek() == Token::kComma) {
            consume();
        } else if (peek() == Token::kRParenthese) {
            break;
        } else {
            auto tmp = std::move(parseFunctionParam());
            function_param.push_back(std::move(tmp));
        }
    }
    // std::cerr << "}\n";
    return std::make_unique<FunctionParameters>(std::move(self_param), std::move(function_param));
}
std::unique_ptr<SelfParam> Parser::parseSelfParam() {
    size_t tmp = pos;
    while (peek() != Token::kSelf) consume();
    consume();
    if (peek() == Token::kColon) {
        pos = tmp;
        return std::make_unique<SelfParam>(std::move(parseTypedSelf()));
    } else {
        pos = tmp;
        return std::make_unique<SelfParam>(std::move(parseShorthandSelf()));
    }
}
std::unique_ptr<ShorthandSelf> Parser::parseShorthandSelf() {
    bool is_reference = false, is_mutable = false;
    if (peek() == Token::kAnd) {
        is_reference = true;
        consume();
    }
    if (peek() == Token::kMut) {
        is_mutable = true;
        consume();
    }
    match(Token::kSelf);
    return std::make_unique<ShorthandSelf>(is_reference, is_mutable);
}
std::unique_ptr<TypedSelf> Parser::parseTypedSelf() {
    bool is_mutable = false;
    std::unique_ptr<Type> type = nullptr;
    if (peek() == Token::kMut) {
        is_mutable = true;
        consume();
    }
    match(Token::kSelf);
    match(Token::kColon);
    type = std::move(parseType());
    return std::make_unique<TypedSelf>(is_mutable, std::move(type));
}
std::unique_ptr<FunctionParam> Parser::parseFunctionParam() {
    std::unique_ptr<PatternNoTopAlt> pattern_no_top_alt;
    std::unique_ptr<Type> type;
    pattern_no_top_alt = std::move(parsePatternNoTopAlt());
    match(Token::kColon);
    type = std::move(parseType());
    return make_unique<FunctionParam>(std::move(pattern_no_top_alt), std::move(type));
}
std::unique_ptr<FunctionReturnType> Parser::parseFunctionReturnType() {
    if (peek() == Token::kRArrow) {
        std::unique_ptr<Type> type;
        consume();
        type = std::move(parseType());
        return std::make_unique<FunctionReturnType>(std::move(type));
    } else {
        return nullptr;
    }
}
std::unique_ptr<StructStruct> Parser::parseStructStruct() {
    std::string identifier;
    std::unique_ptr<StructFields> struct_fields = nullptr;
    match(Token::kStruct);
    if (peek() == Token::kIdentifier) {
        identifier = get_string();
        consume();
    } else {
        throw std::runtime_error("parse failed!");
    }
    if (peek() == Token::kSemi) {
        consume();
    } else {
        match(Token::kLCurly);
        if (peek() == Token::kRCurly) {
            consume();
        } else {
            struct_fields = std::move(parseStructFields());
            match(Token::kRCurly);
        }
    }
    return std::make_unique<StructStruct>(std::move(identifier), std::move(struct_fields));
}
std::unique_ptr<StructFields> Parser::parseStructFields() {
    std::vector<std::unique_ptr<StructField>> struct_field;
    struct_field.push_back(std::move(parseStructField()));
    while (1) {
        if (peek() == Token::kComma) {
            consume();
        } else if (peek() == Token::kRCurly) {
            break;
        } else {
            auto tmp = std::move(parseStructField());
            if (tmp != nullptr) {
                struct_field.push_back(std::move(tmp));
            } else {
                throw std::runtime_error("parse failed");
            }
        }
    }
    return std::make_unique<StructFields>(std::move(struct_field));
}
std::unique_ptr<StructField> Parser::parseStructField() {
    std::string identifier;
    std::unique_ptr<Type> type;
    if (peek() == Token::kIdentifier) {
        identifier = get_string();
        consume();
    } else {
        throw std::runtime_error("parse failed!");
    }
    match(Token::kColon);
    type = std::move(parseType());
    return std::make_unique<StructField>(std::move(identifier), std::move(type));
}
std::unique_ptr<EnumVariants> Parser::parseEnumVariants() {
    std::vector<std::unique_ptr<EnumVariant>> enum_variant;
    enum_variant.push_back(std::move(parseEnumVariant()));
    while (1) {
        if (peek() == Token::kComma) {
            consume();
        } else if (peek() == Token::kRCurly) {
            break;
        } else {
            auto tmp = std::move(parseEnumVariant());
            if (tmp != nullptr) {
                enum_variant.push_back(std::move(tmp));
            } else {
                throw std::runtime_error("parse failed!");
            }
        }
    }
    return std::make_unique<EnumVariants>(std::move(enum_variant));
}
std::unique_ptr<EnumVariant> Parser::parseEnumVariant() {
    std::string identifier;
    if (peek() == Token::kIdentifier) {
        identifier = get_string();
        consume();
    } else {
        throw std::runtime_error("parse failed!");
    }
    return std::make_unique<EnumVariant>(std::move(identifier));
}
std::unique_ptr<AssociatedItem> Parser::parseAssociatedItem() {
    if (peek() == Token::kConst) {
        if (pos < tokens.size() && tokens[pos + 1].first == Token::kFn) {
            return std::make_unique<AssociatedItem>(std::move(parseFunction()));
        } else {
            return std::make_unique<AssociatedItem>(std::move(parseConstantItem()));
        }
    } else {
        return std::make_unique<AssociatedItem>(std::move(parseFunction()));
    }
}
std::unique_ptr<InherentImpl> Parser::parseInherentImpl() {
    std::unique_ptr<Type> type;
    std::vector<std::unique_ptr<AssociatedItem>> associated_item;
    type = std::move(parseType());
    match(Token::kRCurly);
    while (1) {
        if (peek() == Token::kRCurly) {
            consume();
            break;
        } else {
            auto tmp = std::move(parseAssociatedItem());
            associated_item.push_back(std::move(tmp));
        }
    }
    return std::make_unique<InherentImpl>(std::move(type), std::move(associated_item));
}
std::unique_ptr<TraitImpl> Parser::parseTraitImpl() {
    std::string identifier;
    std::unique_ptr<Type> type;
    std::vector<std::unique_ptr<AssociatedItem>> associated_item;
    if (peek() == Token::kIdentifier) {
        identifier = get_string();
        consume();
    } else {
        throw std::runtime_error("parse failed!");
    }
    match(Token::kFor);
    type = std::move(parseType());
    match(Token::kLCurly);
    while (1) {
        if (peek() == Token::kRCurly) {
            consume();
            break;
        } else {
            auto tmp = std::move(parseAssociatedItem());
            associated_item.push_back(std::move(tmp));
        }
    }
    return std::make_unique<TraitImpl>(std::move(identifier), std::move(type), std::move(associated_item));
}

std::unique_ptr<Statement> Parser::parseStatement() {
    if (peek() == Token::kSemi) {
        consume();
        return std::make_unique<Statement>(nullptr);
    } else if (peek() == Token::kLet) {
        return std::make_unique<Statement>(std::move(parseLetStatement()));
    } else {
        std::unique_ptr<ASTNode> child;
        size_t tmp = pos;
        try {
            child = std::move(parseItem());
        } catch (...) {
            pos = tmp;
            child = std::move(parseExpressionStatement());
        }
        return std::make_unique<Statement>(std::move(child));
    }
}
std::unique_ptr<LetStatement> Parser::parseLetStatement() {
    std::unique_ptr<PatternNoTopAlt> pattern_no_top_alt;
    std::unique_ptr<Type> type;
    std::unique_ptr<Expression> expression;
    match(Token::kLet);
    pattern_no_top_alt = parsePatternNoTopAlt();
    match(Token::kColon);
    type = parseType();
    match(Token::kEq);
    expression = parseExpression();
    match(Token::kSemi);
    return std::make_unique<LetStatement>(std::move(pattern_no_top_alt), std::move(type), std::move(expression));
}
std::unique_ptr<ExpressionStatement> Parser::parseExpressionStatement() {
    std::unique_ptr<ASTNode> child;
    size_t tmp = pos;
    try {
        child = parseExpressionWithBlock();
        if (peek() == Token::kSemi) consume();
    } catch (...) {
        pos = tmp;
        child = parseExpressionWithoutBlock();
        match(Token::kSemi);
    }
    return std::make_unique<ExpressionStatement>(std::move(child));
}
std::unique_ptr<Expression> Parser::parseExpression() {
    return nullptr;
}
std::unique_ptr<ExpressionWithoutBlock> Parser::parseExpressionWithoutBlock() {
    return nullptr;
}
std::unique_ptr<ExpressionWithBlock> Parser::parseExpressionWithBlock() {
    return nullptr;
}
std::unique_ptr<BlockExpression> Parser::parseBlockExpression() {
    return nullptr;
}

std::unique_ptr<PatternNoTopAlt> Parser::parsePatternNoTopAlt() {
    return nullptr;
}

std::unique_ptr<Type> Parser::parseType() {
    return nullptr;
}