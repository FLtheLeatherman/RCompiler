#pragma once

#include "parser/visitor.hpp"
#include "parser/astnode.hpp"
#include "semantic/const_value.hpp"
#include "semantic/scope.hpp"

class ConstEvaluator : ASTVisitor {
private:
    std::shared_ptr<Scope> current_scope;
    std::shared_ptr<Scope> root_scope;
public:
    ConstEvaluator(std::shared_ptr<Scope> root_scope);
    ~ConstEvaluator() = default;
    
    // 辅助函数
    std::shared_ptr<ConstValue> createConstValueFromExpression(std::shared_ptr<ASTNode> expression);
    
    void visit(Crate&) override;

    void visit(ConstantItem&) override;

    void visit(ArrayType&) override;
};