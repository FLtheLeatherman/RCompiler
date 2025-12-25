#pragma once

#include <vector>
#include <memory>
#include "parser/utils.hpp"
#include "parser/visitor.hpp"

// 前向声明
class ASTPrinter;

class ASTNode {
public:
    std::string type;
    ASTNode() = default;
    virtual ~ASTNode() = default;
    virtual void accept(ASTVisitor*) = 0;
};

class Crate : public ASTNode {
public:
    std::vector<std::shared_ptr<Item>> items;
public:
    Crate(std::vector<std::shared_ptr<Item>>&& items)
        : items(std::move(items)) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class Item : public ASTNode {
public:
    std::shared_ptr<ASTNode> item; // Function, Struct, Enumeration, ConstantItem, Trait, Implementation
public:
    Item(std::shared_ptr<ASTNode> item)
        : item(std::move(item)) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class Function : public ASTNode {
public:
    bool is_const;
    std::string identifier;
    std::shared_ptr<FunctionParameters> function_parameters;
    std::shared_ptr<FunctionReturnType> function_return_type;
    std::shared_ptr<BlockExpression> block_expression;
public:
    Function(bool is_const,
        std::string identifier,
        std::shared_ptr<FunctionParameters> function_parameters,
        std::shared_ptr<FunctionReturnType> function_return_type,
        std::shared_ptr<BlockExpression> block_expression)
        : is_const(is_const),
        identifier(std::move(identifier)),
        function_parameters(std::move(function_parameters)),
        function_return_type(std::move(function_return_type)),
        block_expression(std::move(block_expression)) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class Struct : public ASTNode {
public:
    std::shared_ptr<StructStruct> struct_struct;
public:
    Struct(std::shared_ptr<StructStruct> struct_struct)
        : struct_struct(std::move(struct_struct)) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class Enumeration : public ASTNode {
public:
    std::string identifier;
    std::shared_ptr<EnumVariants> enum_variants;
public:
    Enumeration(std::string identifier, std::shared_ptr<EnumVariants> enum_variants)
        : identifier(std::move(identifier)), enum_variants(std::move(enum_variants)) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};
    
class ConstantItem : public ASTNode {
public:
    std::string identifier;
    std::shared_ptr<Type> type;
    std::shared_ptr<Expression> expression;
public:
    ConstantItem(std::string identifier, std::shared_ptr<Type> type, std::shared_ptr<Expression> expression)
        : identifier(std::move(identifier)), type(std::move(type)), expression(std::move(expression)) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class Trait : public ASTNode {
public:
    std::string identifier;
    std::vector<std::shared_ptr<AssociatedItem>> associated_item;
public:
    Trait(std::string identifier, std::vector<std::shared_ptr<AssociatedItem>> associated_item)
        : identifier(std::move(identifier)), associated_item(std::move(associated_item)) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class Implementation : public ASTNode {
public:
    std::shared_ptr<ASTNode> impl; // InherentImpl, TraitImpl
public:
    Implementation(std::shared_ptr<ASTNode> impl) : impl(std::move(impl)) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class FunctionParameters : public ASTNode {
public:
    std::shared_ptr<SelfParam> self_param;
    std::vector<std::shared_ptr<FunctionParam>> function_param;
public:
    FunctionParameters(std::shared_ptr<SelfParam> self_param, std::vector<std::shared_ptr<FunctionParam>> function_param)
        : self_param(std::move(self_param)), function_param(std::move(function_param)) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class SelfParam : public ASTNode {
public:
    std::shared_ptr<ASTNode> child; // ShorthandSelf, TypedSelf
public:
    SelfParam(std::shared_ptr<ASTNode> child)
        : child(std::move(child)) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class ShorthandSelf : public ASTNode {
public:
    bool is_reference;
    bool is_mutable;
public:
    ShorthandSelf(bool is_reference, bool is_mutable)
        : is_reference(is_reference), is_mutable(is_mutable) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class TypedSelf : public ASTNode {
public:
    bool is_mutable;
    std::shared_ptr<Type> type;
public:
    TypedSelf(bool is_mutable, std::shared_ptr<Type> type)
        : is_mutable(is_mutable), type(std::move(type)) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
    
    friend class ASTPrinter;
};

class FunctionParam : public ASTNode {
public:
    std::shared_ptr<PatternNoTopAlt> pattern_no_top_alt;
    std::shared_ptr<Type> type;
public:
    FunctionParam(std::shared_ptr<PatternNoTopAlt> pattern_no_top_alt, std::shared_ptr<Type> type)
        : pattern_no_top_alt(std::move(pattern_no_top_alt)), type(std::move(type)) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
    
    friend class ASTPrinter;
};

class FunctionReturnType : public ASTNode {
public:
    std::shared_ptr<Type> type;
public:
    FunctionReturnType(std::shared_ptr<Type> type)
        : type(std::move(type)) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
    
    friend class ASTPrinter;
};

class StructStruct : public ASTNode {
public:
    std::string identifier;
    std::shared_ptr<StructFields> struct_fields;
public:
    StructStruct(std::string identifier, std::shared_ptr<StructFields> struct_fields)
        : identifier(std::move(identifier)), struct_fields(std::move(struct_fields)) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class StructFields : public ASTNode {
public:
    std::vector<std::shared_ptr<StructField>> struct_fields;
public:
    StructFields(std::vector<std::shared_ptr<StructField>> struct_fields)
        : struct_fields(std::move(struct_fields)) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class StructField : public ASTNode {
public:
    std::string identifier;
    std::shared_ptr<Type> type;
public:
    StructField(std::string identifier, std::shared_ptr<Type> type)
        : identifier(std::move(identifier)), type(std::move(type)){}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class EnumVariants : public ASTNode {
public:
    std::vector<std::shared_ptr<EnumVariant>> enum_variant;
public:
    EnumVariants(std::vector<std::shared_ptr<EnumVariant>> enum_variant)
        : enum_variant(std::move(enum_variant)) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class EnumVariant : public ASTNode {
public:
    std::string identifier;
public:
    EnumVariant(std::string identifier)
        : identifier(identifier) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }    
};

class AssociatedItem : public ASTNode {
public:
    std::shared_ptr<ASTNode> child; // ConstantItem, Function
public:
    AssociatedItem(std::shared_ptr<ASTNode> child)
        : child(std::move(child)) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class InherentImpl : public ASTNode {
public:
    std::shared_ptr<Type> type;
    std::vector<std::shared_ptr<AssociatedItem>> associated_item;
public:
    InherentImpl(std::shared_ptr<Type> type, std::vector<std::shared_ptr<AssociatedItem>> associated_item)
        : type(std::move(type)), associated_item(std::move(associated_item)) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};
class TraitImpl : public ASTNode {
public:
    std::string identifier;
    std::shared_ptr<Type> type;
    std::vector<std::shared_ptr<AssociatedItem>> associated_item;
public:
    TraitImpl(std::string identifier, std::shared_ptr<Type> type, std::vector<std::shared_ptr<AssociatedItem>> associated_item)
    : identifier(std::move(identifier)), type(std::move(type)), associated_item(std::move(associated_item)) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class Statement : public ASTNode {
public:
    std::shared_ptr<ASTNode> child; // nullptr(,), Item, LetStatement, ExpressionStatement
public:
    Statement(std::shared_ptr<ASTNode> child)
        : child(std::move(child)) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class LetStatement : public ASTNode {
public:
    std::shared_ptr<PatternNoTopAlt> pattern_no_top_alt;
    std::shared_ptr<Type> type;
    std::shared_ptr<Expression> expression;
public:
    LetStatement(std::shared_ptr<PatternNoTopAlt> pattern_no_top_alt, 
        std::shared_ptr<Type> type, 
        std::shared_ptr<Expression> expression)
        : pattern_no_top_alt(std::move(pattern_no_top_alt)),
        type(std::move(type)),
        expression(std::move(expression))  {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class ExpressionStatement : public ASTNode {
public:
    std::shared_ptr<ASTNode> child; // ExpressionWithoutBlock, ExpressionWithBlock
    bool has_semi;
public:
    ExpressionStatement(std::shared_ptr<ASTNode> child, bool has_semi)
        : child(std::move(child)), has_semi(has_semi) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class Statements : public ASTNode {
public:
    std::vector<std::shared_ptr<ASTNode>> statements; // Statement+ or Statement+ ExpressionWithoutBlock or ExpressionWithoutBlock
public:
    Statements(std::vector<std::shared_ptr<ASTNode>> statements)
        : statements(std::move(statements)) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class Expression : public ASTNode {
public:
    std::shared_ptr<ASTNode> child; // ExpressionWithoutBlock, ExpressionWithBlock
public:
    Expression() = default;
    Expression(std::shared_ptr<ASTNode> child)
        : child(std::move(child)) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class ExpressionWithoutBlock : public Expression {
public:
    std::shared_ptr<ASTNode> child;
    // LiteralExpression, PathExpression, OperatorExpression, GroupedExpression,
    // ArrayExpression, IndexExpression, StructExpression, CallExpression,
    // MethodCallExpression, FieldExpression, ContinueExpression, BreakExpression, ReturnExpression.
    ExpressionWithoutBlock(std::shared_ptr<ASTNode> child)
        : child(std::move(child)) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class ExpressionWithBlock : public Expression {
public:
    std::shared_ptr<ASTNode> child;
public:
    ExpressionWithBlock(std::shared_ptr<ASTNode> child)
        : child(std::move(child)) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};


class CharLiteral : public Expression {
public:
    std::string value;
public:
    CharLiteral(std::string value) : value(std::move(value)) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class StringLiteral : public Expression {
public:
    std::string value;
public:
    StringLiteral(std::string value) : value(std::move(value)) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class RawStringLiteral : public Expression {
public:
    std::string value;
public:
    RawStringLiteral(std::string value) : value(std::move(value)) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class CStringLiteral : public Expression {
public:
    std::string value;
public:
    CStringLiteral(std::string value) : value(std::move(value)) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class RawCStringLiteral : public Expression {
public:
    std::string value;
public:
    RawCStringLiteral(std::string value) : value(std::move(value)) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class IntegerLiteral : public Expression {
public:
    std::string value;
public:
    IntegerLiteral(std::string value) : value(std::move(value)) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class BoolLiteral : public Expression {
public:
    bool value;
public:
    BoolLiteral(bool value) : value(value) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class PathExpression : public Expression {
public:
    std::shared_ptr<PathInExpression> path_in_expression;
public:
    PathExpression(std::shared_ptr<PathInExpression> path_in_expression)
        : path_in_expression(std::move(path_in_expression)) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class OperatorExpression : public Expression {
public:
    OperatorExpression() {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class UnaryExpression : public Expression {
public:
    enum UnaryType {
        MINUS,      // -
        NOT,         // !
        TRY          // ?
    };
    
public:
    UnaryType type;
    std::shared_ptr<Expression> expression;
    
public:
    UnaryExpression(UnaryType type, std::shared_ptr<Expression> expression)
        : type(type), expression(std::move(expression)) {}
    
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class BorrowExpression : public Expression {
public:
    bool is_double;     // true for &&, false for &
    bool is_mutable;    // true if mut keyword present
    std::shared_ptr<Expression> expression;
    
public:
    BorrowExpression(bool is_double, bool is_mutable, std::shared_ptr<Expression> expression)
        : is_double(is_double), is_mutable(is_mutable), expression(std::move(expression)) {}
    
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class DereferenceExpression : public Expression {
public:
    std::shared_ptr<Expression> expression;
    
public:
    DereferenceExpression(std::shared_ptr<Expression> expression)
        : expression(std::move(expression)) {}
    
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class GroupedExpression : public Expression {
public:
    std::shared_ptr<Expression> expression;
public:
    GroupedExpression(std::shared_ptr<Expression> expression)
        : expression(std::move(expression)) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class ArrayExpression : public Expression {
public:
    std::shared_ptr<ArrayElements> array_elements;
public:
    ArrayExpression(std::shared_ptr<ArrayElements> array_elements)
        : array_elements(std::move(array_elements)) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class IndexExpression : public Expression {
public:
    std::shared_ptr<Expression> base_expression;
    std::shared_ptr<Expression> index_expression;
public:
    IndexExpression(std::shared_ptr<Expression> base_expression, std::shared_ptr<Expression> index_expression)
        : base_expression(std::move(base_expression)), index_expression(std::move(index_expression)) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class StructExpression : public Expression {
public:
    std::shared_ptr<PathInExpression> path_in_expression;
    std::shared_ptr<StructExprFields> struct_expr_fields;
public:
    StructExpression(std::shared_ptr<PathInExpression> path_in_expression, std::shared_ptr<StructExprFields> struct_expr_fields)
        : path_in_expression(std::move(path_in_expression)), struct_expr_fields(std::move(struct_expr_fields)) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class CallExpression : public Expression {
public:
    std::shared_ptr<Expression> expression;
    std::shared_ptr<CallParams> call_params;
public:
    CallExpression(std::shared_ptr<Expression> expression, std::shared_ptr<CallParams> call_params)
        : expression(std::move(expression)), call_params(std::move(call_params)) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class MethodCallExpression : public Expression {
public:
    std::shared_ptr<Expression> expression;
    std::shared_ptr<PathIdentSegment> path_ident_segment;
    std::shared_ptr<CallParams> call_params;
public:
    MethodCallExpression(std::shared_ptr<Expression> expression, std::shared_ptr<PathIdentSegment> path_ident_segment, std::shared_ptr<CallParams> call_params)
        : expression(std::move(expression)), path_ident_segment(std::move(path_ident_segment)), call_params(std::move(call_params)) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class FieldExpression : public Expression {
public:
    std::shared_ptr<Expression> expression;
    std::string identifier;
public:
    FieldExpression(std::shared_ptr<Expression> expression, std::string identifier)
        : expression(std::move(expression)), identifier(std::move(identifier)) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class ContinueExpression : public Expression {
public:
    ContinueExpression() {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class BreakExpression : public Expression {
public:
    std::shared_ptr<Expression> expression;
public:
    BreakExpression(std::shared_ptr<Expression> expression)
        : expression(std::move(expression)) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class ReturnExpression : public Expression {
public:
    std::shared_ptr<Expression> expression;
public:
    ReturnExpression(std::shared_ptr<Expression> expression)
        : expression(std::move(expression)) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class BlockExpression : public Expression {
public:
    std::shared_ptr<Statements> statements;
public:
    BlockExpression(std::shared_ptr<Statements> statements)
        : statements(std::move(statements)) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class LoopExpression : public Expression {
public:
    std::shared_ptr<ASTNode> child; // InfiniteLoopExpression or PredicateLoopExpression
public:
    LoopExpression(std::shared_ptr<ASTNode> child)
        : child(std::move(child)) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class InfiniteLoopExpression : public Expression {
public:
    std::shared_ptr<BlockExpression> block_expression;
public:
    InfiniteLoopExpression(std::shared_ptr<BlockExpression> block_expression)
        : block_expression(std::move(block_expression)) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class PredicateLoopExpression : public Expression {
public:
    std::shared_ptr<Condition> condition;
    std::shared_ptr<BlockExpression> block_expression;
public:
    PredicateLoopExpression(std::shared_ptr<Condition> condition, std::shared_ptr<BlockExpression> block_expression)
        : condition(std::move(condition)), block_expression(std::move(block_expression)) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class Condition : public Expression {
public:
    std::shared_ptr<Expression> expression; // Expression except StructExpression
public:
    Condition(std::shared_ptr<Expression> expression)
        : expression(std::move(expression)) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class IfExpression : public Expression {
public:
    std::shared_ptr<Condition> condition;
    std::shared_ptr<BlockExpression> then_block;
    std::shared_ptr<Expression> else_branch; // BlockExpression or IfExpression
public:
    IfExpression(std::shared_ptr<Condition> condition,
                std::shared_ptr<BlockExpression> then_block,
                std::shared_ptr<Expression> else_branch)
        : condition(std::move(condition)),
          then_block(std::move(then_block)),
          else_branch(std::move(else_branch)) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class PatternNoTopAlt : public ASTNode {
public:
    std::shared_ptr<ASTNode> child;
public:
    PatternNoTopAlt(std::shared_ptr<ASTNode> child) 
    : child(std::move(child)) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class IdentifierPattern : public ASTNode {
public:
    bool is_ref;
    bool is_mutable;
    std::string identifier;
public:
    IdentifierPattern(bool is_ref, bool is_mutable, std::string identifier)
        : is_ref(is_ref), is_mutable(is_mutable), identifier(std::move(identifier)) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class ReferencePattern : public ASTNode {
public:
    bool is_double;     // true for &&, false for &
    bool is_mutable;
    std::shared_ptr<PatternNoTopAlt> pattern;
public:
    ReferencePattern(bool is_double, bool is_mutable, std::shared_ptr<PatternNoTopAlt> pattern)
        : is_double(is_double), is_mutable(is_mutable), pattern(std::move(pattern)) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class Type : public ASTNode {
public:
    std::shared_ptr<ASTNode> child; // TypePath (PathIdentSegment), ReferenceType, ArrayType, UnitType
public:
    Type(std::shared_ptr<ASTNode> child)
        : child(std::move(child)) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class ReferenceType : public ASTNode {
public:
    bool is_mutable;
    std::shared_ptr<Type> type;
public:
    ReferenceType(bool is_mutable, std::shared_ptr<Type> type)
        : is_mutable(is_mutable), type(std::move(type)) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class ArrayType : public ASTNode {
public:
    std::shared_ptr<Type> type;
    std::shared_ptr<Expression> expression;
public:
    ArrayType(std::shared_ptr<Type> type, std::shared_ptr<Expression> expression)
        : type(std::move(type)), expression(std::move(expression)) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class UnitType : public ASTNode {
public:
    UnitType() {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class PathInExpression : public ASTNode {
public:
    std::shared_ptr<PathIdentSegment> segment1, segment2;
public:
    PathInExpression(std::shared_ptr<PathIdentSegment> segment1, std::shared_ptr<PathIdentSegment> segment2)
        : segment1(std::move(segment1)), segment2(std::move(segment2)) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class ArrayElements : public ASTNode {
public:
    std::vector<std::shared_ptr<Expression>> expressions;
    bool is_semicolon_separated; // true for semicolon separated, false for comma separated
public:
    ArrayElements(std::vector<std::shared_ptr<Expression>> expressions, bool is_semicolon_separated)
        : expressions(std::move(expressions)), is_semicolon_separated(is_semicolon_separated) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class StructExprFields : public ASTNode {
public:
    std::vector<std::shared_ptr<StructExprField>> struct_expr_fields;
public:
    StructExprFields(std::vector<std::shared_ptr<StructExprField>> struct_expr_fields)
        : struct_expr_fields(std::move(struct_expr_fields)) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class StructExprField : public ASTNode {
public:
    std::string identifier;
    std::shared_ptr<Expression> expression;
public:
    StructExprField(std::string identifier, std::shared_ptr<Expression> expression)
        : identifier(std::move(identifier)), expression(std::move(expression)) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class CallParams : public ASTNode {
public:
    std::vector<std::shared_ptr<Expression>> expressions;
public:
    CallParams(std::vector<std::shared_ptr<Expression>> expressions)
        : expressions(std::move(expressions)) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};


class PathIdentSegment: public ASTNode {
public:
    int type; // 0 for identifier, 1 for Self, 2 for self
    std::string identifier;
public:
    PathIdentSegment(int type, std::string identifier)
        : type(type), identifier(std::move(identifier)) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class AssignmentExpression : public Expression {
public:
    std::shared_ptr<Expression> lhs;
    std::shared_ptr<Expression> rhs;
public:
    AssignmentExpression(std::shared_ptr<Expression> lhs, std::shared_ptr<Expression> rhs)
        : lhs(std::move(lhs)), rhs(std::move(rhs)) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class CompoundAssignmentExpression : public Expression {
public:
    enum CompoundAssignmentType {
        PLUS_EQ,     // +=
        MINUS_EQ,    // -=
        STAR_EQ,     // *=
        SLASH_EQ,    // /=
        PERCENT_EQ,  // %=
        CARET_EQ,    // ^=
        AND_EQ,      // &=
        OR_EQ,       // |=
        SHL_EQ,      // <<=
        SHR_EQ       // >>=
    };
    
public:
    CompoundAssignmentType type;
    std::shared_ptr<Expression> lhs;
    std::shared_ptr<Expression> rhs;
    
public:
    CompoundAssignmentExpression(CompoundAssignmentType type, std::shared_ptr<Expression> lhs, std::shared_ptr<Expression> rhs)
        : type(type), lhs(std::move(lhs)), rhs(std::move(rhs)) {}
    
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class BinaryExpression : public Expression {
public:
    enum BinaryType {
        PLUS,        // +
        MINUS,       // -
        STAR,        // *
        SLASH,       // /
        PERCENT,     // %
        CARET,       // ^
        AND,         // &
        OR,          // |
        SHL,         // <<
        SHR,         // >>
        EQ_EQ,       // ==
        NE,          // !=
        GT,          // >
        LT,          // <
        GE,          // >=
        LE,          // <=
        AND_AND,     // &&
        OR_OR        // ||
    };
    
public:
    BinaryType type;
    std::shared_ptr<Expression> lhs;
    std::shared_ptr<Expression> rhs;
    
public:
    BinaryExpression(BinaryType type, std::shared_ptr<Expression> lhs, std::shared_ptr<Expression> rhs)
        : type(type), lhs(std::move(lhs)), rhs(std::move(rhs)) {}
    
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class TypeCastExpression : public Expression {
public:
    std::shared_ptr<Expression> expression;
    std::shared_ptr<Type> type;
public:
    TypeCastExpression(std::shared_ptr<Expression> expression, std::shared_ptr<Type> type)
        : expression(std::move(expression)), type(std::move(type)) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};