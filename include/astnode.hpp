#pragma once

#include <vector>
#include <memory>
#include "utils.hpp"
#include "visitor.hpp"

class ASTNode {
public:
    ASTNode() = default;
    virtual ~ASTNode() = default;
    virtual void accept(ASTVisitor*) = 0;
};

class Crate : public ASTNode {
private:
    std::vector<std::shared_ptr<Item>> items;
public:
    Crate(std::vector<std::shared_ptr<Item>>&& items)
        : items(std::move(items)) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class Item : public ASTNode {
private:
    std::shared_ptr<ASTNode> item; // Function, Struct, Enumeration, ConstantItem, Trait, Implementation
public:
    Item(std::shared_ptr<ASTNode> item)
        : item(std::move(item)) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class Function : public ASTNode {
private:
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
private:
    std::shared_ptr<StructStruct> struct_struct;
public:
    Struct(std::shared_ptr<StructStruct> struct_struct)
        : struct_struct(std::move(struct_struct)) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class Enumeration : public ASTNode {
private:
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
private:
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
private:
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
private:
    std::shared_ptr<ASTNode> impl; // InherentImpl, TraitImpl
public:
    Implementation(std::shared_ptr<ASTNode> impl) : impl(std::move(impl)) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class FunctionParameters : public ASTNode {
private:
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
private:
    std::shared_ptr<ASTNode> child; // ShorthandSelf, TypedSelf
public:
    SelfParam(std::shared_ptr<ASTNode> child)
        : child(std::move(child)) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class ShorthandSelf : public ASTNode {
private:
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
private:
    bool is_mutable;
    std::shared_ptr<Type> type;
public:
    TypedSelf(bool is_mutable, std::shared_ptr<Type> type) 
        : is_mutable(is_mutable), type(std::move(type)) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class FunctionParam : public ASTNode {
private:
    std::shared_ptr<PatternNoTopAlt> pattern_no_top_alt;
    std::shared_ptr<Type> type;
public:
    FunctionParam(std::shared_ptr<PatternNoTopAlt> pattern_no_top_alt, std::shared_ptr<Type> type)
        : pattern_no_top_alt(std::move(pattern_no_top_alt)), type(std::move(type)) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class FunctionReturnType : public ASTNode {
private:
    std::shared_ptr<Type> type;
public:
    FunctionReturnType(std::shared_ptr<Type> type) 
        : type(std::move(type)) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class StructStruct : public ASTNode {
private:
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
private:
    std::vector<std::shared_ptr<StructField>> struct_fields;
public:
    StructFields(std::vector<std::shared_ptr<StructField>> struct_fields)
        : struct_fields(std::move(struct_fields)) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class StructField : public ASTNode {
private:
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
private:
    std::vector<std::shared_ptr<EnumVariant>> enum_variant;
public:
    EnumVariants(std::vector<std::shared_ptr<EnumVariant>> enum_variant)
        : enum_variant(std::move(enum_variant)) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class EnumVariant : public ASTNode {
private:
    std::string identifier;
public:
    EnumVariant(std::string identifier)
        : identifier(identifier) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }    
};

class AssociatedItem : public ASTNode {
private:
    std::shared_ptr<ASTNode> child; // ConstantItem, Function
public:
    AssociatedItem(std::shared_ptr<ASTNode> child)
        : child(std::move(child)) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class InherentImpl : public ASTNode {
private:
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
private:
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
private:
    std::shared_ptr<ASTNode> child; // nullptr(,), Item, LetStatement, ExpressionStatement
public:
    Statement(std::shared_ptr<ASTNode> child)
        : child(std::move(child)) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class LetStatement : public ASTNode {
private:
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
private:
    std::shared_ptr<ASTNode> child; // ExpressionWithoutBlock, ExpressionWithBlock
public:
    ExpressionStatement(std::shared_ptr<ASTNode> child)
        : child(std::move(child)) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class Expression : public ASTNode {
private:
    std::shared_ptr<ASTNode> child; // ExpressionWithoutBlock, ExpressionWithBlock
public:
    Expression(std::shared_ptr<ASTNode> child)
        : child(std::move(child)) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class ExpressionWithoutBlock : public ASTNode {
private:
    std::shared_ptr<ASTNode> child; 
    // LiteralExpression, PathExpression, OperatorExpression, GroupedExpression,
    // ArrayExpression, IndexExpression, StructExpression, CallExpression,
    // MethodCallExpression, FieldExpression, ContinueExpression, BreakExpression, ReturnExpression.
public:
    ExpressionWithoutBlock(std::shared_ptr<ASTNode> child)
        : child(std::move(child)) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class ExpressionWithBlock : public ASTNode {
private:
    std::shared_ptr<ASTNode> child;
public:
    ExpressionWithBlock(std::shared_ptr<ASTNode> child)
        : child(std::move(child)) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class LiteralExpression : public ASTNode {
public:
    LiteralExpression() {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class PathExpression : public ASTNode {
public:
    PathExpression() {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class OperatorExpression : public ASTNode {
public:
    OperatorExpression() {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class GroupedExpression : public ASTNode {
public:
    GroupedExpression() {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class ArrayExpression : public ASTNode {
public:
    ArrayExpression() {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class IndexExpression : public ASTNode {
public:
    IndexExpression() {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class StructExpression : public ASTNode {
public:
    StructExpression() {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class CallExpression : public ASTNode {
public:
    CallExpression() {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class MethodCallExpression : public ASTNode {
public:
    MethodCallExpression() {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class FieldExpression : public ASTNode {
public:
    FieldExpression() {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class ContinueExpression : public ASTNode {
public:
    ContinueExpression() {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class BreakExpression : public ASTNode {
public:
    BreakExpression() {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class ReturnExpression : public ASTNode {
public:
    ReturnExpression() {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class BlockExpression : public ASTNode {
public:
    BlockExpression() {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class LoopExpression : public ASTNode {
public:
    LoopExpression() {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class IfExpression : public ASTNode {
public:
    IfExpression() {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class PatternNoTopAlt : public ASTNode {
public:
    PatternNoTopAlt() {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class Type : public ASTNode {
private:
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
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class ArrayType : public ASTNode {
public:
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
private:
    std::shared_ptr<PathIdentSegment> segment1, segment2;
public:
    PathInExpression(std::shared_ptr<PathIdentSegment> segment1, std::shared_ptr<PathIdentSegment> segment2)
        : segment1(std::move(segment1)), segment2(segment2) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class PathIdentSegment: public ASTNode {
private:
    int type; // 0 for identifier, 1 for Self, 2 for self
    std::string identifier;
public:
    PathIdentSegment(int type, std::string identifier)
        : type(type), identifier(std::move(identifier)) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};