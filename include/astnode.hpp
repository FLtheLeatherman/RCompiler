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
    std::unique_ptr<ASTNode> item; // function, struct, enumeration, constantitem, trait, implementation
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
public:
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};
    
class ConstantItem : public ASTNode {
public:
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class Trait : public ASTNode {
public:
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class Implementation : public ASTNode {
public:
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

class FunctionReturnType : public ASTNode {
public:
    FunctionReturnType() {}
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

class Type : public ASTNode {
public:
    Type() {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};