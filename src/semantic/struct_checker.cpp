#include "semantic/struct_checker.hpp"
#include "semantic/const_value.hpp"
#include <iostream>
#include <stdexcept>

StructChecker::StructChecker(std::shared_ptr<Scope> root_scope) {
    this->current_scope = this->root_scope = root_scope;
}

void StructChecker::handleInherentImpl() {
    std::shared_ptr<StructSymbol> struct_symbol = current_scope->findStructSymbol(current_scope->getSelfType());
    if (struct_symbol == nullptr) {
        throw std::runtime_error("Undefined Name");
    }
    for (const auto& [name, const_symbol]: current_scope->getConstSymbols()) {
        struct_symbol->addAssociatedConst(const_symbol);
    }
    for (const auto& [name, func_symbol]: current_scope->getFuncSymbols()) {
        if (func_symbol->isMethod()) {
            struct_symbol->addMethod(func_symbol);
        } else {
            struct_symbol->addAssociatedFunction(func_symbol);
        }
    }
}

void StructChecker::handleTraitImpl(std::string identifier) {
    std::shared_ptr<StructSymbol> struct_symbol = current_scope->findStructSymbol(current_scope->getSelfType());
    std::shared_ptr<TraitSymbol> trait_symbol = current_scope->findTraitSymbol(identifier);
    if (struct_symbol == nullptr || trait_symbol == nullptr) {
        throw std::runtime_error("Undefined Name struct or trait not found");
    }

    auto const_symbols = trait_symbol->getConstSymbols();
    if (const_symbols.size() != trait_symbol->getConstSymbols().size()) {
        throw std::runtime_error("Undefined Name const size diff");
    }
    for (const auto& const_symbol: const_symbols) {
        if (!current_scope->constSymbolExists(const_symbol->getIdentifier())) {
            throw std::runtime_error("Undefined Name const not exists");
        }
    }
    auto func_symbols = trait_symbol->getAllAssociatedFunctions();
    if (func_symbols.size() != current_scope->getFuncSymbolCount()) {
        throw std::runtime_error("Undefined Name func size diff");
    }
    for (const auto& func_symbol: func_symbols) {
        auto impl_func = current_scope->getFuncSymbol(func_symbol->getIdentifier());
        if (impl_func->getMethodType() != func_symbol->getMethodType() || impl_func->isConst() != func_symbol->isConst()) {
            throw std::runtime_error("Undefined Name method or const diff");
        }
        if (func_symbol->getReturnType() != impl_func->getReturnType()) {
            throw std::runtime_error("Undefined Name return type diff");
        }
        auto trait_func_param = func_symbol->getParameters();
        auto impl_func_param = impl_func->getParameters();
        if (trait_func_param.size() != impl_func_param.size()) {
            throw std::runtime_error("Undefined Name param size diff");
        }
        for (size_t _ = 0; _ < impl_func_param.size(); ++_) {
            if (impl_func_param[_]->getType() != trait_func_param[_]->getType()) {
                // std::cout << impl_func_param[_]->getType() << ' ' << trait_func_param[_]->getType() << std::endl;
                throw std::runtime_error("Undefined Name param type diff");
            }
        }
    }

    for (const auto& [name, const_symbol]: current_scope->getConstSymbols()) {
        struct_symbol->addAssociatedConst(const_symbol);
    }
    for (const auto& [name, func_symbol]: current_scope->getFuncSymbols()) {
        if (func_symbol->isMethod()) {
            struct_symbol->addMethod(func_symbol);
        } else {
            struct_symbol->addAssociatedFunction(func_symbol);
        }
    }
}

void StructChecker::visit(Crate& node) {
    for (auto item: node.items) {
        item->accept(this);
    }
}

void StructChecker::visit(Item& node) {
    if (node.item) {
        node.item->accept(this);
    }
}

void StructChecker::visit(Function& node) {
    // std::cout << "visit function: " << node.identifier << std::endl;

    // std::cout << "GOOD" << std::endl;

    auto function_symbol = current_scope->getFuncSymbol(node.identifier);

    // std::cout << function_symbol->getReturnType() << std::endl;

    if (!checkTypeExists(current_scope, function_symbol->getReturnType())) {
        throw std::runtime_error("Undefined Name in function return type");
    }
    auto function_params = function_symbol->getParameters();
    for (auto function_param: function_params) {
        if (!checkTypeExists(current_scope, function_param->getType())) {
            throw std::runtime_error("Undefined Name in function params");
        }
    }

    // std::cout << "GOOD" << std::endl;
    
    auto prev_scope = current_scope;
    current_scope = current_scope->getChild();

    // std::cout << "GOOD" << std::endl;

    if (node.block_expression && current_scope) {
        node.block_expression->accept(this);
    }
    current_scope = prev_scope;
    current_scope->nextChild();
}

void StructChecker::visit(Struct& node) {
    // std::cout << "visit struct: " << std::endl;
    // struct 不会创建新的 scope，直接访问其内容
    if (node.struct_struct) {
        node.struct_struct->accept(this);
    }
}

void StructChecker::visit(Enumeration& node) {
    // std::cout << "visit enum: " << std::endl;
    // 检查枚举符号是否存在
    auto enum_symbol = current_scope->getEnumSymbol(node.identifier);
    if (!enum_symbol) {
        throw std::runtime_error("Undefined Name");
    }
    
    // 枚举不会创建新的 scope，直接访问其内容
    if (node.enum_variants) {
        node.enum_variants->accept(this);
    }
}

void StructChecker::visit(ConstantItem& node) {
    // std::cout << "visit constant item: " << node.identifier << std::endl;
    // 从当前作用域获取常量符号
    auto const_symbol = current_scope->getConstSymbol(node.identifier);
    if (!const_symbol) {
        throw std::runtime_error("Undefined Name");
    }
    
    // 检查常量类型是否存在
    if (!checkTypeExists(current_scope, const_symbol->getType())) {
        throw std::runtime_error("Undefined Name");
    }
}

void StructChecker::visit(Trait& node) {
    // std::cout << "visit trait: " << node.identifier << std::endl;
    // 检查 trait 符号是否存在
    auto trait_symbol = current_scope->getTraitSymbol(node.identifier);
    if (!trait_symbol) {
        throw std::runtime_error("Undefined Name");
    }
    
    // trait 会创建新的 scope，需要进入
    auto prev_scope = current_scope;
    current_scope = current_scope->getChild();
    
    // 处理关联项
    for (auto& item : node.associated_item) {
        if (item) {
            item->accept(this);
        }
    }
    
    current_scope = prev_scope;
    current_scope->nextChild();
}

void StructChecker::visit(Implementation& node) {
    // std::cout << "visit impl: " << std::endl;
    if (node.impl) {
        node.impl->accept(this);
    }
}

void StructChecker::visit(InherentImpl& node) {
    // std::cout << "visit inherent impl: " << std::endl;
    // 进入 impl 作用域
    auto prev_scope = current_scope;
    current_scope = current_scope->getChild();
    
    // 处理关联项
    for (auto& item : node.associated_item) {
        if (item) {
            item->accept(this);
        }
    }
    
    // 调用辅助函数处理 inherent impl
    handleInherentImpl();
    
    current_scope = prev_scope;
    current_scope->nextChild();
}

void StructChecker::visit(TraitImpl& node) {
    // std::cout << "visit trait impl: " << node.identifier << std::endl;
    // 进入 impl 作用域
    auto prev_scope = current_scope;
    current_scope = current_scope->getChild();
    
    // 处理关联项
    for (auto& item : node.associated_item) {
        if (item) {
            item->accept(this);
        }
    }
    
    // 调用辅助函数处理 trait impl
    handleTraitImpl(node.identifier);
    
    current_scope = prev_scope;
    current_scope->nextChild();
}

void StructChecker::visit(AssociatedItem& node) {
    // std::cout << "visit associated item: " << std::endl;
    if (node.child) {
        node.child->accept(this);
    }
}

// 函数相关节点
void StructChecker::visit(FunctionParameters& node) {
    if (node.self_param) {
        node.self_param->accept(this);
    }
    for (auto& param : node.function_param) {
        if (param) {
            param->accept(this);
        }
    }
}

void StructChecker::visit(SelfParam& node) {
    if (node.child) {
        node.child->accept(this);
    }
}

void StructChecker::visit(ShorthandSelf& node) {
    // ShorthandSelf 不包含类型信息，无需类型检查
}

void StructChecker::visit(TypedSelf& node) {
    if (node.type) {
        node.type->accept(this);
    }
}

void StructChecker::visit(FunctionParam& node) {
    if (node.type) {
        node.type->accept(this);
    }
    if (node.pattern_no_top_alt) {
        node.pattern_no_top_alt->accept(this);
    }
}

void StructChecker::visit(FunctionReturnType& node) {
    if (node.type) {
        node.type->accept(this);
    }
}

// 结构体相关节点
void StructChecker::visit(StructStruct& node) {
    if (node.struct_fields) {
        node.struct_fields->accept(this);
    }
}

void StructChecker::visit(StructFields& node) {
    for (auto& field : node.struct_fields) {
        if (field) {
            field->accept(this);
        }
    }
}

void StructChecker::visit(StructField& node) {
    if (node.type) {
        node.type->accept(this);
    }
}

// 枚举相关节点
void StructChecker::visit(EnumVariants& node) {
    for (auto& variant : node.enum_variant) {
        if (variant) {
            variant->accept(this);
        }
    }
}

void StructChecker::visit(EnumVariant& node) {
    // EnumVariant 不包含类型信息，无需类型检查
}

// 语句类节点
void StructChecker::visit(Statement& node) {
    if (node.child) {
        node.child->accept(this);
    }
}

void StructChecker::visit(LetStatement& node) {
    // 检查 let statement 的类型是否存在
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

void StructChecker::visit(ExpressionStatement& node) {
    if (node.child) {
        node.child->accept(this);
    }
}

void StructChecker::visit(Statements& node) {
    for (auto& stmt : node.statements) {
        if (stmt) {
            stmt->accept(this);
        }
    }
}

// 表达式类节点
void StructChecker::visit(Expression& node) {
    if (node.child) {
        node.child->accept(this);
    }
}

void StructChecker::visit(ExpressionWithoutBlock& node) {
    if (node.child) {
        node.child->accept(this);
    }
}

void StructChecker::visit(ExpressionWithBlock& node) {
    if (node.child) {
        node.child->accept(this);
    }
}

// 字面量表达式
void StructChecker::visit(CharLiteral& node) {
    // 字面量不包含类型信息，无需类型检查
}

void StructChecker::visit(StringLiteral& node) {
    // 字面量不包含类型信息，无需类型检查
}

void StructChecker::visit(RawStringLiteral& node) {
    // 字面量不包含类型信息，无需类型检查
}

void StructChecker::visit(CStringLiteral& node) {
    // 字面量不包含类型信息，无需类型检查
}

void StructChecker::visit(RawCStringLiteral& node) {
    // 字面量不包含类型信息，无需类型检查
}

void StructChecker::visit(IntegerLiteral& node) {
    // 字面量不包含类型信息，无需类型检查
}

void StructChecker::visit(BoolLiteral& node) {
    // 字面量不包含类型信息，无需类型检查
}

// 路径和访问表达式
void StructChecker::visit(PathExpression& node) {
    if (node.path_in_expression) {
        node.path_in_expression->accept(this);
    }
}

void StructChecker::visit(FieldExpression& node) {
    if (node.expression) {
        node.expression->accept(this);
    }
}

// 运算符表达式
void StructChecker::visit(UnaryExpression& node) {
    if (node.expression) {
        node.expression->accept(this);
    }
}

void StructChecker::visit(BorrowExpression& node) {
    if (node.expression) {
        node.expression->accept(this);
    }
}

void StructChecker::visit(DereferenceExpression& node) {
    if (node.expression) {
        node.expression->accept(this);
    }
}

void StructChecker::visit(BinaryExpression& node) {
    if (node.lhs) {
        node.lhs->accept(this);
    }
    if (node.rhs) {
        node.rhs->accept(this);
    }
}

void StructChecker::visit(AssignmentExpression& node) {
    if (node.lhs) {
        node.lhs->accept(this);
    }
    if (node.rhs) {
        node.rhs->accept(this);
    }
}

void StructChecker::visit(CompoundAssignmentExpression& node) {
    if (node.lhs) {
        node.lhs->accept(this);
    }
    if (node.rhs) {
        node.rhs->accept(this);
    }
}

void StructChecker::visit(TypeCastExpression& node) {
    if (node.expression) {
        node.expression->accept(this);
    }
    if (node.type) {
        node.type->accept(this);
    }
}

// 调用和索引表达式
void StructChecker::visit(CallExpression& node) {
    if (node.expression) {
        node.expression->accept(this);
    }
    if (node.call_params) {
        node.call_params->accept(this);
    }
}

void StructChecker::visit(MethodCallExpression& node) {
    if (node.expression) {
        node.expression->accept(this);
    }
    if (node.call_params) {
        node.call_params->accept(this);
    }
}

void StructChecker::visit(IndexExpression& node) {
    if (node.base_expression) {
        node.base_expression->accept(this);
    }
    if (node.index_expression) {
        node.index_expression->accept(this);
    }
}

// 结构体和数组表达式
void StructChecker::visit(StructExpression& node) {
    if (node.path_in_expression) {
        node.path_in_expression->accept(this);
    }
    if (node.struct_expr_fields) {
        node.struct_expr_fields->accept(this);
    }
}

void StructChecker::visit(ArrayExpression& node) {
    if (node.array_elements) {
        node.array_elements->accept(this);
    }
}

void StructChecker::visit(GroupedExpression& node) {
    if (node.expression) {
        node.expression->accept(this);
    }
}

// 控制流表达式
void StructChecker::visit(BlockExpression& node) {
    // BlockExpression 会创建新的 scope，需要进入
    auto prev_scope = current_scope;
    current_scope = current_scope->getChild();
    
    if (node.statements) {
        node.statements->accept(this);
    }
    
    current_scope = prev_scope;
    current_scope->nextChild();
}

void StructChecker::visit(IfExpression& node) {
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

void StructChecker::visit(LoopExpression& node) {
    if (node.child) {
        node.child->accept(this);
    }
}

void StructChecker::visit(InfiniteLoopExpression& node) {
    // InfiniteLoopExpression 会创建新的 scope，需要进入
    auto prev_scope = current_scope;
    current_scope = current_scope->getChild();
    
    if (node.block_expression) {
        node.block_expression->accept(this);
    }
    
    current_scope = prev_scope;
    current_scope->nextChild();
}

void StructChecker::visit(PredicateLoopExpression& node) {
    // PredicateLoopExpression 会创建新的 scope，需要进入
    auto prev_scope = current_scope;
    current_scope = current_scope->getChild();
    
    if (node.condition) {
        node.condition->accept(this);
    }
    if (node.block_expression) {
        node.block_expression->accept(this);
    }
    
    current_scope = prev_scope;
    current_scope->nextChild();
}

void StructChecker::visit(BreakExpression& node) {
    if (node.expression) {
        node.expression->accept(this);
    }
}

void StructChecker::visit(ContinueExpression& node) {
    // ContinueExpression 不包含类型信息，无需类型检查
}

void StructChecker::visit(ReturnExpression& node) {
    if (node.expression) {
        node.expression->accept(this);
    }
}

// 辅助表达式节点
void StructChecker::visit(Condition& node) {
    if (node.expression) {
        node.expression->accept(this);
    }
}

void StructChecker::visit(ArrayElements& node) {
    for (auto& expr : node.expressions) {
        if (expr) {
            expr->accept(this);
        }
    }
}

void StructChecker::visit(StructExprFields& node) {
    for (auto& field : node.struct_expr_fields) {
        if (field) {
            field->accept(this);
        }
    }
}

void StructChecker::visit(StructExprField& node) {
    if (node.expression) {
        node.expression->accept(this);
    }
}

void StructChecker::visit(CallParams& node) {
    for (auto& expr : node.expressions) {
        if (expr) {
            expr->accept(this);
        }
    }
}

// 模式类节点
void StructChecker::visit(PatternNoTopAlt& node) {
    if (node.child) {
        node.child->accept(this);
    }
}

void StructChecker::visit(IdentifierPattern& node) {
    // IdentifierPattern 不包含类型信息，无需类型检查
}

void StructChecker::visit(ReferencePattern& node) {
    if (node.pattern) {
        node.pattern->accept(this);
    }
}

// 类型类节点
void StructChecker::visit(Type& node) {
    if (node.child) {
        node.child->accept(this);
    }
}

void StructChecker::visit(ReferenceType& node) {
    if (node.type) {
        node.type->accept(this);
    }
}

void StructChecker::visit(ArrayType& node) {
    if (node.type) {
        node.type->accept(this);
    }
    if (node.expression) {
        node.expression->accept(this);
    }
}

void StructChecker::visit(UnitType& node) {
    // UnitType 不包含类型信息，无需类型检查
}

// 路径类节点
void StructChecker::visit(PathInExpression& node) {
    if (node.segment1) {
        node.segment1->accept(this);
    }
    if (node.segment2) {
        node.segment2->accept(this);
    }
}

void StructChecker::visit(PathIdentSegment& node) {
    // PathIdentSegment 不包含类型信息，无需类型检查
}
