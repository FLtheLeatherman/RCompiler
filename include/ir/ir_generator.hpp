#pragma once

#include "parser/astnode.hpp"
#include "parser/visitor.hpp"
#include "semantic/scope.hpp"
#include "ir_builder.hpp"

namespace llvm {
class IRGenerator : public ASTVisitor {
private:
    Scope *current_scope;
    Scope *root_scope;
    IRBuilder *builder;
    LLVMContext *context;
public:
    IRGenerator(Scope *root_scope, IRBuilder *builder, LLVMContext *context);
    ~IRGenerator() = default;

    // 顶层节点
    void visit(Crate& node) override;
    void visit(Item& node) override;
};

}
