#pragma once

#include <memory>
#include "lexer.hpp"
#include "astnode.hpp"

class Parser {
private:
    enum BindingPower {
        PATH_ACCESS = 200,
        CALL_INDEX = 190,
        STRUCT_EXPR = 180,
        UNARY = 170,
        TYPE_CAST = 160,
        MUL_DIV_MOD = 150,
        ADD_SUB = 140,
        SHIFT = 130,
        BIT_AND = 120,
        BIT_XOR = 110,
        BIT_OR = 100,
        COMPARISON = 90,
        LOGIC_AND = 80,
        LOGIC_OR = 70,
        ASSIGNMENT = 60,
        FLOW_CONTROL = 50
    };

    std::vector<std::pair<Token, std::string>> tokens;
    size_t pos = 0;
public:
    Parser(std::vector<std::pair<Token, std::string>>&& tokens)
        : tokens(std::move(tokens)) {}

    Token peek();
    std::string get_string();
    void consume();
    void match(Token);

    // Pratt parsing functions
    int getTokenLeftBP(Token);
    int getTokenRightBP(Token);
    int getTokenUnaryBP(Token);
    std::shared_ptr<ASTNode> parsePrattExpression(int current_bp);
    std::shared_ptr<ASTNode> parsePrattPrefix();

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
    std::shared_ptr<Statements> parseStatements();
    std::shared_ptr<Expression> parseExpression();
    std::shared_ptr<ExpressionWithoutBlock> parseExpressionWithoutBlock();
    std::shared_ptr<ExpressionWithBlock> parseExpressionWithBlock();
    std::shared_ptr<BlockExpression> parseBlockExpression();

    std::shared_ptr<PatternNoTopAlt> parsePatternNoTopAlt();
    std::shared_ptr<IdentifierPattern> parseIdentifierPattern();
    std::shared_ptr<ReferencePattern> parseReferencePattern();

    std::shared_ptr<Type> parseType();
    std::shared_ptr<ReferenceType> parseReferenceType();
    std::shared_ptr<ArrayType> parseArrayType();
    std::shared_ptr<UnitType> parseUnitType();

    std::shared_ptr<PathInExpression> parsePathInExpression();
    std::shared_ptr<PathIdentSegment> parsePathIdentSegment();
    std::shared_ptr<CharLiteral> parseCharLiteral();
    std::shared_ptr<StringLiteral> parseStringLiteral();
    std::shared_ptr<RawStringLiteral> parseRawStringLiteral();
    std::shared_ptr<CStringLiteral> parseCStringLiteral();
    std::shared_ptr<RawCStringLiteral> parseRawCStringLiteral();
    std::shared_ptr<IntegerLiteral> parseIntegerLiteral();
    std::shared_ptr<BoolLiteral> parseBoolLiteral();
    std::shared_ptr<ReturnExpression> parseReturnExpression();
    std::shared_ptr<IfExpression> parseIfExpression();
    std::shared_ptr<Condition> parseCondition();
    
    // Loop expressions
    std::shared_ptr<LoopExpression> parseLoopExpression();
    std::shared_ptr<InfiniteLoopExpression> parseInfiniteLoopExpression();
    std::shared_ptr<PredicateLoopExpression> parsePredicateLoopExpression();
    std::shared_ptr<BreakExpression> parseBreakExpression();
    std::shared_ptr<ContinueExpression> parseContinueExpression();
    
    // Array and grouped expressions
    std::shared_ptr<GroupedExpression> parseGroupedExpression();
    std::shared_ptr<ArrayExpression> parseArrayExpression();
    std::shared_ptr<ArrayElements> parseArrayElements();
    std::shared_ptr<IndexExpression> parseIndexExpression();
    std::shared_ptr<IndexExpression> parseIndexExpressionFromInfix(std::shared_ptr<Expression> lhs);
    
    // Struct expressions
    std::shared_ptr<StructExpression> parseStructExpression();
    std::shared_ptr<StructExprFields> parseStructExprFields();
    std::shared_ptr<StructExprField> parseStructExprField();
    
    // Call expressions
    std::shared_ptr<CallExpression> parseCallExpression();
    std::shared_ptr<CallExpression> parseCallExpressionFromInfix(std::shared_ptr<Expression> lhs);
    std::shared_ptr<CallParams> parseCallParams();
    
    // Method call, field, and path expressions
    std::shared_ptr<MethodCallExpression> parseMethodCallExpression();
    std::shared_ptr<MethodCallExpression> parseMethodCallExpressionFromInfix(std::shared_ptr<Expression> lhs);
    std::shared_ptr<FieldExpression> parseFieldExpression();
    std::shared_ptr<FieldExpression> parseFieldExpressionFromInfix(std::shared_ptr<Expression> lhs);
    std::shared_ptr<PathExpression> parsePathExpression();
    
    // Unary expressions
    std::shared_ptr<UnaryExpression> parseUnaryExpression();
    std::shared_ptr<BorrowExpression> parseBorrowExpression();
    std::shared_ptr<DereferenceExpression> parseDereferenceExpression();
    
    // Binary and assignment expressions
    std::shared_ptr<AssignmentExpression> parseAssignmentExpression(std::shared_ptr<Expression> lhs, std::shared_ptr<Expression> rhs);
    std::shared_ptr<CompoundAssignmentExpression> parseCompoundAssignmentExpression(CompoundAssignmentExpression::CompoundAssignmentType type, std::shared_ptr<Expression> lhs, std::shared_ptr<Expression> rhs);
    std::shared_ptr<BinaryExpression> parseBinaryExpression(BinaryExpression::BinaryType type, std::shared_ptr<Expression> lhs, std::shared_ptr<Expression> rhs);
    
    // Type cast expression
    std::shared_ptr<TypeCastExpression> parseTypeCastExpression(std::shared_ptr<Expression> expression, std::shared_ptr<Type> type);
};