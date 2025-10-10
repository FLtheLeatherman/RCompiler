#pragma once

#include <vector>
#include <memory>
#include "visitor.hpp"

class ASTNode {
public:
    ASTNode() = default;
    virtual ~ASTNode() = default;
    virtual void accept(ASTVisitor*) = 0;
};

class Crate: ASTNode {
public:
    std::vector<std::unique_ptr<Item>> items;
    Crate(std::vector<std::unique_ptr<Item>>&& items)
        : items(std::move(items)) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class Item: ASTNode {
public:
    std::unique_ptr<ASTNode> item; // function, struct, enumeration, constantitem, trait, implementation
    Item(std::unique_ptr<ASTNode> item)
        : item(std::move(item)) {}
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class Function: ASTNode {
public:
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class Struct: ASTNode {
public:
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class Enumeration: ASTNode {
public:
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};
    
class ConstantItem: ASTNode {
public:
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class Trait: ASTNode {
public:
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};

class Implementation: ASTNode {
public:
    void accept(ASTVisitor* visitor) override {
        visitor->visit(*this);
    }
};