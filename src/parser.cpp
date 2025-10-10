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
        consume();
        if (peek() == Token::kFn) {
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
    if (peek() == Token::kFn) {
        consume();
    } else {
        throw std::runtime_error("parse failed!");
    }
    if (peek() == Token::kIdentifier) {
        identifier = get_string();
        consume();
    } else {
        throw std::runtime_error("parse failed!");
    }
    if (peek() == Token::kLParenthese) {
        consume();
    } else {
        throw std::runtime_error("parse failed!");
    }
    if (peek() == Token::kRParenthese) {
        consume();
        function_parameters = nullptr;
    } else {
        function_parameters = std::move(parseFunctionParameters());
        if (peek() == Token::kRParenthese) {
            consume();
        } else {
            throw std::runtime_error("parse failed!");
        }
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

}
std::unique_ptr<ConstantItem> Parser::parseConstantItem() {

}
std::unique_ptr<Trait> Parser::parseTrait() {

}
std::unique_ptr<Implementation> Parser::parseImplementation() {

}

std::unique_ptr<FunctionParameters> Parser::parseFunctionParameters() {

}
std::unique_ptr<FunctionReturnType> Parser::parseFunctionReturnType() {

}
std::unique_ptr<BlockExpression> Parser::parseBlockExpression() {

}

std::unique_ptr<StructStruct> Parser::parseStructStruct() {
    std::string identifier;
    std::unique_ptr<StructFields> struct_fields = nullptr;
    if (peek() == Token::kStruct) {
        consume();
    } else {
        throw std::runtime_error("parse failed!");
    }
    if (peek() == Token::kIdentifier) {
        identifier = get_string();
        consume();
    } else {
        throw std::runtime_error("parse failed!");
    }
    if (peek() == Token::kSemi) {
        consume();
    } else {
        if (peek() == Token::kLParenthese) {
            consume();
        } else {
            throw std::runtime_error("parse failed!");
        }
        if (peek() == Token::kRParenthese) {
            consume();
        } else {
            struct_fields = std::move(parseStructFields());
        }
    }
    return make_unique<StructStruct>(std::move(identifier), std::move(struct_fields));
}
std::unique_ptr<StructFields> Parser::parseStructFields() {

} 