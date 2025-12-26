#include "semantic/symbol_collector.hpp"
#include "parser/astnode.hpp"
#include <iostream>

// 构造函数
SymbolCollector::SymbolCollector() {
    root_scope = std::make_shared<Scope>(ScopeType::GLOBAL);
    current_scope = root_scope;
}

// 访问 Crate - 创建全局作用域并遍历所有 items
void SymbolCollector::visit(Crate& node) {
    // std::cout << "Visiting Crate, creating global scope" << std::endl;
    
    // 遍历所有顶层 items
    for (auto& item : node.items) {
        if (item) {
            item->accept(this);
        }
    }
}

// 访问 Item
void SymbolCollector::visit(Item& node) {
    if (node.item) {
        node.item->accept(this);
    }
}

// 访问 Function - 处理函数符号和作用域
void SymbolCollector::visit(Function& node) {
    // std::cout << "Visiting Function: " << node.identifier << std::endl;
    
    // 创建函数符号
    std::string return_type_str = "()";  // 默认返回类型
    if (node.function_return_type && node.function_return_type->type) {
        return_type_str = typeToString(node.function_return_type->type);
    }
    
    // 分析 self 参数类型
    MethodType method_type = MethodType::NOT_METHOD;
    if (node.function_parameters && node.function_parameters->self_param) {
        if (auto shorthand_self = std::dynamic_pointer_cast<ShorthandSelf>(node.function_parameters->self_param->child)) {
            // 处理简写形式的 self: self, &self, mut self, &mut self
            if (shorthand_self->is_reference) {
                if (shorthand_self->is_mutable) {
                    method_type = MethodType::SELF_MUT_REF;  // &mut self
                } else {
                    method_type = MethodType::SELF_REF;       // &self
                }
            } else {
                if (shorthand_self->is_mutable) {
                    method_type = MethodType::SELF_MUT_VALUE; // mut self
                } else {
                    method_type = MethodType::SELF_VALUE;     // self
                }
            }
        } else if (auto typed_self = std::dynamic_pointer_cast<TypedSelf>(node.function_parameters->self_param->child)) {
            // 处理带类型注解的 self: self: Type, mut self: Type
            if (typed_self->is_mutable) {
                method_type = MethodType::SELF_MUT_VALUE; // mut self: Type
            } else {
                method_type = MethodType::SELF_VALUE;     // self: Type
            }
        }
    }
    
    auto func_symbol = std::make_shared<FuncSymbol>(node.identifier, return_type_str, node.is_const, method_type);
    
    // 处理函数参数
    // 保存当前作用域
    auto prev_scope = current_scope;

    // 创建函数作用域
    auto func_scope = std::make_shared<Scope>(ScopeType::FUNCTION, current_scope);
    func_scope->setSelfType(node.identifier);
    current_scope = func_scope;

    // 访问函数参数
    if (node.function_parameters) {
        node.function_parameters->accept(this);
        // 将参数添加到函数符号中
        for (auto& param : node.function_parameters->function_param) {
            if (param) {
                auto var_symbol = createVariableSymbolFromPattern(param->pattern_no_top_alt, param->type);
                if (var_symbol) {
                    func_symbol->addParameter(var_symbol);
                }
            }
        }
    }

    // 将函数符号添加到父作用域
    prev_scope->addFuncSymbol(node.identifier, func_symbol);
    
    // 访问函数体
    if (node.block_expression) {
        node.block_expression->accept(this);
    }
    
    // 恢复作用域
    current_scope = prev_scope;
    
    // 将函数作用域添加为父作用域的子作用域
    prev_scope->addChild(func_scope);
}

// 访问 Struct - 处理结构体符号
void SymbolCollector::visit(Struct& node) {
    if (node.struct_struct) {
        node.struct_struct->accept(this);
    }
}

// 访问 StructStruct
void SymbolCollector::visit(StructStruct& node) {
    // std::cout << "Visiting Struct: " << node.identifier << std::endl;
    
    // 创建结构体符号
    auto struct_symbol = std::make_shared<StructSymbol>(node.identifier, "Struct");
    
    // 处理结构体字段
    if (node.struct_fields) {
        node.struct_fields->accept(this);
        
        // 添加字段到结构体符号
        for (auto& field : node.struct_fields->struct_fields) {
            if (field) {
                std::string field_type = typeToString(field->type);
                auto field_symbol = std::make_shared<VariableSymbol>(field->identifier, field_type);
                struct_symbol->addField(field_symbol);
            }
        }
    }
    
    // 将结构体符号添加到当前作用域
    current_scope->addStructSymbol(node.identifier, struct_symbol);
}

// 访问 StructFields
void SymbolCollector::visit(StructFields& node) {
    // 字段处理在 StructStruct 中完成
}

// 访问 StructField
void SymbolCollector::visit(StructField& node) {
    // 字段处理在 StructStruct 中完成
}

// 访问 Enumeration - 处理枚举符号
void SymbolCollector::visit(Enumeration& node) {
    // std::cout << "Visiting Enum: " << node.identifier << std::endl;
    
    // 创建枚举符号
    auto enum_symbol = std::make_shared<EnumSymbol>(node.identifier, node.identifier);
    
    // 处理枚举变体
    if (node.enum_variants) {
        node.enum_variants->accept(this);
        
        // 添加变体到枚举符号
        for (auto& variant : node.enum_variants->enum_variant) {
            if (variant) {
                auto enum_var = std::make_shared<EnumVar>(variant->identifier);
                enum_symbol->addVariant(enum_var);
            }
        }
    }
    
    // 将枚举符号添加到当前作用域
    current_scope->addEnumSymbol(node.identifier, enum_symbol);
}

// 访问 EnumVariants
void SymbolCollector::visit(EnumVariants& node) {
    // 变体处理在 Enumeration 中完成
}

// 访问 EnumVariant
void SymbolCollector::visit(EnumVariant& node) {
    // 变体处理在 Enumeration 中完成
}

// 访问 ConstantItem - 处理常量符号
void SymbolCollector::visit(ConstantItem& node) {
    // std::cout << "Visiting Constant: " << node.identifier << std::endl;
    
    std::string type_str = "unknown";
    if (node.type) {
        type_str = typeToString(node.type);
    }
    
    // 尝试从表达式创建 ConstValue
    std::shared_ptr<ConstValue> const_value = nullptr;
    // std::cout << "Created ConstValue for " << node.identifier << std::endl;
    auto const_symbol = std::make_shared<ConstSymbol>(node.identifier, type_str, const_value);
    
    // 将常量符号添加到当前作用域
    current_scope->addConstSymbol(node.identifier, const_symbol);
    
    // 访问常量的表达式（如果需要）
    // 注意：这里我们不再调用 node.expression->accept(this)，因为表达式已经在 createConstValueFromExpression 中处理了
    // 如果需要进一步处理表达式内部的结构，可以在 createConstValueFromExpression 中扩展
}

// 访问 Trait - 处理特征符号和作用域
void SymbolCollector::visit(Trait& node) {
    // std::cout << "Visiting Trait: " << node.identifier << std::endl;
    
    // 保存当前作用域
    auto prev_scope = current_scope;
    
    // 创建特征作用域
    auto trait_scope = std::make_shared<Scope>(ScopeType::TRAIT, current_scope);
    current_scope = trait_scope;
    
    // 创建特征符号
    // auto trait_symbol = std::make_shared<TraitSymbol>(node.identifier);
    
    // 处理关联项
    for (auto& item : node.associated_item) {
        if (item) {
            item->accept(this);
            
            // 将关联项添加到特征符号中
            // 改为在 const_evaluator 中做这件事情。
            // if (item->child) {
            //     if (auto const_item = std::dynamic_pointer_cast<ConstantItem>(item->child)) {
            //         std::string type_str = "unknown";
            //         if (const_item->type) {
            //             type_str = typeToString(const_item->type);
            //         }
            //         auto const_symbol = std::make_shared<ConstSymbol>(const_item->identifier, type_str);
            //         trait_symbol->addConstSymbol(const_symbol);
            //     } else if (auto func = std::dynamic_pointer_cast<Function>(item->child)) {
            //         std::cout << "trait func " << func->identifier << std::endl;
            //         std::string return_type_str = "()";
            //         if (func->function_return_type && func->function_return_type->type) {
            //             return_type_str = typeToString(func->function_return_type->type);
            //         }
            //         // 分析 self 参数类型
            //         MethodType method_type = MethodType::NOT_METHOD;
            //         if (func->function_parameters && func->function_parameters->self_param) {
            //             if (auto shorthand_self = std::dynamic_pointer_cast<ShorthandSelf>(func->function_parameters->self_param->child)) {
            //                 // 处理简写形式的 self: self, &self, mut self, &mut self
            //                 if (shorthand_self->is_reference) {
            //                     if (shorthand_self->is_mutable) {
            //                         method_type = MethodType::SELF_MUT_REF;  // &mut self
            //                     } else {
            //                         method_type = MethodType::SELF_REF;       // &self
            //                     }
            //                 } else {
            //                     if (shorthand_self->is_mutable) {
            //                         method_type = MethodType::SELF_MUT_VALUE; // mut self
            //                     } else {
            //                         method_type = MethodType::SELF_VALUE;     // self
            //                     }
            //                 }
            //             } else if (auto typed_self = std::dynamic_pointer_cast<TypedSelf>(func->function_parameters->self_param->child)) {
            //                 // 处理带类型注解的 self: self: Type, mut self: Type
            //                 if (typed_self->is_mutable) {
            //                     method_type = MethodType::SELF_MUT_VALUE; // mut self: Type
            //                 } else {
            //                     method_type = MethodType::SELF_VALUE;     // self: Type
            //                 }
            //             }
            //         }
                    
            //         auto func_symbol = std::make_shared<FuncSymbol>(func->identifier, return_type_str, func->is_const, method_type);

            //         // 访问函数参数
            //         if (func->function_parameters) {
            //             // 将参数添加到函数符号中
            //             for (auto& param : func->function_parameters->function_param) {
            //                 if (param) {
            //                     auto var_symbol = createVariableSymbolFromPattern(param->pattern_no_top_alt, param->type);
            //                     if (var_symbol) {
            //                         func_symbol->addParameter(var_symbol);
            //                     }
            //                 }
            //             }
            //         }
                    
            //         if (method_type != MethodType::NOT_METHOD) {
            //             trait_symbol->addMethod(func_symbol);
            //         } else {
            //             trait_symbol->addAssociatedFunction(func_symbol);
            //         }
            //     }
            // }
        }
    }
    
    // 将特征符号添加到父作用域
    // prev_scope->addTraitSymbol(node.identifier, trait_symbol);
    
    // 恢复作用域
    current_scope = prev_scope;
    
    // 将特征作用域添加为父作用域的子作用域
    prev_scope->addChild(trait_scope);
}

// 访问 Implementation - 处理实现块和作用域
void SymbolCollector::visit(Implementation& node) {
    if (node.impl) {
        node.impl->accept(this);
    }
}

// 访问 InherentImpl
void SymbolCollector::visit(InherentImpl& node) {
    // std::cout << "Visiting InherentImpl" << std::endl;
    
    // 保存当前作用域
    auto prev_scope = current_scope;
    
    // 创建实现作用域
    auto impl_scope = std::make_shared<Scope>(ScopeType::IMPL, current_scope);
    impl_scope->setSelfType(typeToString(node.type));
    current_scope = impl_scope;
    
    // 处理关联项
    for (auto& item : node.associated_item) {
        if (item) {
            item->accept(this);
        }
    }
    
    // 恢复作用域
    current_scope = prev_scope;
    
    // 将实现作用域添加为父作用域的子作用域
    prev_scope->addChild(impl_scope);
}

// 访问 TraitImpl
void SymbolCollector::visit(TraitImpl& node) {
    // std::cout << "Visiting TraitImpl: " << node.identifier << std::endl;
    
    // 保存当前作用域
    auto prev_scope = current_scope;
    
    // 创建实现作用域
    auto impl_scope = std::make_shared<Scope>(ScopeType::IMPL, current_scope);
    impl_scope->setSelfType(typeToString(node.type));
    current_scope = impl_scope;
    
    // 处理关联项
    for (auto& item : node.associated_item) {
        if (item) {
            item->accept(this);
        }
    }
    
    // 恢复作用域
    current_scope = prev_scope;
    
    // 将实现作用域添加为父作用域的子作用域
    prev_scope->addChild(impl_scope);
}

// 访问 AssociatedItem
void SymbolCollector::visit(AssociatedItem& node) {
    if (node.child) {
        node.child->accept(this);
    }
}

// 访问 FunctionParameters
void SymbolCollector::visit(FunctionParameters& node) {
    // 处理 Self 参数
    if (node.self_param) {
        node.self_param->accept(this);
    }
    
    // 处理其他参数
    for (auto& param : node.function_param) {
        if (param) {
            param->accept(this);
        }
    }
}

// 访问 FunctionParam
void SymbolCollector::visit(FunctionParam& node) {
    // 参数处理在 Function 中完成
}

// 访问 FunctionReturnType
void SymbolCollector::visit(FunctionReturnType& node) {
    // 返回类型处理在 Function 中完成
}

// 访问 SelfParam
void SymbolCollector::visit(SelfParam& node) {
    if (node.child) {
        node.child->accept(this);
    }
}

// 访问 ShorthandSelf
void SymbolCollector::visit(ShorthandSelf& node) {
    // Self 参数处理
}

// 访问 TypedSelf
void SymbolCollector::visit(TypedSelf& node) {
    // Self 参数处理
}

// 访问 BlockExpression - 处理块表达式作用域
void SymbolCollector::visit(BlockExpression& node) {
    // std::cout << "Visiting BlockExpression" << std::endl;
    
    // 保存当前作用域
    auto prev_scope = current_scope;
    
    // 创建块作用域
    auto block_scope = std::make_shared<Scope>(ScopeType::BLOCK, current_scope);
    current_scope = block_scope;
    
    // 访问块中的语句
    if (node.statements) {
        node.statements->accept(this);
    }
    
    // 恢复作用域
    current_scope = prev_scope;
    
    // 将块作用域添加为父作用域的子作用域
    prev_scope->addChild(block_scope);
}

// 访问 Statements
void SymbolCollector::visit(Statements& node) {
    for (auto& stmt : node.statements) {
        if (stmt) {
            stmt->accept(this);
        }
    }
}

// 访问 Statement
void SymbolCollector::visit(Statement& node) {
    if (node.child) {
        node.child->accept(this);
    }
}

// 访问 LetStatement
void SymbolCollector::visit(LetStatement& node) {
    // 目前暂时不将 LetStatement 中的东西放入 Scope 的符号表。
    
    // 访问初始化表达式
    if (node.expression) {
        node.expression->accept(this);
    }
}

// 访问 ExpressionStatement
void SymbolCollector::visit(ExpressionStatement& node) {
    if (node.child) {
        node.child->accept(this);
    }
}

// 访问 Expression
void SymbolCollector::visit(Expression& node) {
    if (node.child) {
        node.child->accept(this);
    }
}

// 访问 ExpressionWithoutBlock
void SymbolCollector::visit(ExpressionWithoutBlock& node) {
    if (node.child) {
        node.child->accept(this);
    }
}

// 访问 ExpressionWithBlock
void SymbolCollector::visit(ExpressionWithBlock& node) {
    if (node.child) {
        node.child->accept(this);
    }
}

// 访问 LoopExpression - 处理循环表达式作用域
void SymbolCollector::visit(LoopExpression& node) {
    if (node.child) {
        node.child->accept(this);
    }
}

// 访问 InfiniteLoopExpression
void SymbolCollector::visit(InfiniteLoopExpression& node) {
    // std::cout << "Visiting InfiniteLoopExpression" << std::endl;
    
    // 保存当前作用域
    auto prev_scope = current_scope;
    
    // 创建循环作用域
    auto loop_scope = std::make_shared<Scope>(ScopeType::LOOP, current_scope);
    current_scope = loop_scope;
    
    // 访问循环体
    if (node.block_expression) {
        node.block_expression->accept(this);
    }
    
    // 恢复作用域
    current_scope = prev_scope;
    
    // 将循环作用域添加为父作用域的子作用域
    prev_scope->addChild(loop_scope);
}

// 访问 PredicateLoopExpression
void SymbolCollector::visit(PredicateLoopExpression& node) {
    // std::cout << "Visiting PredicateLoopExpression" << std::endl;
    
    // 保存当前作用域
    auto prev_scope = current_scope;
    
    // 创建循环作用域
    auto loop_scope = std::make_shared<Scope>(ScopeType::LOOP, current_scope);
    current_scope = loop_scope;
    
    // 访问条件
    if (node.condition) {
        node.condition->accept(this);
    }
    
    // 访问循环体
    if (node.block_expression) {
        node.block_expression->accept(this);
    }
    
    // 恢复作用域
    current_scope = prev_scope;
    
    // 将循环作用域添加为父作用域的子作用域
    prev_scope->addChild(loop_scope);
}

// 访问 IfExpression
void SymbolCollector::visit(IfExpression& node) {
    // 访问条件
    if (node.condition) {
        node.condition->accept(this);
    }
    
    // 访问 then 块
    if (node.then_block) {
        node.then_block->accept(this);
    }
    
    // 访问 else 分支
    if (node.else_branch) {
        node.else_branch->accept(this);
    }
}

// 访问 PatternNoTopAlt
void SymbolCollector::visit(PatternNoTopAlt& node) {
    if (node.child) {
        node.child->accept(this);
    }
}

// 访问 IdentifierPattern
void SymbolCollector::visit(IdentifierPattern& node) {
    // 模式处理在 createVariableSymbolFromPattern 中完成
}

// 访问 Type
void SymbolCollector::visit(Type& node) {
    if (node.child) {
        node.child->accept(this);
    }
}

// 访问 PathInExpression
void SymbolCollector::visit(PathInExpression& node) {
    if (node.segment1) {
        node.segment1->accept(this);
    }
    if (node.segment2) {
        node.segment2->accept(this);
    }
}

// 访问 PathIdentSegment
void SymbolCollector::visit(PathIdentSegment& node) {
    // 路径段处理
}

// 以下是需要完整遍历的其他节点方法
void SymbolCollector::visit(CallExpression& node) {
    if (node.expression) node.expression->accept(this);
    if (node.call_params) node.call_params->accept(this);
}

void SymbolCollector::visit(MethodCallExpression& node) {
    if (node.expression) node.expression->accept(this);
    if (node.path_ident_segment) node.path_ident_segment->accept(this);
    if (node.call_params) node.call_params->accept(this);
}

void SymbolCollector::visit(FieldExpression& node) {
    if (node.expression) node.expression->accept(this);
}

void SymbolCollector::visit(IndexExpression& node) {
    if (node.base_expression) node.base_expression->accept(this);
    if (node.index_expression) node.index_expression->accept(this);
}

void SymbolCollector::visit(StructExpression& node) {
    if (node.path_in_expression) node.path_in_expression->accept(this);
    if (node.struct_expr_fields) node.struct_expr_fields->accept(this);
}

void SymbolCollector::visit(ArrayExpression& node) {
    if (node.array_elements) node.array_elements->accept(this);
}

void SymbolCollector::visit(GroupedExpression& node) {
    if (node.expression) node.expression->accept(this);
}

void SymbolCollector::visit(UnaryExpression& node) {
    if (node.expression) node.expression->accept(this);
}

void SymbolCollector::visit(BorrowExpression& node) {
    if (node.expression) node.expression->accept(this);
}

void SymbolCollector::visit(DereferenceExpression& node) {
    if (node.expression) node.expression->accept(this);
}

void SymbolCollector::visit(BinaryExpression& node) {
    if (node.lhs) node.lhs->accept(this);
    if (node.rhs) node.rhs->accept(this);
}

void SymbolCollector::visit(AssignmentExpression& node) {
    if (node.lhs) node.lhs->accept(this);
    if (node.rhs) node.rhs->accept(this);
}

void SymbolCollector::visit(CompoundAssignmentExpression& node) {
    if (node.lhs) node.lhs->accept(this);
    if (node.rhs) node.rhs->accept(this);
}

void SymbolCollector::visit(TypeCastExpression& node) {
    if (node.expression) node.expression->accept(this);
    if (node.type) node.type->accept(this);
}

void SymbolCollector::visit(PathExpression& node) {
    if (node.path_in_expression) node.path_in_expression->accept(this);
}

void SymbolCollector::visit(Condition& node) {
    if (node.expression) node.expression->accept(this);
}

void SymbolCollector::visit(ArrayElements& node) {
    for (auto& expr : node.expressions) {
        if (expr) expr->accept(this);
    }
}

void SymbolCollector::visit(StructExprFields& node) {
    for (auto& field : node.struct_expr_fields) {
        if (field) field->accept(this);
    }
}

void SymbolCollector::visit(StructExprField& node) {
    if (node.expression) node.expression->accept(this);
}

void SymbolCollector::visit(CallParams& node) {
    for (auto& expr : node.expressions) {
        if (expr) expr->accept(this);
    }
}

void SymbolCollector::visit(ReferenceType& node) {
    if (node.type) node.type->accept(this);
}

void SymbolCollector::visit(ArrayType& node) {
    if (node.type) node.type->accept(this);
    if (node.expression) node.expression->accept(this);
}

void SymbolCollector::visit(UnitType& node) {
    // 单元类型，无需进一步处理
}

void SymbolCollector::visit(ReferencePattern& node) {
    if (node.pattern) node.pattern->accept(this);
}

// 字面量表达式
void SymbolCollector::visit(CharLiteral& node) {
    // 字面量，无需进一步处理
}

void SymbolCollector::visit(StringLiteral& node) {
    // 字面量，无需进一步处理
}

void SymbolCollector::visit(RawStringLiteral& node) {
    // 字面量，无需进一步处理
}

void SymbolCollector::visit(CStringLiteral& node) {
    // 字面量，无需进一步处理
}

void SymbolCollector::visit(RawCStringLiteral& node) {
    // 字面量，无需进一步处理
}

void SymbolCollector::visit(IntegerLiteral& node) {
    // 字面量，无需进一步处理
}

void SymbolCollector::visit(BoolLiteral& node) {
    // 字面量，无需进一步处理
}

// 控制流
void SymbolCollector::visit(BreakExpression& node) {
    if (node.expression) node.expression->accept(this);
}

void SymbolCollector::visit(ContinueExpression& node) {
    // continue 表达式，无需进一步处理
}

void SymbolCollector::visit(ReturnExpression& node) {
    if (node.expression) node.expression->accept(this);
}
