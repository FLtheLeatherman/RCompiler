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
    std::vector<std::unique_ptr<Item>> items;
public:
    Crate(std::vector<std::unique_ptr<Item>>&& items)
        : items(std::move(items)) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class Item : public ASTNode {
private:
    std::unique_ptr<ASTNode> item; // Function, Struct, Enumeration, ConstantItem, Trait, Implementation
public:
    Item(std::unique_ptr<ASTNode> item)
        : item(std::move(item)) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class Function : public ASTNode {
private:
    bool is_const;
    std::string identifier;
    std::unique_ptr<FunctionParameters> function_parameters;
    std::unique_ptr<FunctionReturnType> function_return_type;
    std::unique_ptr<BlockExpression> block_expression;
public:
    Function(bool is_const, std::string identifier, std::unique_ptr<FunctionParameters> function_parameters, std::unique_ptr<FunctionReturnType> function_return_type, std::unique_ptr<BlockExpression> block_expression)
        : is_const(is_const), identifier(std::move(identifier)), function_parameters(std::move(function_parameters)), function_return_type(std::move(function_return_type)), block_expression(std::move(block_expression)) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class Struct : public ASTNode {
private:
    std::unique_ptr<StructStruct> struct_struct;
public:
    Struct(std::unique_ptr<StructStruct> struct_struct)
        : struct_struct(std::move(struct_struct)) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class Enumeration : public ASTNode {
private:
    std::string identifier;
    std::unique_ptr<EnumVariants> enum_variants;
public:
    Enumeration(std::string identifier, std::unique_ptr<EnumVariants> enum_variants)
        : identifier(std::move(identifier)), enum_variants(std::move(enum_variants)) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};
    
class ConstantItem : public ASTNode {
private:
    std::string identifier;
    std::unique_ptr<Type> type;
    std::unique_ptr<Expression> expression;
public:
    ConstantItem(std::string identifier, std::unique_ptr<Type> type, std::unique_ptr<Expression> expression)
        : identifier(std::move(identifier)), type(std::move(type)), expression(std::move(expression)) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class Trait : public ASTNode {
private:
    std::string identifier;
    std::vector<std::unique_ptr<AssociatedItem>> associated_item;
public:
    Trait(std::string identifier, std::vector<std::unique_ptr<AssociatedItem>> associated_item)
        : identifier(std::move(identifier)), associated_item(std::move(associated_item)) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class Implementation : public ASTNode {
private:
    std::unique_ptr<ASTNode> impl; // InherentImpl, TraitImpl
public:
    Implementation(std::unique_ptr<ASTNode> impl) : impl(std::move(impl)) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class FunctionParameters : public ASTNode {
public:
    FunctionParameters() {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class SelfParam : public ASTNode {
public:
    SelfParam() {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class ShorthandSelf : public ASTNode {
public:
    ShorthandSelf() {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class TypedSelf : public ASTNode {
public:
    TypedSelf() {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class FunctionParam : public ASTNode {
public:
    FunctionParam() {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class FunctionReturnType : public ASTNode {
private:
    std::unique_ptr<Type> type;
public:
    FunctionReturnType(std::unique_ptr<Type> type) 
        : type(std::move(type)) {}
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

class StructStruct : public ASTNode {
private:
    std::string identifier;
    std::unique_ptr<StructFields> struct_fields;
public:
    StructStruct(std::string identifier, std::unique_ptr<StructFields> struct_fields)
        : identifier(std::move(identifier)), struct_fields(std::move(struct_fields)) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class StructFields : public ASTNode {
private:
    std::vector<std::unique_ptr<StructField>> struct_fields;
public:
    StructFields(std::vector<std::unique_ptr<StructField>> struct_fields)
        : struct_fields(std::move(struct_fields)) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class StructField : public ASTNode {
private:
    std::string identifier;
    std::unique_ptr<Type> type;
public:
    StructField(std::string identifier, std::unique_ptr<Type> type)
        : identifier(std::move(identifier)), type(std::move(type)){}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class EnumVariants : public ASTNode {
private:
    std::vector<std::unique_ptr<EnumVariant>> enum_variant;
public:
    EnumVariants(std::vector<std::unique_ptr<EnumVariant>> enum_variant)
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
    std::unique_ptr<ASTNode> child; // ConstantItem, Function
public:
    AssociatedItem(std::unique_ptr<ASTNode> child)
        : child(std::move(child)) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class InherentImpl : public ASTNode {
private:
    std::unique_ptr<Type> type;
    std::vector<std::unique_ptr<AssociatedItem>> associated_item;
public:
    InherentImpl(std::unique_ptr<Type> type, std::vector<std::unique_ptr<AssociatedItem>> associated_item)
        : type(std::move(type)), associated_item(std::move(associated_item)) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};
class TraitImpl : public ASTNode {
private:
    std::string identifier;
    std::unique_ptr<Type> type;
    std::vector<std::unique_ptr<AssociatedItem>> associated_item;
public:
    TraitImpl(std::string identifier, std::unique_ptr<Type> type, std::vector<std::unique_ptr<AssociatedItem>> associated_item)
    : identifier(std::move(identifier)), type(std::move(type)), associated_item(std::move(associated_item)) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class Type : public ASTNode {
public:
    Type() {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class Expression : public ASTNode {
public:
    Expression() {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};