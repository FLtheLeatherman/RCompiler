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
    std::vector<std::unique_ptr<Item>> items;
    while (1) {
        auto curItem = parseItem();
        if (curItem == nullptr) break;
        items.push_back(std::move(curItem));
    }
    return std::make_unique<Crate>(std::move(items));
}
std::unique_ptr<Item> Parser::parseItem() {
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
}

std::unique_ptr<Function> Parser::parseFunction() {
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
    return std::make_unique<Function>(is_const, std::move(identifier), std::move(function_parameters), std::move(function_return_type), std::move(block_expression));
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
    match(Token::kLParenthese);
    if (peek() == Token::kRParenthese) {
        consume();
    } else {
        enum_variants = std::move(parseEnumVariants());
        match(Token::kRParenthese);
    }
    return make_unique<Enumeration>(std::move(identifier), std::move(enum_variants));
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
    return make_unique<ConstantItem>(std::move(identifier), std::move(type), std::move(expression));
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
    match(Token::kLParenthese);
    while (1) {
        if (peek() == Token::kRParenthese) {
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
        return make_unique<Implementation>(std::move(parseInherentImpl()));
    } else if (peek() == Token::kIdentifier) {
        return make_unique<Implementation>(std::move(parseTraitImpl()));
    } else {
        throw std::runtime_error("parse failed!");
    }
}

std::unique_ptr<FunctionParameters> Parser::parseFunctionParameters() {
    return nullptr;
}
std::unique_ptr<SelfParam> Parser::parseSelfParam() {
    return nullptr;
}
std::unique_ptr<ShorthandSelf> Parser::parseShorthandSelf() {
    return nullptr;
}
std::unique_ptr<TypedSelf> Parser::parseTypedSelf() {
    return nullptr;
}
std::unique_ptr<FunctionParam> Parser::parseFunctionParam() {
    return nullptr;
}
std::unique_ptr<FunctionReturnType> Parser::parseFunctionReturnType() {
    std::unique_ptr<Type> type;
    match(Token::kRArrow);
    type = std::move(parseType());
    return make_unique<FunctionReturnType>(std::move(type));
}
std::unique_ptr<BlockExpression> Parser::parseBlockExpression() {
    return nullptr;
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
        match(Token::kLParenthese);
        if (peek() == Token::kRParenthese) {
            consume();
        } else {
            struct_fields = std::move(parseStructFields());
            match(Token::kRParenthese);
        }
    }
    return make_unique<StructStruct>(std::move(identifier), std::move(struct_fields));
}
std::unique_ptr<StructFields> Parser::parseStructFields() {
    std::vector<std::unique_ptr<StructField>> struct_field;
    struct_field.push_back(std::move(parseStructField()));
    while (1) {
        if (peek() == Token::kComma) {
            consume();
        } else if (peek() == Token::kRParenthese) {
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
    return make_unique<StructFields>(std::move(struct_field));
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
    return make_unique<StructField>(std::move(identifier), std::move(type));
}

std::unique_ptr<EnumVariants> Parser::parseEnumVariants() {
    std::vector<std::unique_ptr<EnumVariant>> enum_variant;
    enum_variant.push_back(std::move(parseEnumVariant()));
    while (1) {
        if (peek() == Token::kComma) {
            consume();
        } else if (peek() == Token::kRParenthese) {
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
    return make_unique<EnumVariants>(std::move(enum_variant));
}
std::unique_ptr<EnumVariant> Parser::parseEnumVariant() {
    std::string identifier;
    if (peek() == Token::kIdentifier) {
        identifier = get_string();
        consume();
    } else {
        throw std::runtime_error("parse failed!");
    }
    return make_unique<EnumVariant>(std::move(identifier));
}

std::unique_ptr<AssociatedItem> Parser::parseAssociatedItem() {
    if (peek() == Token::kConst) {
        if (pos < tokens.size() && tokens[pos + 1].first == Token::kFn) {
            return make_unique<AssociatedItem>(std::move(parseFunction()));
        } else {
            return make_unique<AssociatedItem>(std::move(parseConstantItem()));
        }
    } else {
        return make_unique<AssociatedItem>(std::move(parseFunction()));
    }
}

std::unique_ptr<InherentImpl> Parser::parseInherentImpl() {
    std::unique_ptr<Type> type;
    std::vector<std::unique_ptr<AssociatedItem>> associated_item;
    type = std::move(parseType());
    match(Token::kLParenthese);
    while (1) {
        if (peek() == Token::kRParenthese) {
            consume();
            break;
        } else {
            auto tmp = std::move(parseAssociatedItem());
            associated_item.push_back(std::move(tmp));
        }
    }
    return make_unique<InherentImpl>(std::move(type), std::move(associated_item));
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
    match(Token::kLParenthese);
    while (1) {
        if (peek() == Token::kRParenthese) {
            consume();
            break;
        } else {
            auto tmp = std::move(parseAssociatedItem());
            associated_item.push_back(std::move(tmp));
        }
    }
    return make_unique<TraitImpl>(std::move(identifier), std::move(type), std::move(associated_item));
}

std::unique_ptr<Type> Parser::parseType() {
    return nullptr;
}

std::unique_ptr<Expression> Parser::parseExpression() {
    return nullptr;
}