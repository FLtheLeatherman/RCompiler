#pragma once

#include "parser/utils.hpp"

class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;
    
    // 基础访问方法
    virtual void visit(ASTNode&) {}
};