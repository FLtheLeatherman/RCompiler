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

std::shared_ptr<Crate> Parser::parseCrate() {
    // std::cerr << "Crate: {\n";
    std::vector<std::shared_ptr<Item>> items;
    while (1) {
        auto curItem = parseItem();
        if (curItem == nullptr) break;
        items.push_back(std::move(curItem));
    }
    // std::cerr << "}\n";
    return std::make_shared<Crate>(std::move(items));
}

std::shared_ptr<Item> Parser::parseItem() {
    // std::cerr << "Item: {\n";
    // std::cerr << (int)peek() << std::endl;
    if (peek() == Token::kEOF) {
        return nullptr;
    } else if (peek() == Token::kFn) {
        return std::make_shared<Item>(std::move(parseFunction()));
    } else if (peek() == Token::kStruct) {
        return std::make_shared<Item>(std::move(parseStruct()));
    } else if (peek() == Token::kEnum) {
        return std::make_shared<Item>(std::move(parseEnumeration()));
    } else if (peek() == Token::kConst) {
        if (pos < tokens.size() && tokens[pos + 1].first == Token::kFn) {
            return std::make_shared<Item>(std::move(parseFunction()));
        } else {
            return std::make_shared<Item>(std::move(parseConstantItem()));
        }
    } else if (peek() == Token::kTrait) {
        return std::make_shared<Item>(std::move(parseTrait()));
    } else if (peek() == Token::kImpl) {
        return std::make_shared<Item>(std::move(parseImplementation()));
    } else {
        throw std::runtime_error("parse failed!");
    }
    // std::cerr << "}\n";
}
std::shared_ptr<Function> Parser::parseFunction() {
    // std::cerr << "Function: {\n";
    bool is_const = false;
    std::string identifier;
    std::shared_ptr<FunctionParameters> function_parameters = nullptr;
    std::shared_ptr<FunctionReturnType> function_return_type = nullptr;
    std::shared_ptr<BlockExpression> block_expression = nullptr;
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
    return std::make_shared<Function>(is_const, 
        std::move(identifier), 
        std::move(function_parameters), 
        std::move(function_return_type), 
        std::move(block_expression));
}
std::shared_ptr<Struct> Parser::parseStruct() {
    return std::make_shared<Struct>(std::move(parseStructStruct()));
}
std::shared_ptr<Enumeration> Parser::parseEnumeration() {
    std::string identifier;
    std::shared_ptr<EnumVariants> enum_variants;
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
    return std::make_shared<Enumeration>(std::move(identifier), std::move(enum_variants));
}
std::shared_ptr<ConstantItem> Parser::parseConstantItem() {
    std::string identifier;
    std::shared_ptr<Type> type;
    std::shared_ptr<Expression> expression;
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
    return std::make_shared<ConstantItem>(std::move(identifier), std::move(type), std::move(expression));
}
std::shared_ptr<Trait> Parser::parseTrait() {
    std::string identifier;
    std::vector<std::shared_ptr<AssociatedItem>> associated_item;
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
    return std::make_shared<Trait>(std::move(identifier), std::move(associated_item));
}
std::shared_ptr<Implementation> Parser::parseImplementation() {
    match(Token::kImpl);
    if (peek() == Token::kType) {
        return std::make_shared<Implementation>(std::move(parseInherentImpl()));
    } else if (peek() == Token::kIdentifier) {
        return std::make_shared<Implementation>(std::move(parseTraitImpl()));
    } else {
        throw std::runtime_error("parse failed!");
    }
}
std::shared_ptr<FunctionParameters> Parser::parseFunctionParameters() {
    // std::cerr << "FunctionParameters: {";
    bool has_self = false;
    std::shared_ptr<SelfParam> self_param = nullptr;
    std::vector<std::shared_ptr<FunctionParam>> function_param;
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
    return std::make_shared<FunctionParameters>(std::move(self_param), std::move(function_param));
}
std::shared_ptr<SelfParam> Parser::parseSelfParam() {
    size_t tmp = pos;
    while (peek() != Token::kSelf) consume();
    consume();
    if (peek() == Token::kColon) {
        pos = tmp;
        return std::make_shared<SelfParam>(std::move(parseTypedSelf()));
    } else {
        pos = tmp;
        return std::make_shared<SelfParam>(std::move(parseShorthandSelf()));
    }
}
std::shared_ptr<ShorthandSelf> Parser::parseShorthandSelf() {
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
    return std::make_shared<ShorthandSelf>(is_reference, is_mutable);
}
std::shared_ptr<TypedSelf> Parser::parseTypedSelf() {
    bool is_mutable = false;
    std::shared_ptr<Type> type = nullptr;
    if (peek() == Token::kMut) {
        is_mutable = true;
        consume();
    }
    match(Token::kSelf);
    match(Token::kColon);
    type = std::move(parseType());
    return std::make_shared<TypedSelf>(is_mutable, std::move(type));
}
std::shared_ptr<FunctionParam> Parser::parseFunctionParam() {
    std::shared_ptr<PatternNoTopAlt> pattern_no_top_alt;
    std::shared_ptr<Type> type;
    pattern_no_top_alt = std::move(parsePatternNoTopAlt());
    match(Token::kColon);
    type = std::move(parseType());
    return make_shared<FunctionParam>(std::move(pattern_no_top_alt), std::move(type));
}
std::shared_ptr<FunctionReturnType> Parser::parseFunctionReturnType() {
    if (peek() == Token::kRArrow) {
        std::shared_ptr<Type> type;
        consume();
        type = std::move(parseType());
        return std::make_shared<FunctionReturnType>(std::move(type));
    } else {
        return nullptr;
    }
}
std::shared_ptr<StructStruct> Parser::parseStructStruct() {
    std::string identifier;
    std::shared_ptr<StructFields> struct_fields = nullptr;
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
    return std::make_shared<StructStruct>(std::move(identifier), std::move(struct_fields));
}
std::shared_ptr<StructFields> Parser::parseStructFields() {
    std::vector<std::shared_ptr<StructField>> struct_field;
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
    return std::make_shared<StructFields>(std::move(struct_field));
}
std::shared_ptr<StructField> Parser::parseStructField() {
    std::string identifier;
    std::shared_ptr<Type> type;
    if (peek() == Token::kIdentifier) {
        identifier = get_string();
        consume();
    } else {
        throw std::runtime_error("parse failed!");
    }
    match(Token::kColon);
    type = std::move(parseType());
    return std::make_shared<StructField>(std::move(identifier), std::move(type));
}
std::shared_ptr<EnumVariants> Parser::parseEnumVariants() {
    std::vector<std::shared_ptr<EnumVariant>> enum_variant;
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
    return std::make_shared<EnumVariants>(std::move(enum_variant));
}
std::shared_ptr<EnumVariant> Parser::parseEnumVariant() {
    std::string identifier;
    if (peek() == Token::kIdentifier) {
        identifier = get_string();
        consume();
    } else {
        throw std::runtime_error("parse failed!");
    }
    return std::make_shared<EnumVariant>(std::move(identifier));
}
std::shared_ptr<AssociatedItem> Parser::parseAssociatedItem() {
    if (peek() == Token::kConst) {
        if (pos < tokens.size() && tokens[pos + 1].first == Token::kFn) {
            return std::make_shared<AssociatedItem>(std::move(parseFunction()));
        } else {
            return std::make_shared<AssociatedItem>(std::move(parseConstantItem()));
        }
    } else {
        return std::make_shared<AssociatedItem>(std::move(parseFunction()));
    }
}
std::shared_ptr<InherentImpl> Parser::parseInherentImpl() {
    std::shared_ptr<Type> type;
    std::vector<std::shared_ptr<AssociatedItem>> associated_item;
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
    return std::make_shared<InherentImpl>(std::move(type), std::move(associated_item));
}
std::shared_ptr<TraitImpl> Parser::parseTraitImpl() {
    std::string identifier;
    std::shared_ptr<Type> type;
    std::vector<std::shared_ptr<AssociatedItem>> associated_item;
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
    return std::make_shared<TraitImpl>(std::move(identifier), std::move(type), std::move(associated_item));
}

std::shared_ptr<Statement> Parser::parseStatement() {
    if (peek() == Token::kSemi) {
        consume();
        return std::make_shared<Statement>(nullptr);
    } else if (peek() == Token::kLet) {
        return std::make_shared<Statement>(std::move(parseLetStatement()));
    } else {
        std::shared_ptr<ASTNode> child;
        size_t tmp = pos;
        try {
            child = std::move(parseExpressionStatement());
        } catch (...) {
            pos = tmp;
            child = std::move(parseItem());
        }
        return std::make_shared<Statement>(std::move(child));
    }
}
std::shared_ptr<LetStatement> Parser::parseLetStatement() {
    std::shared_ptr<PatternNoTopAlt> pattern_no_top_alt;
    std::shared_ptr<Type> type;
    std::shared_ptr<Expression> expression;
    match(Token::kLet);
    pattern_no_top_alt = parsePatternNoTopAlt();
    match(Token::kColon);
    type = parseType();
    match(Token::kEq);
    expression = parseExpression();
    match(Token::kSemi);
    return std::make_shared<LetStatement>(std::move(pattern_no_top_alt), std::move(type), std::move(expression));
}
std::shared_ptr<ExpressionStatement> Parser::parseExpressionStatement() {
    std::shared_ptr<ASTNode> child;
    size_t tmp = pos;
    bool has_semi = false;
    try {
        child = parseExpressionWithBlock();
        if (peek() == Token::kSemi) {
            consume();
            has_semi = true;
        }
    } catch (...) {
        pos = tmp;
        child = parseExpressionWithoutBlock();
        match(Token::kSemi);
        has_semi = true;
    }
    return std::make_shared<ExpressionStatement>(std::move(child), has_semi);
}

std::shared_ptr<Statements> Parser::parseStatements() {
    std::vector<std::shared_ptr<ASTNode>> statements;
    
    // Try to parse statements until we hit a closing brace or EOF
    while (peek() != Token::kRCurly && peek() != Token::kEOF) {
        size_t tmp = pos;
        std::shared_ptr<ASTNode> statement;
        
        // First try to parse a Statement
        try {
            statement = std::move(parseStatement());
            if (statement) {
                statements.push_back(std::move(statement));
                continue;
            }
        } catch (...) {
            pos = tmp;
        }
        
        // If Statement parsing failed, try to parse ExpressionWithoutBlock
        try {
            statement = std::move(parseExpressionWithoutBlock());
            if (statement) {
                statements.push_back(std::move(statement));
                // Check if there's a semicolon after the expression
                break;
            }
        } catch (...) {
            pos = tmp;
        }
        
        // If both failed, break
        break;
    }
    
    return std::make_shared<Statements>(std::move(statements));
}

std::shared_ptr<Expression> Parser::parseExpression() {
    std::shared_ptr<ASTNode> child;
    size_t tmp = pos;
    try {
        child = parseExpressionWithBlock();
    } catch (...) {
        pos = tmp;
        child = parseExpressionWithoutBlock();
    }
    return std::make_shared<Expression>(std::move(child));
}
std::shared_ptr<ExpressionWithoutBlock> Parser::parseExpressionWithoutBlock() {
    // Pratt parsing will be implemented later
    return nullptr;
}
std::shared_ptr<ExpressionWithBlock> Parser::parseExpressionWithBlock() {
    std::shared_ptr<ASTNode> child;
    
    // Try to parse if expression
    if (peek() == Token::kIf) {
        child = std::move(parseIfExpression());
        return std::make_shared<ExpressionWithBlock>(std::move(child));
    }
    
    // Try to parse loop expression
    if (peek() == Token::kLoop || peek() == Token::kWhile) {
        child = std::move(parseLoopExpression());
        return std::make_shared<ExpressionWithBlock>(std::move(child));
    }
    
    // Try to parse block expression
    if (peek() == Token::kLCurly) {
        child = std::move(parseBlockExpression());
        return std::make_shared<ExpressionWithBlock>(std::move(child));
    }
    
    return nullptr;
}
std::shared_ptr<BlockExpression> Parser::parseBlockExpression() {
    std::shared_ptr<Statements> statements = nullptr;
    
    match(Token::kLCurly);
    
    // Check if there are statements inside the block
    if (peek() != Token::kRCurly) {
        statements = std::move(parseStatements());
    }
    
    match(Token::kRCurly);
    
    return std::make_shared<BlockExpression>(std::move(statements));
}

std::shared_ptr<PatternNoTopAlt> Parser::parsePatternNoTopAlt() {
    return nullptr;
}

std::shared_ptr<Type> Parser::parseType() {
    return nullptr;
}

std::shared_ptr<PathInExpression> Parser::parsePathInExpression() {
    std::shared_ptr<PathIdentSegment> segment1, segment2;
    segment1 = std::move(parsePathIdentSegment());
    size_t tmp = pos;
    int cnt = 0;
    for (size_t _ = 0; _ < 2; ++_) {
        if (peek() == Token::kColon) {
            consume();
            cnt++;
        }
    }
    if (cnt == 2) {
        segment2 = std::move(parsePathIdentSegment());
    } else {
        segment2 = nullptr;
        pos = tmp;
    }
    return std::make_shared<PathInExpression>(std::move(segment1), std::move(segment2));
}

std::shared_ptr<PathIdentSegment> Parser::parsePathIdentSegment() {
    if (peek() == Token::kIdentifier) {
        std::string identifier = get_string();
        consume();
        return std::make_shared<PathIdentSegment>(0, std::move(identifier));
    } else if (peek() == Token::kSelf) {
        return std::make_shared<PathIdentSegment>(1, "");
    } else if (peek() == Token::kSelf) {
        return std::make_shared<PathIdentSegment>(2, "");
    } else {
        throw std::runtime_error("parse failed!");
    }
}


std::shared_ptr<CharLiteral> Parser::parseCharLiteral() {
    std::string value = get_string();
    consume();
    return std::make_shared<CharLiteral>(std::move(value));
}

std::shared_ptr<StringLiteral> Parser::parseStringLiteral() {
    std::string value = get_string();
    consume();
    return std::make_shared<StringLiteral>(std::move(value));
}

std::shared_ptr<RawStringLiteral> Parser::parseRawStringLiteral() {
    std::string value = get_string();
    consume();
    return std::make_shared<RawStringLiteral>(std::move(value));
}

std::shared_ptr<CStringLiteral> Parser::parseCStringLiteral() {
    std::string value = get_string();
    consume();
    return std::make_shared<CStringLiteral>(std::move(value));
}

std::shared_ptr<RawCStringLiteral> Parser::parseRawCStringLiteral() {
    std::string value = get_string();
    consume();
    return std::make_shared<RawCStringLiteral>(std::move(value));
}

std::shared_ptr<IntegerLiteral> Parser::parseIntegerLiteral() {
    std::string value = get_string();
    consume();
    return std::make_shared<IntegerLiteral>(std::move(value));
}

std::shared_ptr<BoolLiteral> Parser::parseBoolLiteral() {
    if (peek() == Token::kTrue) {
        consume();
        return std::make_shared<BoolLiteral>(true);
    } else if (peek() == Token::kFalse) {
        consume();
        return std::make_shared<BoolLiteral>(false);
    } else {
        throw std::runtime_error("parse failed! Expected boolean literal");
    }
}

std::shared_ptr<Condition> Parser::parseCondition() {
    std::shared_ptr<Expression> expression = std::move(parseExpression());
    
    // Check if expression is StructExpression (should not be allowed)
    // StructExpression can only be ExpressionWithoutBlock
    if (expression) {
        // For ExpressionWithoutBlock
        if (auto expr_without_block = std::dynamic_pointer_cast<ExpressionWithoutBlock>(expression)) {
            if (expr_without_block->child) {
                if (std::dynamic_pointer_cast<StructExpression>(expr_without_block->child)) {
                    throw std::runtime_error("parse failed! StructExpression not allowed in if condition");
                }
            }
        }
        // For direct Expression types (including StructExpression)
        else if (std::dynamic_pointer_cast<StructExpression>(expression)) {
            throw std::runtime_error("parse failed! StructExpression not allowed in if condition");
        }
    }
    
    return std::make_shared<Condition>(std::move(expression));
}

std::shared_ptr<IfExpression> Parser::parseIfExpression() {
    std::shared_ptr<Condition> condition;
    std::shared_ptr<BlockExpression> then_block;
    std::shared_ptr<Expression> else_branch = nullptr;
    
    match(Token::kIf);
    
    // Parse condition
    condition = std::move(parseCondition());
    
    // Parse then block
    then_block = std::move(parseBlockExpression());
    
    // Parse optional else branch
    if (peek() == Token::kElse) {
        consume();
        if (peek() == Token::kIf) {
            // else if - parse another IfExpression
            else_branch = std::move(parseIfExpression());
        } else {
            // else block - parse BlockExpression
            else_branch = std::move(parseBlockExpression());
        }
    }
    
    return std::make_shared<IfExpression>(std::move(condition),
                                       std::move(then_block),
                                       std::move(else_branch));
}

std::shared_ptr<ReturnExpression> Parser::parseReturnExpression() {
    std::shared_ptr<Expression> expression = nullptr;
    
    match(Token::kReturn);
    
    // Check if there's an expression after return
    if (peek() != Token::kSemi && peek() != Token::kRCurly && peek() != Token::kEOF) {
        expression = std::move(parseExpression());
    }
    
    return std::make_shared<ReturnExpression>(std::move(expression));
}

// Loop expressions
std::shared_ptr<LoopExpression> Parser::parseLoopExpression() {
    std::shared_ptr<ASTNode> child;
    
    if (peek() == Token::kLoop) {
        child = std::move(parseInfiniteLoopExpression());
    } else if (peek() == Token::kWhile) {
        child = std::move(parsePredicateLoopExpression());
    } else {
        throw std::runtime_error("parse failed! Expected 'loop' or 'while'");
    }
    
    return std::make_shared<LoopExpression>(std::move(child));
}

std::shared_ptr<InfiniteLoopExpression> Parser::parseInfiniteLoopExpression() {
    std::shared_ptr<BlockExpression> block_expression;
    
    match(Token::kLoop);
    block_expression = std::move(parseBlockExpression());
    
    return std::make_shared<InfiniteLoopExpression>(std::move(block_expression));
}

std::shared_ptr<PredicateLoopExpression> Parser::parsePredicateLoopExpression() {
    std::shared_ptr<Condition> condition;
    std::shared_ptr<BlockExpression> block_expression;
    
    match(Token::kWhile);
    condition = std::move(parseCondition());
    block_expression = std::move(parseBlockExpression());
    
    return std::make_shared<PredicateLoopExpression>(std::move(condition), std::move(block_expression));
}

std::shared_ptr<BreakExpression> Parser::parseBreakExpression() {
    std::shared_ptr<Expression> expression = nullptr;
    
    match(Token::kBreak);
    
    // Check if there's an expression after break
    if (peek() != Token::kSemi && peek() != Token::kRCurly && peek() != Token::kEOF) {
        expression = std::move(parseExpression());
    }
    
    return std::make_shared<BreakExpression>(std::move(expression));
}

std::shared_ptr<ContinueExpression> Parser::parseContinueExpression() {
    match(Token::kContinue);
    return std::make_shared<ContinueExpression>();
}