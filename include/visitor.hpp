#pragma once

#include "utils.hpp"

class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;
    void visit(ASTNode&);
};