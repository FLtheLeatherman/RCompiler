#pragma once

#include "astnode.hpp"

class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;
    void visit(ASTNode&);
};