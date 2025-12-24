#include "parser/parser.hpp"

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
        throw std::runtime_error(std::string("parse failed! Expected token ") + tokenToString(token) + std::string(" in match"));
    }
}

int Parser::getTokenLeftBP(Token token) {
    switch (token) {
        // PATH_ACCESS = 200
        case Token::kPathSep:
        case Token::kDot:
            return PATH_ACCESS;
        
        // CALL_INDEX = 190
        case Token::kLParenthese:  // function call
        case Token::kLSquare:     // array indexing
            return CALL_INDEX;
        
        // STRUCT_EXPR = 180
        case Token::kLCurly:      // struct expression
            return STRUCT_EXPR;
        
        // TYPE_CAST = 160 (as operator)
        case Token::kAs:
            return TYPE_CAST;
        
        // MUL_DIV_MOD = 150
        case Token::kStar:
        case Token::kSlash:
        case Token::kPercent:
            return MUL_DIV_MOD;
        
        // ADD_SUB = 140
        case Token::kPlus:
        case Token::kMinus:
            return ADD_SUB;
        
        // SHIFT = 130
        case Token::kShl:
        case Token::kShr:
            return SHIFT;
        
        // BIT_AND = 120
        case Token::kAnd:
            return BIT_AND;
        
        // BIT_XOR = 110
        case Token::kCaret:
            return BIT_XOR;
        
        // BIT_OR = 100
        case Token::kOr:
            return BIT_OR;
        
        // COMPARISON = 90
        case Token::kEqEq:
        case Token::kNe:
        case Token::kLt:
        case Token::kLe:
        case Token::kGt:
        case Token::kGe:
            return COMPARISON;
        
        // LOGIC_AND = 80
        case Token::kAndAnd:
            return LOGIC_AND;
        
        // LOGIC_OR = 70
        case Token::kOrOr:
            return LOGIC_OR;
        
        // ASSIGNMENT = 60
        case Token::kEq:
        case Token::kPlusEq:
        case Token::kMinusEq:
        case Token::kStarEq:
        case Token::kSlashEq:
        case Token::kPercentEq:
        case Token::kCaretEq:
        case Token::kAndEq:
        case Token::kOrEq:
        case Token::kShlEq:
        case Token::kShrEq:
        case Token::kDotDot:
        case Token::kDotDotDot:
        case Token::kDotDotEq:
            return ASSIGNMENT;
        
        // FLOW_CONTROL = 50
        case Token::kReturn:
        case Token::kBreak:
        case Token::kContinue:
            return FLOW_CONTROL;
        
        default:
            return 0; // No binding power for non-operators
    }
}

int Parser::getTokenUnaryBP(Token token) {
    switch (token) {
        // UNARY = 170
        case Token::kMinus:       // unary minus
        case Token::kStar:        // dereference
        case Token::kAnd:         // reference
        case Token::kNot:         // logical not
        case Token::kQuestion:    // try operator
            return UNARY;
        
        default:
            return 0; // No unary binding power for non-unary operators
    }
}

int Parser::getTokenRightBP(Token token) {
    // For all operators, we treat them as left-associative
    // So getTokenRightBP should return getTokenLeftBP + 1
    int leftBP = getTokenLeftBP(token);
    return leftBP > 0 ? leftBP + 1 : 0;
}

std::shared_ptr<ASTNode> Parser::parsePrattPrefix() {
    // std::cerr << "parsePrattPrefix:" << std::endl;
    Token token = peek();
    // std::cerr << tokenToString(token) << std::endl;
    
    switch (token) {
        // Control flow expressions
        case Token::kIf:
            return parseIfExpression();
        
        case Token::kLoop:
        case Token::kWhile:
            return parseLoopExpression();
        
        case Token::kReturn:
            return parseReturnExpression();
        
        case Token::kBreak:
            return parseBreakExpression();
        
        case Token::kContinue:
            return parseContinueExpression();
        
        // Block expressions
        case Token::kLCurly:
            return parseBlockExpression();
        
        // Grouped expressions
        case Token::kLParenthese:
            return parseGroupedExpression();
        
        // Array expressions
        case Token::kLSquare:
            return parseArrayExpression();
        
        // Struct expressions
        case Token::kSelf:
        case Token::kSelf_:
        case Token::kIdentifier: {
            // std::cerr << "Identifier in Prefix!" << std::endl;
            // This could be a path expression or struct expression
            // We need to look ahead to determine
            size_t tmp = pos;
            consume(); // consume identifier
            if (peek() == Token::kLCurly) {
                pos = tmp; // reset position
                return parseStructExpression();
            } else {
                pos = tmp; // reset position
                return parsePathExpression();
            }
        }
        
        // Unary operators
        case Token::kMinus:
        case Token::kNot:
        case Token::kQuestion: {
            return parseUnaryExpression();
        }
        
        case Token::kStar: {
            return parseDereferenceExpression();
        }
        
        case Token::kAnd: {
            return parseBorrowExpression();
        }
        
        // Literals
        case Token::kCharLiteral:
            return parseCharLiteral();
        
        case Token::kStringLiteral:
            return parseStringLiteral();
        
        case Token::kRawStringLiteral:
            return parseRawStringLiteral();
        
        case Token::kCStringLiteral:
            return parseCStringLiteral();
        
        case Token::kRawCStringLiteral:
            return parseRawCStringLiteral();
        
        case Token::kIntegerLiteral:
            return parseIntegerLiteral();
        
        case Token::kTrue:
        case Token::kFalse:
            return parseBoolLiteral();
        
        default:
            throw std::runtime_error("parse failed! Unexpected token in prefix expression");
    }
}

std::shared_ptr<ASTNode> Parser::parsePrattExpression(int current_bp) {
    // std::cerr << "PrattExpression:" << std::endl;
    // Try to parse prefix expression
    auto lhs = parsePrattPrefix();
    if (!lhs) {
        return nullptr;
    }
    // std::cerr << "lhs is good!" << std::endl;
    // std::cerr << pos << std::endl;
    // While the next operator has higher binding power, consume it
    while (true) {
        Token next_token = peek();

        // std::cerr << pos << ' ' << tokenToString(next_token) << std::endl;
        
        // Check for closing tokens that should break the loop
        if (next_token == Token::kRParenthese || next_token == Token::kRSquare || next_token == Token::kRCurly) {
            // std::cerr << "break!" << std::endl;
            break;
        }
        
        int next_bp = getTokenLeftBP(next_token);

        // std::cerr << next_bp << ' ' << current_bp << std::endl;

        if (next_bp <= current_bp) {
            break;
        }
        
        // Create proper expression node based on operator type
        switch (next_token) {
            // Function call
            case Token::kLParenthese: {
                lhs = parseCallExpressionFromInfix(std::dynamic_pointer_cast<Expression>(lhs));
                break;
            }
            
            // Index expression
            case Token::kLSquare: {
                lhs = parseIndexExpressionFromInfix(std::dynamic_pointer_cast<Expression>(lhs));
                break;
            }
            
            // Method call and field access
            case Token::kDot: {
                // Look ahead to determine if it's a method call or field access
                auto lhs_ = lhs;
                size_t tmp = pos;
                try {
                    lhs = parseMethodCallExpressionFromInfix(std::dynamic_pointer_cast<Expression>(lhs));
                } catch (...) {
                    pos = tmp;
                    lhs = lhs_;
                    lhs = parseFieldExpressionFromInfix(std::dynamic_pointer_cast<Expression>(lhs));
                }
                break;
            }
            
            // Type cast
            case Token::kAs: {
                consume(); // Consume 'as'
                auto type = parseType();
                lhs = parseTypeCastExpression(std::dynamic_pointer_cast<Expression>(lhs), std::move(type));
                break;
            }
            
            // Assignment and binary operators - need to parse RHS
            default: {
                // std::cerr << "Are we here?" << std::endl;
                // Consume the operator and parse the right-hand side
                consume();
                int right_bp = getTokenRightBP(next_token);
                auto rhs = parsePrattExpression(right_bp);
                
                if (!rhs) {
                    throw std::runtime_error("parse failed! Expected expression after operator");
                }
                
                // Assignment
                if (next_token == Token::kEq) {
                    lhs = parseAssignmentExpression(std::dynamic_pointer_cast<Expression>(lhs), std::dynamic_pointer_cast<Expression>(rhs));
                }
                // Compound assignment
                else if (next_token == Token::kPlusEq) {
                    lhs = parseCompoundAssignmentExpression(CompoundAssignmentExpression::PLUS_EQ, std::dynamic_pointer_cast<Expression>(lhs), std::dynamic_pointer_cast<Expression>(rhs));
                }
                else if (next_token == Token::kMinusEq) {
                    lhs = parseCompoundAssignmentExpression(CompoundAssignmentExpression::MINUS_EQ, std::dynamic_pointer_cast<Expression>(lhs), std::dynamic_pointer_cast<Expression>(rhs));
                }
                else if (next_token == Token::kStarEq) {
                    lhs = parseCompoundAssignmentExpression(CompoundAssignmentExpression::STAR_EQ, std::dynamic_pointer_cast<Expression>(lhs), std::dynamic_pointer_cast<Expression>(rhs));
                }
                else if (next_token == Token::kSlashEq) {
                    lhs = parseCompoundAssignmentExpression(CompoundAssignmentExpression::SLASH_EQ, std::dynamic_pointer_cast<Expression>(lhs), std::dynamic_pointer_cast<Expression>(rhs));
                }
                else if (next_token == Token::kPercentEq) {
                    lhs = parseCompoundAssignmentExpression(CompoundAssignmentExpression::PERCENT_EQ, std::dynamic_pointer_cast<Expression>(lhs), std::dynamic_pointer_cast<Expression>(rhs));
                }
                else if (next_token == Token::kCaretEq) {
                    lhs = parseCompoundAssignmentExpression(CompoundAssignmentExpression::CARET_EQ, std::dynamic_pointer_cast<Expression>(lhs), std::dynamic_pointer_cast<Expression>(rhs));
                }
                else if (next_token == Token::kAndEq) {
                    lhs = parseCompoundAssignmentExpression(CompoundAssignmentExpression::AND_EQ, std::dynamic_pointer_cast<Expression>(lhs), std::dynamic_pointer_cast<Expression>(rhs));
                }
                else if (next_token == Token::kOrEq) {
                    lhs = parseCompoundAssignmentExpression(CompoundAssignmentExpression::OR_EQ, std::dynamic_pointer_cast<Expression>(lhs), std::dynamic_pointer_cast<Expression>(rhs));
                }
                else if (next_token == Token::kShlEq) {
                    lhs = parseCompoundAssignmentExpression(CompoundAssignmentExpression::SHL_EQ, std::dynamic_pointer_cast<Expression>(lhs), std::dynamic_pointer_cast<Expression>(rhs));
                }
                else if (next_token == Token::kShrEq) {
                    lhs = parseCompoundAssignmentExpression(CompoundAssignmentExpression::SHR_EQ, std::dynamic_pointer_cast<Expression>(lhs), std::dynamic_pointer_cast<Expression>(rhs));
                }
                // Binary operators
                else if (next_token == Token::kPlus) {
                    lhs = parseBinaryExpression(BinaryExpression::PLUS, std::dynamic_pointer_cast<Expression>(lhs), std::dynamic_pointer_cast<Expression>(rhs));
                }
                else if (next_token == Token::kMinus) {
                    lhs = parseBinaryExpression(BinaryExpression::MINUS, std::dynamic_pointer_cast<Expression>(lhs), std::dynamic_pointer_cast<Expression>(rhs));
                }
                else if (next_token == Token::kStar) {
                    lhs = parseBinaryExpression(BinaryExpression::STAR, std::dynamic_pointer_cast<Expression>(lhs), std::dynamic_pointer_cast<Expression>(rhs));
                }
                else if (next_token == Token::kSlash) {
                    lhs = parseBinaryExpression(BinaryExpression::SLASH, std::dynamic_pointer_cast<Expression>(lhs), std::dynamic_pointer_cast<Expression>(rhs));
                }
                else if (next_token == Token::kPercent) {
                    lhs = parseBinaryExpression(BinaryExpression::PERCENT, std::dynamic_pointer_cast<Expression>(lhs), std::dynamic_pointer_cast<Expression>(rhs));
                }
                else if (next_token == Token::kCaret) {
                    lhs = parseBinaryExpression(BinaryExpression::CARET, std::dynamic_pointer_cast<Expression>(lhs), std::dynamic_pointer_cast<Expression>(rhs));
                }
                else if (next_token == Token::kAnd) {
                    lhs = parseBinaryExpression(BinaryExpression::AND, std::dynamic_pointer_cast<Expression>(lhs), std::dynamic_pointer_cast<Expression>(rhs));
                }
                else if (next_token == Token::kOr) {
                    lhs = parseBinaryExpression(BinaryExpression::OR, std::dynamic_pointer_cast<Expression>(lhs), std::dynamic_pointer_cast<Expression>(rhs));
                }
                else if (next_token == Token::kShl) {
                    lhs = parseBinaryExpression(BinaryExpression::SHL, std::dynamic_pointer_cast<Expression>(lhs), std::dynamic_pointer_cast<Expression>(rhs));
                }
                else if (next_token == Token::kShr) {
                    lhs = parseBinaryExpression(BinaryExpression::SHR, std::dynamic_pointer_cast<Expression>(lhs), std::dynamic_pointer_cast<Expression>(rhs));
                }
                else if (next_token == Token::kEqEq) {
                    lhs = parseBinaryExpression(BinaryExpression::EQ_EQ, std::dynamic_pointer_cast<Expression>(lhs), std::dynamic_pointer_cast<Expression>(rhs));
                }
                else if (next_token == Token::kNe) {
                    lhs = parseBinaryExpression(BinaryExpression::NE, std::dynamic_pointer_cast<Expression>(lhs), std::dynamic_pointer_cast<Expression>(rhs));
                }
                else if (next_token == Token::kGt) {
                    lhs = parseBinaryExpression(BinaryExpression::GT, std::dynamic_pointer_cast<Expression>(lhs), std::dynamic_pointer_cast<Expression>(rhs));
                }
                else if (next_token == Token::kLt) {
                    lhs = parseBinaryExpression(BinaryExpression::LT, std::dynamic_pointer_cast<Expression>(lhs), std::dynamic_pointer_cast<Expression>(rhs));
                }
                else if (next_token == Token::kGe) {
                    lhs = parseBinaryExpression(BinaryExpression::GE, std::dynamic_pointer_cast<Expression>(lhs), std::dynamic_pointer_cast<Expression>(rhs));
                }
                else if (next_token == Token::kLe) {
                    lhs = parseBinaryExpression(BinaryExpression::LE, std::dynamic_pointer_cast<Expression>(lhs), std::dynamic_pointer_cast<Expression>(rhs));
                }
                else if (next_token == Token::kAndAnd) {
                    lhs = parseBinaryExpression(BinaryExpression::AND_AND, std::dynamic_pointer_cast<Expression>(lhs), std::dynamic_pointer_cast<Expression>(rhs));
                }
                else if (next_token == Token::kOrOr) {
                    lhs = parseBinaryExpression(BinaryExpression::OR_OR, std::dynamic_pointer_cast<Expression>(lhs), std::dynamic_pointer_cast<Expression>(rhs));
                }
                else {
                    throw std::runtime_error("parse failed! Unexpected operator in infix expression");
                }
                break;
            }
        }
    }
    
    return lhs;
}

std::shared_ptr<Crate> Parser::parseCrate() {
    // std::cerr << "Crate: " << std::endl;
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
    // std::cerr << "Item: " << std::endl;
    // std::cerr << pos << ' ' << tokenToString(peek()) << std::endl;
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
        throw std::runtime_error("parse failed! Unexpected token in Item");
    }
    // std::cerr << "}\n";
}
std::shared_ptr<Function> Parser::parseFunction() {
    // std::cerr << "Function: " << std::endl;
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
        throw std::runtime_error("parse failed! Unexpected token in function");
    }
    // std::cerr << "function name: " << identifier << std::endl;
    match(Token::kLParenthese);
    // std::cerr << tokenToString(peek()) << std::endl;
    if (peek() == Token::kRParenthese) {
        consume();
        function_parameters = nullptr;
    } else {
        function_parameters = std::move(parseFunctionParameters());
        match(Token::kRParenthese);
    }
    // std::cerr << "function param done" << std::endl;
    // std::cerr << pos << std::endl;
    function_return_type = std::move(parseFunctionReturnType());
    // std::cerr << "function return type done" << std::endl;
    if (peek() == Token::kSemi) {
        consume();
    } else {
        // std::cerr << "start function block expression" << std::endl;
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
        throw std::runtime_error("parse failed! Unexpected token in struct");
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
        throw std::runtime_error("parse failed! Unexpected token in constant item");
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
        throw std::runtime_error("parse failed! Unexpected token in trait");
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
    std::shared_ptr<ASTNode> child;
    size_t tmp = pos;
    try {
        child = std::move(parseTraitImpl());
    } catch (...) {
        try {
            pos = tmp;
            child = std::move(parseInherentImpl());
        } catch (...) {
            throw std::runtime_error("parse failed! Unexpected token in implementation");
        }
    }
    return std::make_shared<Implementation>(std::move(child));
}
std::shared_ptr<FunctionParameters> Parser::parseFunctionParameters() {
    // std::cerr << "FunctionParameter:";
    bool has_self = false;
    std::shared_ptr<SelfParam> self_param = nullptr;
    std::vector<std::shared_ptr<FunctionParam>> function_param;
    size_t tmp = pos;
    for (size_t _ = 0; _ < 3; ++_) {
        if (peek() != Token::kSelf) consume();
        else has_self = true;
    }
    pos = tmp;
    // std::cerr << has_self << ' ' << pos << std::endl;
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
        throw std::runtime_error("parse failed! Unexpected token in structstruct");
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
        throw std::runtime_error("parse failed! Unexpected token in struct field");
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
                throw std::runtime_error("parse failed! Unexpected token in enum variants");
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
        throw std::runtime_error("parse failed! Unexpected token in enum variant");
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
    // std::cerr << "InherentImpl:" << std::endl;
    // std::cerr << pos << std::endl;
    std::shared_ptr<Type> type;
    std::vector<std::shared_ptr<AssociatedItem>> associated_item;
    type = std::move(parseType());
    // std::cerr << "Type matched!" << std::endl;
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
        throw std::runtime_error("parse failed! Unexpected token in trait impl");
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
    // std::cerr << "Statement:" << std::endl;
    // std::cerr << pos << ' ' << tokenToString(peek()) << std::endl;
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
    // std::cerr << "LetStatement:" << std::endl;
    std::shared_ptr<PatternNoTopAlt> pattern_no_top_alt;
    std::shared_ptr<Type> type;
    std::shared_ptr<Expression> expression;
    match(Token::kLet);
    pattern_no_top_alt = parsePatternNoTopAlt();
    match(Token::kColon);
    // std::cerr << "colon matched!" << std::endl;
    type = parseType();
    // std::cerr << "type matched!" << std::endl;
    match(Token::kEq);
    // std::cerr << "eq matched!" << std::endl;
    expression = parseExpression();
    // std::cerr << "expression matched!" << std::endl;
    match(Token::kSemi);
    // std::cerr << "semi matched!" << std::endl;
    return std::make_shared<LetStatement>(std::move(pattern_no_top_alt), std::move(type), std::move(expression));
}
std::shared_ptr<ExpressionStatement> Parser::parseExpressionStatement() {
    // std::cerr << "ExpressionStatement:" << std::endl;
    std::shared_ptr<ASTNode> child;
    size_t tmp = pos;
    bool has_semi = false;
    try {
        child = parseExpressionWithoutBlock();
        match(Token::kSemi);
        has_semi = true;
    } catch (...) {
        // std::cerr << "failed ExpressionWithouBlock!" << std::endl;
        pos = tmp;
        child = parseExpressionWithBlock();
        if (peek() == Token::kSemi) {
            consume();
            has_semi = true;
        }
        // std::cerr << "succeeded ExpressionWithouBlock " << pos << std::endl;
    }
    return std::make_shared<ExpressionStatement>(std::move(child), has_semi);
}
int ccnt = 0;
std::shared_ptr<Statements> Parser::parseStatements() {
    // std::cerr << "Statements:" << std::endl;
    std::vector<std::shared_ptr<ASTNode>> statements;
    ccnt++;
    // std::cerr << ccnt << ' ' << pos << std::endl;
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
            // std::cerr << "statements failed statement " << ccnt << ' ' << pos << ' ' << tmp << std::endl;
            statement = std::move(parseExpressionWithoutBlock());
            if (statement) {
                // std::cerr << "succeeded ExpressionWithoutBlock in Statements " << pos << std::endl;
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
    // std::cerr << "Expression:" << std::endl;
    // std::shared_ptr<ASTNode> child;
    // size_t tmp = pos;
    // try {
    //     child = parseExpressionWithBlock();
    // } catch (...) {
    //     pos = tmp;
    //     child = parsePrattExpression(0);
    // }
    auto child = parsePrattExpression(0);
    return std::make_shared<Expression>(std::move(child));
}
std::shared_ptr<ExpressionWithoutBlock> Parser::parseExpressionWithoutBlock() {
    // std::cerr << "parseExpressionWithoutBlock: " << pos << std::endl;
    auto expression = parsePrattExpression(0);

    // std::cerr << "at least here" << std::endl;
    
    // 检查返回的表达式是否可以被 cast 到 ExpressionWithBlock
    if (std::dynamic_pointer_cast<ExpressionWithBlock>(expression)
     || std::dynamic_pointer_cast<IfExpression>(expression)
     || std::dynamic_pointer_cast<LoopExpression>(expression)
     || std::dynamic_pointer_cast<BlockExpression>(expression)
    ) {
        // std::cerr << "this is not what we wanted" << std::endl;
        throw std::runtime_error("parse failed! ExpressionWithBlock not allowed in ExpressionWithoutBlock context");
    }

    // if (expression == nullptr) std::cerr << "???" << std::endl;

    // std::cerr << "we are good!" << std::endl;
    // std::cerr << pos << std::endl;
    
    // 如果不能 cast 到 ExpressionWithBlock，则正常运行，创建 ExpressionWithoutBlock
    return std::make_shared<ExpressionWithoutBlock>(std::move(expression));
}
std::shared_ptr<ExpressionWithBlock> Parser::parseExpressionWithBlock() {
    // std::cerr << "ExpressionWithBlock:" << std::endl;
    // std::cerr << pos << ' ' << tokenToString(peek()) << std::endl;
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

    // std::cerr << "ExpressionWithBlock not matched!" << std::endl;
    throw(std::runtime_error("parse failed! Not an ExpressionWithBlock"));

    return nullptr;
}
std::shared_ptr<BlockExpression> Parser::parseBlockExpression() {
    // std::cerr << "BlockExpression:" << std::endl;
    // std::cerr << "!!" << pos << std::endl;
    std::shared_ptr<Statements> statements = nullptr;
    
    match(Token::kLCurly);
    
    // Check if there are statements inside the block
    
    auto tmp = pos;
    if (peek() != Token::kRCurly) {
        
        statements = std::move(parseStatements());
    }
    
    try {
        match(Token::kRCurly);
    } catch (...) {
        // std::cerr << "we are here!" << std::endl;
        pos = tmp;
        std::vector<std::shared_ptr<ASTNode>> vec;
        vec.push_back(std::move(parseExpressionWithoutBlock()));
        statements = std::make_shared<Statements>(std::move(vec));
        match(Token::kRCurly);
    }
    
    return std::make_shared<BlockExpression>(std::move(statements));
}

std::shared_ptr<PatternNoTopAlt> Parser::parsePatternNoTopAlt() {
    // PatternNoTopAlt → IdentifierPattern | ReferencePattern
    // std::cerr << "PatternNoTopAlt:" << std::endl;

    try {
        auto pattern = parseIdentifierPattern();
        return std::make_shared<PatternNoTopAlt>(std::move(pattern));
    } catch (...) {
        auto pattern = parseReferencePattern();
        return std::make_shared<PatternNoTopAlt>(std::move(pattern));
    }
}

std::shared_ptr<IdentifierPattern> Parser::parseIdentifierPattern() {
    // std::cerr << "IdentifierPattern" << std::endl;
    // IdentifierPattern → `ref`? `mut`? IDENTIFIER
    bool is_ref = false;
    bool is_mutable = false;
    std::string identifier;
    
    // Check for 'ref'
    if (peek() == Token::kRef) {
        is_ref = true;
        consume(); // consume 'ref'
    }
    
    // Check for 'mut'
    if (peek() == Token::kMut) {
        is_mutable = true;
        consume(); // consume 'mut'
    }
    
    // Expect identifier
    if (peek() == Token::kIdentifier) {
        identifier = get_string();
        // std::cerr << "IDENTIFIER: " << identifier << std::endl;
        consume(); // consume identifier
    } else {
        throw std::runtime_error("parse failed! Expected identifier in pattern");
    }
    
    return std::make_shared<IdentifierPattern>(is_ref, is_mutable, std::move(identifier));
}

std::shared_ptr<ReferencePattern> Parser::parseReferencePattern() {
    // ReferencePattern → ( `&` | `&&` ) `mut`? PatternNoTopAlt
    
    size_t tmp = pos;
    
    bool is_double = false;
    if (peek() == Token::kAnd) {
        consume(); // consume first '&'
        if (peek() == Token::kAnd) {
            is_double = true;
            consume(); // consume second '&'
        } else {
            // Single reference
        }
    } else {
        pos = tmp; // backtrack, this wasn't a reference pattern
        return nullptr;
    }
    
    bool is_mutable = false;
    if (peek() == Token::kMut) {
        is_mutable = true;
        consume(); // consume 'mut'
    }
    
    auto pattern = parsePatternNoTopAlt();
    if (!pattern) {
        throw std::runtime_error("parse failed! Expected pattern after reference");
    }
    
    return std::make_shared<ReferencePattern>(is_double, is_mutable, std::move(pattern));
}

std::shared_ptr<Type> Parser::parseType() {
    // std::cerr << "Type:" << std::endl;
    // Type → PathIdentSegment | ReferenceType | ArrayType | UnitType
    std::shared_ptr<ASTNode> child;
    
    // Try UnitType first: `(` `)`
    if (peek() == Token::kLParenthese) {
        size_t tmp = pos;
        consume();
        if (peek() == Token::kRParenthese) {
            consume();
            child = std::make_shared<UnitType>();
            return std::make_shared<Type>(std::move(child));
        } else {
            pos = tmp; // backtrack
        }
    }
    
    // Try ReferenceType: `&` `mut`? Type
    if (peek() == Token::kAnd) {
        child = parseReferenceType();
        if (child) {
            return std::make_shared<Type>(std::move(child));
        }
    }
    
    // Try ArrayType: `[` Type `;` Expression `]`
    if (peek() == Token::kLSquare) {
        child = parseArrayType();
        if (child) {
            return std::make_shared<Type>(std::move(child));
        }
    }
    // std::cerr << "parsePathIdentSegment?" << std::endl;
    // Try PathIdentSegment
    child = parsePathIdentSegment();
    if (child) {
        return std::make_shared<Type>(std::move(child));
    }
    
    throw std::runtime_error("parse type failed!");
    return nullptr;
}

std::shared_ptr<ReferenceType> Parser::parseReferenceType() {
    // ReferenceType → `&` `mut`? Type
    if (peek() != Token::kAnd) {
        return nullptr;
    }
    
    consume(); // consume '&'
    
    bool is_mutable = false;
    if (peek() == Token::kMut) {
        is_mutable = true;
        consume(); // consume 'mut'
    }
    
    auto type = parseType();
    if (!type) {
        throw std::runtime_error("parse failed! Expected type after reference");
    }
    
    return std::make_shared<ReferenceType>(is_mutable, std::move(type));
}

std::shared_ptr<ArrayType> Parser::parseArrayType() {
    // ArrayType → `[` Type `;` Expression `]`
    if (peek() != Token::kLSquare) {
        return nullptr;
    }
    
    consume(); // consume '['
    
    auto type = parseType();
    if (!type) {
        throw std::runtime_error("parse failed! Expected type in array type");
    }
    
    if (peek() != Token::kSemi) {
        throw std::runtime_error("parse failed! Expected ';' in array type");
    }
    consume(); // consume ';'
    
    auto expression = std::dynamic_pointer_cast<Expression>(parseExpression());
    if (!expression) {
        throw std::runtime_error("parse failed! Expected expression in array type");
    }
    
    if (peek() != Token::kRSquare) {
        throw std::runtime_error("parse failed! Expected ']' in array type");
    }
    consume(); // consume ']'
    
    return std::make_shared<ArrayType>(std::move(type), std::move(expression));
}

std::shared_ptr<UnitType> Parser::parseUnitType() {
    // UnitType → `(` `)`
    if (peek() != Token::kLParenthese) {
        return nullptr;
    }
    
    consume(); // consume '('
    
    if (peek() != Token::kRParenthese) {
        throw std::runtime_error("parse failed! Expected ')' for unit type");
    }
    consume(); // consume ')'
    
    return std::make_shared<UnitType>();
}

std::shared_ptr<PathInExpression> Parser::parsePathInExpression() {
    std::shared_ptr<PathIdentSegment> segment1, segment2;
    segment1 = std::move(parsePathIdentSegment());
    if (peek() == Token::kPathSep) {
        consume();
        segment2 = std::move(parsePathIdentSegment());
    } else {
        segment2 = nullptr;
    }
    return std::make_shared<PathInExpression>(std::move(segment1), std::move(segment2));
}

std::shared_ptr<PathIdentSegment> Parser::parsePathIdentSegment() {
    // std::cerr << "PathIdentSegment:" << std::endl;
    // std::cerr << pos << std::endl;
    if (peek() == Token::kIdentifier) {
        std::string identifier = get_string();
        // std::cerr << "IDENTIFIER: " << identifier << std::endl;
        consume();
        return std::make_shared<PathIdentSegment>(0, std::move(identifier));
    } else if (peek() == Token::kSelf) {
        consume();
        return std::make_shared<PathIdentSegment>(1, "self");
    } else if (peek() == Token::kSelf_) {
        consume();
        return std::make_shared<PathIdentSegment>(2, "Self");
    } else {
        throw std::runtime_error("parse failed! Unexpected token in path ident segment");
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
    // std::cerr << "Condition:" << std::endl;
    // std::cerr << pos << ' ' << tokenToString(peek()) << std::endl;
    match(Token::kLParenthese);
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
    match(Token::kRParenthese);
    return std::make_shared<Condition>(std::move(expression));
}

std::shared_ptr<IfExpression> Parser::parseIfExpression() {
    // std::cerr << "IfExpression!" << std::endl;
    std::shared_ptr<Condition> condition;
    std::shared_ptr<BlockExpression> then_block;
    std::shared_ptr<Expression> else_branch = nullptr;
    
    match(Token::kIf);

    // std::cerr << "?" << std::endl;
    
    // Parse condition
    condition = std::move(parseCondition());

    // std::cerr << "Condition matched!" << std::endl;
    
    // Parse then block
    then_block = std::move(parseBlockExpression());

    // std::cerr << "BlockExpression matched!" << std::endl;
    
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
    // std::cerr << "ReturnExpression!" << std::endl;
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

// Array and grouped expressions
std::shared_ptr<GroupedExpression> Parser::parseGroupedExpression() {
    std::shared_ptr<Expression> expression;
    
    match(Token::kLParenthese);
    expression = std::move(parseExpression());
    match(Token::kRParenthese);
    
    return std::make_shared<GroupedExpression>(std::move(expression));
}

std::shared_ptr<ArrayExpression> Parser::parseArrayExpression() {
    std::shared_ptr<ArrayElements> array_elements = nullptr;
    
    match(Token::kLSquare);
    
    // Check if there are elements inside the array
    if (peek() != Token::kRSquare) {
        array_elements = std::move(parseArrayElements());
    }
    
    match(Token::kRSquare);
    
    return std::make_shared<ArrayExpression>(std::move(array_elements));
}

std::shared_ptr<ArrayElements> Parser::parseArrayElements() {
    std::vector<std::shared_ptr<Expression>> expressions;
    
    // Parse first expression
    expressions.push_back(std::move(parseExpression()));
    
    // Check if this is semicolon separated or comma separated
    if (peek() == Token::kSemi) {
        // Semicolon separated: Expression ; Expression
        match(Token::kSemi);
        expressions.push_back(std::move(parseExpression()));
        
        // Optional trailing comma
        // if (peek() == Token::kComma) {
        //     consume();
        // }
        
        return std::make_shared<ArrayElements>(std::move(expressions), true);
    } else {
        // Comma separated: Expression ( , Expression )* ,?
        while (peek() == Token::kComma) {
            consume();
            if (peek() == Token::kRSquare) break;
            expressions.push_back(std::move(parseExpression()));
        }
        
        return std::make_shared<ArrayElements>(std::move(expressions), false);
    }
}

std::shared_ptr<IndexExpression> Parser::parseIndexExpression() {
    std::shared_ptr<Expression> base_expression;
    std::shared_ptr<Expression> index_expression;
    
    // Parse the base expression
    base_expression = std::move(parseExpression());
    
    // Parse the index part [ Expression ]
    match(Token::kLSquare);
    index_expression = std::move(parseExpression());
    match(Token::kRSquare);
    
    return std::make_shared<IndexExpression>(std::move(base_expression), std::move(index_expression));
}

std::shared_ptr<IndexExpression> Parser::parseIndexExpressionFromInfix(std::shared_ptr<Expression> lhs) {
    std::shared_ptr<Expression> index_expression;
    
    // Parse the index part [ Expression ]
    match(Token::kLSquare);
    index_expression = std::move(parseExpression());
    match(Token::kRSquare);
    
    return std::make_shared<IndexExpression>(std::move(lhs), std::move(index_expression));
}

// Struct expressions
std::shared_ptr<StructExpression> Parser::parseStructExpression() {
    std::shared_ptr<PathInExpression> path_in_expression;
    std::shared_ptr<StructExprFields> struct_expr_fields = nullptr;
    
    // Parse the path
    path_in_expression = std::move(parsePathInExpression());
    
    // Parse the struct fields
    match(Token::kLCurly);
    if (peek() != Token::kRCurly) {
        struct_expr_fields = std::move(parseStructExprFields());
    }
    match(Token::kRCurly);
    
    return std::make_shared<StructExpression>(std::move(path_in_expression), std::move(struct_expr_fields));
}

std::shared_ptr<StructExprFields> Parser::parseStructExprFields() {
    std::vector<std::shared_ptr<StructExprField>> struct_expr_fields;
    
    // Parse first field
    struct_expr_fields.push_back(std::move(parseStructExprField()));
    
    // Parse remaining fields
    while (peek() == Token::kComma) {
        consume();
        if (peek() == Token::kRCurly) {
            // Handle trailing comma case
            break;
        }
        struct_expr_fields.push_back(std::move(parseStructExprField()));
    }
    
    return std::make_shared<StructExprFields>(std::move(struct_expr_fields));
}

std::shared_ptr<StructExprField> Parser::parseStructExprField() {
    std::string identifier;
    std::shared_ptr<Expression> expression;
    
    // Parse identifier
    if (peek() == Token::kIdentifier) {
        identifier = get_string();
        consume();
    } else {
        throw std::runtime_error("parse failed! Expected identifier in struct field");
    }
    
    // Parse colon and expression
    match(Token::kColon);
    expression = std::move(parseExpression());
    
    return std::make_shared<StructExprField>(std::move(identifier), std::move(expression));
}

// Call expressions
std::shared_ptr<CallExpression> Parser::parseCallExpression() {
    std::shared_ptr<Expression> expression;
    std::shared_ptr<CallParams> call_params = nullptr;
    
    // Parse the expression (function to call)
    expression = std::move(parseExpression());
    
    // Parse the call parameters
    match(Token::kLParenthese);
    if (peek() != Token::kRParenthese) {
        call_params = std::move(parseCallParams());
    }
    match(Token::kRParenthese);
    
    return std::make_shared<CallExpression>(std::move(expression), std::move(call_params));
}

std::shared_ptr<CallExpression> Parser::parseCallExpressionFromInfix(std::shared_ptr<Expression> lhs) {
    // std::cerr << "CallExpressionFromInfix:" << std::endl;
    std::shared_ptr<CallParams> call_params = nullptr;
    
    // Parse the call parameters
    match(Token::kLParenthese);
    // std::cerr << "( matched!" << std::endl;
    if (peek() != Token::kRParenthese) {
        call_params = std::move(parseCallParams());
    }
    match(Token::kRParenthese);
    // std::cerr << "CallExpressionFromInfix done!" << std::endl;
    return std::make_shared<CallExpression>(std::move(lhs), std::move(call_params));
}

std::shared_ptr<CallParams> Parser::parseCallParams() {
    std::vector<std::shared_ptr<Expression>> expressions;
    
    // Parse first expression
    expressions.push_back(std::move(parseExpression()));
    
    // Parse remaining expressions
    while (peek() == Token::kComma) {
        consume();
        if (peek() == Token::kRParenthese) {
            // Handle trailing comma case
            break;
        }
        expressions.push_back(std::move(parseExpression()));
    }
    
    return std::make_shared<CallParams>(std::move(expressions));
}

// Method call, field, and path expressions
std::shared_ptr<MethodCallExpression> Parser::parseMethodCallExpression() {
    std::shared_ptr<Expression> expression;
    std::shared_ptr<PathIdentSegment> path_ident_segment;
    std::shared_ptr<CallParams> call_params = nullptr;
    
    // Parse the base expression
    expression = std::move(parseExpression());
    
    // Parse the method call part: . PathIdentSegment ( CallParams? )
    match(Token::kDot);
    path_ident_segment = std::move(parsePathIdentSegment());
    
    match(Token::kLParenthese);
    if (peek() != Token::kRParenthese) {
        call_params = std::move(parseCallParams());
    }
    match(Token::kRParenthese);
    
    return std::make_shared<MethodCallExpression>(std::move(expression), std::move(path_ident_segment), std::move(call_params));
}

std::shared_ptr<MethodCallExpression> Parser::parseMethodCallExpressionFromInfix(std::shared_ptr<Expression> lhs) {
    std::shared_ptr<PathIdentSegment> path_ident_segment;
    std::shared_ptr<CallParams> call_params = nullptr;
    
    // Parse the method call part: . PathIdentSegment ( CallParams? )
    match(Token::kDot);
    path_ident_segment = std::move(parsePathIdentSegment());
    
    match(Token::kLParenthese);
    if (peek() != Token::kRParenthese) {
        call_params = std::move(parseCallParams());
    }
    match(Token::kRParenthese);
    
    return std::make_shared<MethodCallExpression>(std::move(lhs), std::move(path_ident_segment), std::move(call_params));
}

std::shared_ptr<FieldExpression> Parser::parseFieldExpression() {
    std::shared_ptr<Expression> expression;
    std::string identifier;
    
    // Parse the base expression
    expression = std::move(parseExpression());
    
    // Parse the field access part: . IDENTIFIER
    match(Token::kDot);
    if (peek() == Token::kIdentifier) {
        identifier = get_string();
        consume();
    } else {
        throw std::runtime_error("parse failed! Expected identifier in field expression");
    }
    
    return std::make_shared<FieldExpression>(std::move(expression), std::move(identifier));
}

std::shared_ptr<FieldExpression> Parser::parseFieldExpressionFromInfix(std::shared_ptr<Expression> lhs) {
    std::string identifier;
    
    // Parse the field access part: . IDENTIFIER
    match(Token::kDot);
    if (peek() == Token::kIdentifier) {
        identifier = get_string();
        consume();
    } else {
        throw std::runtime_error("parse failed! Expected identifier in field expression");
    }
    
    return std::make_shared<FieldExpression>(std::move(lhs), std::move(identifier));
}

std::shared_ptr<PathExpression> Parser::parsePathExpression() {
    std::shared_ptr<PathInExpression> path_in_expression;
    
    // Parse the path in expression
    path_in_expression = std::move(parsePathInExpression());
    
    return std::make_shared<PathExpression>(std::move(path_in_expression));
}

// Unary expressions
std::shared_ptr<UnaryExpression> Parser::parseUnaryExpression() {
    Token token = peek();
    UnaryExpression::UnaryType type;
    
    switch (token) {
        case Token::kMinus:
            type = UnaryExpression::MINUS;
            break;
        case Token::kNot:
            type = UnaryExpression::NOT;
            break;
        case Token::kQuestion:
            type = UnaryExpression::TRY;
            break;
        default:
            throw std::runtime_error("parse failed! Expected unary operator");
    }
    
    consume();
    auto expression = std::dynamic_pointer_cast<Expression>(parsePrattExpression(getTokenUnaryBP(token)));
    
    return std::make_shared<UnaryExpression>(type, std::move(expression));
}

std::shared_ptr<BorrowExpression> Parser::parseBorrowExpression() {
    // std::cerr << "BorrowExpression:" << std::endl;
    // std::cerr << pos << std::endl;
    bool is_double = false;
    bool is_mutable = false;
    
    // Check for double borrow (&&)
    if (peek() == Token::kAnd) {
        consume();
        if (peek() == Token::kAnd) {
            is_double = true;
            consume();
        }
    } else {
        throw std::runtime_error("parse failed! Expected & for borrow expression");
    }
    
    // Check for mut keyword
    if (peek() == Token::kMut) {
        is_mutable = true;
        consume();
    }
    
    auto expression = std::dynamic_pointer_cast<Expression>(parsePrattExpression(getTokenUnaryBP(Token::kAnd)));
    
    return std::make_shared<BorrowExpression>(is_double, is_mutable, std::move(expression));
}

std::shared_ptr<DereferenceExpression> Parser::parseDereferenceExpression() {
    if (peek() != Token::kStar) {
        throw std::runtime_error("parse failed! Expected * for dereference expression");
    }
    
    consume();
    auto expression = std::dynamic_pointer_cast<Expression>(parsePrattExpression(getTokenLeftBP(Token::kStar)));
    
    return std::make_shared<DereferenceExpression>(std::move(expression));
}

// Binary and assignment expressions
std::shared_ptr<AssignmentExpression> Parser::parseAssignmentExpression(std::shared_ptr<Expression> lhs, std::shared_ptr<Expression> rhs) {
    return std::make_shared<AssignmentExpression>(std::move(lhs), std::move(rhs));
}

std::shared_ptr<CompoundAssignmentExpression> Parser::parseCompoundAssignmentExpression(CompoundAssignmentExpression::CompoundAssignmentType type, std::shared_ptr<Expression> lhs, std::shared_ptr<Expression> rhs) {
    return std::make_shared<CompoundAssignmentExpression>(type, std::move(lhs), std::move(rhs));
}

std::shared_ptr<BinaryExpression> Parser::parseBinaryExpression(BinaryExpression::BinaryType type, std::shared_ptr<Expression> lhs, std::shared_ptr<Expression> rhs) {
    return std::make_shared<BinaryExpression>(type, std::move(lhs), std::move(rhs));
}

// Type cast expression
std::shared_ptr<TypeCastExpression> Parser::parseTypeCastExpression(std::shared_ptr<Expression> expression, std::shared_ptr<Type> type) {
    return std::make_shared<TypeCastExpression>(std::move(expression), std::move(type));
}
