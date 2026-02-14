#include "ir/ir_generator.hpp"

IRGenerator::IRGenerator(Scope *root_scope, llvm::IRBuilder *builder) : current_scope(root_scope), root_scope(root_scope), builder(builder) {}

void IRGenerator::visit(Crate& node) {
    for (auto item: node.items) {
        item->accept(this);
    }
}

void IRGenerator::visit(Item& node) {
    if (node.item) {
        node.item->accept(this);
    }
}

void IRGenerator::visit(Function& node) {
    auto prev_scope = current_scope;
    current_scope = (current_scope->getChild()).get();
    
    // 处理函数
    
    current_scope = prev_scope;
    current_scope->nextChild();
}

void IRGenerator::visit(Struct& node) {
    if (node.struct_struct) {
        node.struct_struct->accept(this);
    }
}

void IRGenerator::visit(Enumeration& node) {
    if (node.enum_variants) {
        node.enum_variants->accept(this);
    }
}

void IRGenerator::visit(ConstantItem& node) {
    if (node.expression) {
        node.expression->accept(this);
    }
}

void IRGenerator::visit(Trait& node) {
    auto prev_scope = current_scope;
    current_scope = (current_scope->getChild()).get();
    
    for (auto& item : node.associated_item) {
        if (item) {
            item->accept(this);
        }
    }

    current_scope = prev_scope;
    current_scope->nextChild();
}

void IRGenerator::visit(Implementation& node) {
    if (node.impl) {
        node.impl->accept(this);
    }
}

void IRGenerator::visit(InherentImpl& node) {
    auto prev_scope = current_scope;
    current_scope = (current_scope->getChild()).get();
    
    // 处理关联项
    for (auto& item : node.associated_item) {
        if (item) {
            item->accept(this);
        }
    }
    
    current_scope = prev_scope;
    current_scope->nextChild();
}

void IRGenerator::visit(TraitImpl& node) {
    auto prev_scope = current_scope;
    current_scope = (current_scope->getChild()).get();
    
    // 处理关联项
    for (auto& item : node.associated_item) {
        if (item) {
            item->accept(this);
        }
    }
    
    current_scope = prev_scope;
    current_scope->nextChild();
}

void IRGenerator::visit(AssociatedItem& node) {
    if (node.child) {
        node.child->accept(this);
    }
}

void IRGenerator::visit(FunctionParameters& node) {
    if (node.self_param) {
        node.self_param->accept(this);
    }
    for (auto& param : node.function_param) {
        if (param) {
            param->accept(this);
        }
    }
}

void IRGenerator::visit(SelfParam& node) {
    if (node.child) {
        node.child->accept(this);
    }
}

void IRGenerator::visit(ShorthandSelf& node) {
    
}

void IRGenerator::visit(TypedSelf& node) {
    if (node.type) {
        node.type->accept(this);
    }
}

void IRGenerator::visit(FunctionParam& node) {
    if (node.type) {
        node.type->accept(this);
    }
    if (node.pattern_no_top_alt) {
        node.pattern_no_top_alt->accept(this);
    }
}

void IRGenerator::visit(FunctionReturnType& node) {
    if (node.type) {
        node.type->accept(this);
    }
}

void IRGenerator::visit(StructStruct& node) {
    if (node.struct_fields) {
        node.struct_fields->accept(this);
    }
}

void IRGenerator::visit(StructFields& node) {
    for (auto& field : node.struct_fields) {
        if (field) {
            field->accept(this);
        }
    }
}

void IRGenerator::visit(StructField& node) {
    if (node.type) {
        node.type->accept(this);
    }
}

void IRGenerator::visit(EnumVariants& node) {
    for (auto& variant : node.enum_variant) {
        if (variant) {
            variant->accept(this);
        }
    }
}

void IRGenerator::visit(EnumVariant& node) {

}

void IRGenerator::visit(Statement& node) {
    if (node.child) {
        node.child->accept(this);
    }
}

void IRGenerator::visit(LetStatement& node) {
    if (node.type) {
        node.type->accept(this);
    }
    if (node.expression) {
        node.expression->accept(this);
    }
    if (node.pattern_no_top_alt) {
        node.pattern_no_top_alt->accept(this);
    }
}

void IRGenerator::visit(ExpressionStatement& node) {
    if (node.child) {
        node.child->accept(this);
    }
}

void IRGenerator::visit(Statements& node) {
    for (auto& stmt : node.statements) {
        if (stmt) {
            stmt->accept(this);
        }
    }
}

void IRGenerator::visit(Expression& node) {
    if (node.child) {
        node.child->accept(this);
    }
}

void IRGenerator::visit(ExpressionWithoutBlock& node) {
    if (node.child) {
        node.child->accept(this);
    }
}

void IRGenerator::visit(ExpressionWithBlock& node) {
    if (node.child) {
        node.child->accept(this);
    }
}

void IRGenerator::visit(CharLiteral& node) {

}

void IRGenerator::visit(StringLiteral& node) {

}

void IRGenerator::visit(RawStringLiteral& node) {

}

void IRGenerator::visit(CStringLiteral& node) {

}

void IRGenerator::visit(RawCStringLiteral& node) {

}

void IRGenerator::visit(IntegerLiteral& node) {

}

void IRGenerator::visit(BoolLiteral& node) {

}

void IRGenerator::visit(PathExpression& node) {
    if (node.path_in_expression) {
        node.path_in_expression->accept(this);
    }
}

void IRGenerator::visit(FieldExpression& node) {
    if (node.expression) {
        node.expression->accept(this);
    }
}

void IRGenerator::visit(UnaryExpression& node) {
    if (node.expression) {
        node.expression->accept(this);
    }
}

void IRGenerator::visit(BorrowExpression& node) {
    if (node.expression) {
        node.expression->accept(this);
    }
}

void IRGenerator::visit(DereferenceExpression& node) {
    if (node.expression) {
        node.expression->accept(this);
    }
}

void IRGenerator::visit(BinaryExpression& node) {
    if (node.lhs) {
        node.lhs->accept(this);
    }
    if (node.rhs) {
        node.rhs->accept(this);
    }
}

void IRGenerator::visit(AssignmentExpression& node) {
    if (node.lhs) {
        node.lhs->accept(this);
    }
    if (node.rhs) {
        node.rhs->accept(this);
    }
}

void IRGenerator::visit(CompoundAssignmentExpression& node) {
    if (node.lhs) {
        node.lhs->accept(this);
    }
    if (node.rhs) {
        node.rhs->accept(this);
    }
}

void IRGenerator::visit(TypeCastExpression& node) {
    if (node.expression) {
        node.expression->accept(this);
    }
    if (node.type) {
        node.type->accept(this);
    }
}

void IRGenerator::visit(CallExpression& node) {
    if (node.expression) {
        node.expression->accept(this);
    }
    if (node.call_params) {
        node.call_params->accept(this);
    }
}

void IRGenerator::visit(MethodCallExpression& node) {
    if (node.expression) {
        node.expression->accept(this);
    }
    if (node.call_params) {
        node.call_params->accept(this);
    }
}

void IRGenerator::visit(IndexExpression& node) {
    if (node.base_expression) {
        node.base_expression->accept(this);
    }
    if (node.index_expression) {
        node.index_expression->accept(this);
    }
}

void IRGenerator::visit(StructExpression& node) {
    if (node.path_in_expression) {
        node.path_in_expression->accept(this);
    }
    if (node.struct_expr_fields) {
        node.struct_expr_fields->accept(this);
    }
}

void IRGenerator::visit(ArrayExpression& node) {
    if (node.array_elements) {
        node.array_elements->accept(this);
    }
}

void IRGenerator::visit(GroupedExpression& node) {
    if (node.expression) {
        node.expression->accept(this);
    }
}

void IRGenerator::visit(BlockExpression& node) {
    auto prev_scope = current_scope;
    current_scope = (current_scope->getChild()).get();
    
    if (node.statements) {
        node.statements->accept(this);
    }
    
    current_scope = prev_scope;
    current_scope->nextChild();
}

void IRGenerator::visit(IfExpression& node) {
    if (node.condition) {
        node.condition->accept(this);
    }
    if (node.then_block) {
        node.then_block->accept(this);
    }
    if (node.else_branch) {
        node.else_branch->accept(this);
    }
}

void IRGenerator::visit(LoopExpression& node) {
    if (node.child) {
        node.child->accept(this);
    }
}

void IRGenerator::visit(InfiniteLoopExpression& node) {
    auto prev_scope = current_scope;
    current_scope = (current_scope->getChild()).get();

    if (node.block_expression) {
        node.block_expression->accept(this);
    }

    current_scope = prev_scope;
    current_scope->nextChild();
}

void IRGenerator::visit(PredicateLoopExpression& node) {
    auto prev_scope = current_scope;
    current_scope = (current_scope->getChild()).get();

    if (node.condition) {
        node.condition->accept(this);
    }
    if (node.block_expression) {
        node.block_expression->accept(this);
    }

    current_scope = prev_scope;
    current_scope->nextChild();
}

void IRGenerator::visit(BreakExpression& node) {
    if (node.expression) {
        node.expression->accept(this);
    }
}

void IRGenerator::visit(ContinueExpression& node) {

}

void IRGenerator::visit(ReturnExpression& node) {
    if (node.expression) {
        node.expression->accept(this);
    }
}

void IRGenerator::visit(Condition& node) {
    if (node.expression) {
        node.expression->accept(this);
    }
}

void IRGenerator::visit(ArrayElements& node) {
    for (auto& expr : node.expressions) {
        if (expr) {
            expr->accept(this);
        }
    }
}

void IRGenerator::visit(StructExprFields& node) {
    for (auto& field : node.struct_expr_fields) {
        if (field) {
            field->accept(this);
        }
    }
}

void IRGenerator::visit(StructExprField& node) {
    if (node.expression) {
        node.expression->accept(this);
    }
}

void IRGenerator::visit(CallParams& node) {
    for (auto& expr : node.expressions) {
        if (expr) {
            expr->accept(this);
        }
    }
}

void IRGenerator::visit(PatternNoTopAlt& node) {
    if (node.child) {
        node.child->accept(this);
    }
}

void IRGenerator::visit(IdentifierPattern& node) {

}

void IRGenerator::visit(ReferencePattern& node) {
    if (node.pattern) {
        node.pattern->accept(this);
    }
}

void IRGenerator::visit(Type& node) {
    if (node.child) {
        node.child->accept(this);
    }
}

void IRGenerator::visit(ReferenceType& node) {
    if (node.type) {
        node.type->accept(this);
    }
}

void IRGenerator::visit(ArrayType& node) {
    if (node.type) {
        node.type->accept(this);
    }
    if (node.expression) {
        node.expression->accept(this);
    }
}

void IRGenerator::visit(UnitType& node) {

}

void IRGenerator::visit(PathInExpression& node) {
    if (node.segment1) {
        node.segment1->accept(this);
    }
    if (node.segment2) {
        node.segment2->accept(this);
    }
}

void IRGenerator::visit(PathIdentSegment& node) {

}
