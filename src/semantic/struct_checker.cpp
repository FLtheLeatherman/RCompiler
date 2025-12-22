#include "semantic/struct_checker.hpp"
#include "semantic/const_value.hpp"
#include <iostream>
#include <stdexcept>

// 构造函数
StructChecker::StructChecker(std::shared_ptr<Scope> root_scope) 
    : root_scope(root_scope), current_scope(root_scope) {}

// 辅助方法：将 Type 转换为字符串
std::string StructChecker::typeToString(std::shared_ptr<Type> type) {
    if (!type || !type->child) {
        return "unknown";
    }
    
    // 处理不同的类型
    if (auto path_ident = std::dynamic_pointer_cast<PathIdentSegment>(type->child)) {
        return path_ident->identifier;
    } else if (auto ref_type = std::dynamic_pointer_cast<ReferenceType>(type->child)) {
        std::string base_type = typeToString(ref_type->type);
        return (ref_type->is_mutable ? "&mut " : "&") + base_type;
    } else if (auto array_type = std::dynamic_pointer_cast<ArrayType>(type->child)) {
        std::string base_type = typeToString(array_type->type);
        return "[" + base_type + "]";
    } else if (auto unit_type = std::dynamic_pointer_cast<UnitType>(type->child)) {
        return "()";
    }
    
    return "unknown";
}

// 辅助方法：检查类型是否存在
bool StructChecker::checkTypeExists(const std::string& type_name) {
    if (type_name == "unknown" || type_name == "()") {
        return true; // 基础类型总是存在
    }
    
    // 检查基础类型
    if (type_name == "i32" || type_name == "i64" || type_name == "u32" || type_name == "u64" ||
        type_name == "f32" || type_name == "f64" || type_name == "bool" || type_name == "char" ||
        type_name == "str" || type_name == "String") {
        return true; // Rust 基础类型
    }
    
    // 在当前作用域及其父作用域中查找结构体、枚举或特征
    auto struct_symbol = current_scope->findStructSymbol(type_name);
    if (struct_symbol) return true;
    
    auto enum_symbol = current_scope->findEnumSymbol(type_name);
    if (enum_symbol) return true;
    
    auto trait_symbol = current_scope->findTraitSymbol(type_name);
    if (trait_symbol) return true;
    
    return false;
}

// 辅助方法：查找结构体符号
std::shared_ptr<StructSymbol> StructChecker::findStructSymbol(const std::string& struct_name) {
    return current_scope->findStructSymbol(struct_name);
}

// 辅助方法：查找特征符号
std::shared_ptr<TraitSymbol> StructChecker::findTraitSymbol(const std::string& trait_name) {
    return current_scope->findTraitSymbol(trait_name);
}

// 辅助方法：检查函数是否有 self 参数
bool StructChecker::hasSelfParameter(std::shared_ptr<Function> func) {
    if (!func || !func->function_parameters) {
        return false;
    }
    
    // 检查是否有 self 参数
    if (func->function_parameters->self_param) {
        return true;
    }
    
    return false;
}

// 辅助方法：检查特征实现完整性
void StructChecker::checkTraitImplementation(std::shared_ptr<TraitSymbol> trait_symbol, 
                                              std::shared_ptr<StructSymbol> struct_symbol,
                                              const std::vector<std::shared_ptr<AssociatedItem>>& impl_items) {
    if (!trait_symbol || !struct_symbol) {
        return;
    }
    
    // 创建已实现项的映射
    std::unordered_map<std::string, std::shared_ptr<FuncSymbol>> implemented_funcs;
    std::unordered_map<std::string, std::shared_ptr<ConstSymbol>> implemented_consts;
    
    for (auto& item : impl_items) {
        if (!item || !item->child) continue;
        
        if (auto func = std::dynamic_pointer_cast<Function>(item->child)) {
            implemented_funcs[func->identifier] = nullptr; // 标记为已实现
        } else if (auto const_item = std::dynamic_pointer_cast<ConstantItem>(item->child)) {
            implemented_consts[const_item->identifier] = nullptr; // 标记为已实现
        }
    }
    
    // 检查特征中的关联函数是否全部实现
    for (auto& trait_func : trait_symbol->getAssociatedFunctions()) {
        if (implemented_funcs.find(trait_func->getIdentifier()) == implemented_funcs.end()) {
            throw std::runtime_error("Missing implementation of trait function: " + trait_func->getIdentifier());
        }
    }
    
    // 检查特征中的关联常量是否全部实现
    for (auto& trait_const : trait_symbol->getConstSymbols()) {
        if (implemented_consts.find(trait_const->getIdentifier()) == implemented_consts.end()) {
            throw std::runtime_error("Missing implementation of trait constant: " + trait_const->getIdentifier());
        }
    }
}

// 访问 Crate - 遍历所有顶层项
void StructChecker::visit(Crate& node) {
    std::cout << "StructChecker: Visiting Crate" << std::endl;
    
    for (auto& item : node.items) {
        if (item) {
            item->accept(this);
        }
    }
}

// 访问 Item
void StructChecker::visit(Item& node) {
    if (node.item) {
        node.item->accept(this);
    }
}

// 访问 Struct - 处理结构体定义
void StructChecker::visit(Struct& node) {
    if (node.struct_struct) {
        node.struct_struct->accept(this);
    }
}

// 访问 StructStruct - 检查结构体字段类型
void StructChecker::visit(StructStruct& node) {
    std::cout << "StructChecker: Visiting Struct: " << node.identifier << std::endl;
    
    // 查找结构体符号
    auto struct_symbol = findStructSymbol(node.identifier);
    if (!struct_symbol) {
        throw std::runtime_error("Undefined Name: " + node.identifier);
    }
    
    // 检查字段类型
    if (node.struct_fields) {
        node.struct_fields->accept(this);
    }
}

// 访问 StructFields
void StructChecker::visit(StructFields& node) {
    for (auto& field : node.struct_fields) {
        if (field) {
            field->accept(this);
        }
    }
}

// 访问 StructField - 检查字段类型
void StructChecker::visit(StructField& node) {
    std::string field_type = typeToString(node.type);
    
    // 检查字段类型是否存在
    if (!checkTypeExists(field_type)) {
        throw std::runtime_error("Undefined Name: " + field_type);
    }
    
    std::cout << "StructChecker: Field " << node.identifier << " has valid type: " << field_type << std::endl;
}

// 访问 Implementation
void StructChecker::visit(Implementation& node) {
    if (node.impl) {
        node.impl->accept(this);
    }
}

// 访问 InherentImpl - 处理固有实现
void StructChecker::visit(InherentImpl& node) {
    std::cout << "StructChecker: Visiting InherentImpl" << std::endl;
    
    // 获取目标结构体类型
    std::string struct_type_name = typeToString(node.type);
    
    // 检查结构体是否存在
    auto struct_symbol = findStructSymbol(struct_type_name);
    if (!struct_symbol) {
        throw std::runtime_error("Undefined Name: " + struct_type_name);
    }
    
    // 处理关联项
    for (auto& item : node.associated_item) {
        if (item) {
            item->accept(this);
            
            // 将关联项添加到结构体中
            if (item->child) {
                if (auto func = std::dynamic_pointer_cast<Function>(item->child)) {
                    // 创建函数符号
                    std::string return_type_str = "()";
                    if (func->function_return_type && func->function_return_type->type) {
                        return_type_str = typeToString(func->function_return_type->type);
                    }
                    
                    auto func_symbol = std::make_shared<FuncSymbol>(func->identifier, return_type_str, func->is_const);
                    
                    // 处理函数参数
                    if (func->function_parameters) {
                        for (auto& param : func->function_parameters->function_param) {
                            if (param) {
                                std::string param_type = typeToString(param->type);
                                if (!checkTypeExists(param_type)) {
                                    throw std::runtime_error("Undefined Name: " + param_type);
                                }
                                // 创建参数符号（简化处理）
                                auto param_symbol = std::make_shared<VariableSymbol>("param", param_type);
                                func_symbol->addParameter(param_symbol);
                            }
                        }
                    }
                    
                    // 检查是否有 self 参数，决定是方法还是关联函数
                    if (hasSelfParameter(func)) {
                        struct_symbol->addMethod(func_symbol);
                        std::cout << "StructChecker: Added method " << func->identifier << " to struct " << struct_type_name << std::endl;
                    } else {
                        struct_symbol->addAssociatedFunction(func_symbol);
                        std::cout << "StructChecker: Added associated function " << func->identifier << " to struct " << struct_type_name << std::endl;
                    }
                } else if (auto const_item = std::dynamic_pointer_cast<ConstantItem>(item->child)) {
                    // 检查常量类型
                    std::string const_type = "unknown";
                    if (const_item->type) {
                        const_type = typeToString(const_item->type);
                        if (!checkTypeExists(const_type)) {
                            throw std::runtime_error("Undefined Name: " + const_type);
                        }
                    }
                    
                    auto const_symbol = std::make_shared<ConstSymbol>(const_item->identifier, const_type);
                    struct_symbol->addAssociatedConst(const_symbol);
                    std::cout << "StructChecker: Added associated constant " << const_item->identifier << " to struct " << struct_type_name << std::endl;
                }
            }
        }
    }
}

// 访问 TraitImpl - 处理特征实现
void StructChecker::visit(TraitImpl& node) {
    std::cout << "StructChecker: Visiting TraitImpl: " << node.identifier << std::endl;
    
    // 检查特征是否存在
    auto trait_symbol = findTraitSymbol(node.identifier);
    if (!trait_symbol) {
        throw std::runtime_error("Undefined Name: " + node.identifier);
    }
    
    // 获取目标结构体类型
    std::string struct_type_name = typeToString(node.type);
    
    // 检查结构体是否存在
    auto struct_symbol = findStructSymbol(struct_type_name);
    if (!struct_symbol) {
        throw std::runtime_error("Undefined Name: " + struct_type_name);
    }
    
    // 检查特征实现完整性
    checkTraitImplementation(trait_symbol, struct_symbol, node.associated_item);
    
    // 处理关联项（类似 InherentImpl，但需要验证与特征的兼容性）
    for (auto& item : node.associated_item) {
        if (item) {
            item->accept(this);
            
            // 将关联项添加到结构体中
            if (item->child) {
                if (auto func = std::dynamic_pointer_cast<Function>(item->child)) {
                    // 创建函数符号
                    std::string return_type_str = "()";
                    if (func->function_return_type && func->function_return_type->type) {
                        return_type_str = typeToString(func->function_return_type->type);
                    }
                    
                    auto func_symbol = std::make_shared<FuncSymbol>(func->identifier, return_type_str, func->is_const);
                    
                    // 处理函数参数
                    if (func->function_parameters) {
                        for (auto& param : func->function_parameters->function_param) {
                            if (param) {
                                std::string param_type = typeToString(param->type);
                                if (!checkTypeExists(param_type)) {
                                    throw std::runtime_error("Undefined Name: " + param_type);
                                }
                                auto param_symbol = std::make_shared<VariableSymbol>("param", param_type);
                                func_symbol->addParameter(param_symbol);
                            }
                        }
                    }
                    
                    // 检查是否有 self 参数，决定是方法还是关联函数
                    if (hasSelfParameter(func)) {
                        struct_symbol->addMethod(func_symbol);
                        std::cout << "StructChecker: Added trait method " << func->identifier << " to struct " << struct_type_name << std::endl;
                    } else {
                        struct_symbol->addAssociatedFunction(func_symbol);
                        std::cout << "StructChecker: Added trait associated function " << func->identifier << " to struct " << struct_type_name << std::endl;
                    }
                } else if (auto const_item = std::dynamic_pointer_cast<ConstantItem>(item->child)) {
                    // 检查常量类型
                    std::string const_type = "unknown";
                    if (const_item->type) {
                        const_type = typeToString(const_item->type);
                        if (!checkTypeExists(const_type)) {
                            throw std::runtime_error("Undefined Name: " + const_type);
                        }
                    }
                    
                    auto const_symbol = std::make_shared<ConstSymbol>(const_item->identifier, const_type);
                    struct_symbol->addAssociatedConst(const_symbol);
                    std::cout << "StructChecker: Added trait associated constant " << const_item->identifier << " to struct " << struct_type_name << std::endl;
                }
            }
        }
    }
}

// 访问 AssociatedItem
void StructChecker::visit(AssociatedItem& node) {
    if (node.child) {
        node.child->accept(this);
    }
}

// 访问 Function - 检查函数参数和返回类型
void StructChecker::visit(Function& node) {
    std::cout << "StructChecker: Visiting Function: " << node.identifier << std::endl;
    
    // 检查返回类型
    if (node.function_return_type && node.function_return_type->type) {
        std::string return_type = typeToString(node.function_return_type->type);
        if (!checkTypeExists(return_type)) {
            throw std::runtime_error("Undefined Name: " + return_type);
        }
    }
    
    // 检查参数类型
    if (node.function_parameters) {
        node.function_parameters->accept(this);
    }
}

// 访问 FunctionParameters
void StructChecker::visit(FunctionParameters& node) {
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

// 访问 SelfParam
void StructChecker::visit(SelfParam& node) {
    if (node.child) {
        node.child->accept(this);
    }
}

// 访问 ShorthandSelf
void StructChecker::visit(ShorthandSelf& node) {
    // Self 参数，无需检查类型
}

// 访问 TypedSelf
void StructChecker::visit(TypedSelf& node) {
    // 检查 Self 的类型
    if (node.type) {
        std::string self_type = typeToString(node.type);
        if (!checkTypeExists(self_type)) {
            throw std::runtime_error("Undefined Name: " + self_type);
        }
    }
}

// 访问 FunctionParam - 检查参数类型
void StructChecker::visit(FunctionParam& node) {
    std::string param_type = typeToString(node.type);
    
    // 检查参数类型是否存在
    if (!checkTypeExists(param_type)) {
        throw std::runtime_error("Undefined Name: " + param_type);
    }
    
    std::cout << "StructChecker: Parameter has valid type: " << param_type << std::endl;
}

// 访问 FunctionReturnType
void StructChecker::visit(FunctionReturnType& node) {
    // 返回类型检查在 Function 中完成
}

// 访问 ConstantItem - 检查常量类型
void StructChecker::visit(ConstantItem& node) {
    std::cout << "StructChecker: Visiting Constant: " << node.identifier << std::endl;
    
    if (node.type) {
        std::string const_type = typeToString(node.type);
        if (!checkTypeExists(const_type)) {
            throw std::runtime_error("Undefined Name: " + const_type);
        }
        std::cout << "StructChecker: Constant " << node.identifier << " has valid type: " << const_type << std::endl;
    }
}

// 访问 Trait
void StructChecker::visit(Trait& node) {
    std::cout << "StructChecker: Visiting Trait: " << node.identifier << std::endl;
    
    // 检查关联项的类型
    for (auto& item : node.associated_item) {
        if (item) {
            item->accept(this);
        }
    }
}

// 以下是需要完整遍历的其他节点方法（保持 AST 遍历的完整性）
void StructChecker::visit(Type& node) {
    if (node.child) {
        node.child->accept(this);
    }
}

void StructChecker::visit(PathInExpression& node) {
    if (node.segment1) {
        node.segment1->accept(this);
    }
    if (node.segment2) {
        node.segment2->accept(this);
    }
}

void StructChecker::visit(PathIdentSegment& node) {
    // 路径段，无需进一步处理
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
    // 单元类型，无需进一步处理
}

void StructChecker::visit(Enumeration& node) {
    if (node.enum_variants) {
        node.enum_variants->accept(this);
    }
}

void StructChecker::visit(EnumVariants& node) {
    for (auto& variant : node.enum_variant) {
        if (variant) {
            variant->accept(this);
        }
    }
}

void StructChecker::visit(EnumVariant& node) {
    // 枚举变体，无需进一步处理
}

void StructChecker::visit(Statement& node) {
    if (node.child) {
        node.child->accept(this);
    }
}

void StructChecker::visit(LetStatement& node) {
    if (node.pattern_no_top_alt) {
        node.pattern_no_top_alt->accept(this);
    }
    if (node.type) {
        node.type->accept(this);
    }
    if (node.expression) {
        node.expression->accept(this);
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

void StructChecker::visit(BlockExpression& node) {
    if (node.statements) {
        node.statements->accept(this);
    }
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
    if (node.block_expression) {
        node.block_expression->accept(this);
    }
}

void StructChecker::visit(PredicateLoopExpression& node) {
    if (node.condition) {
        node.condition->accept(this);
    }
    if (node.block_expression) {
        node.block_expression->accept(this);
    }
}

void StructChecker::visit(PatternNoTopAlt& node) {
    if (node.child) {
        node.child->accept(this);
    }
}

void StructChecker::visit(IdentifierPattern& node) {
    // 标识符模式，无需进一步处理
}

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
    if (node.path_ident_segment) {
        node.path_ident_segment->accept(this);
    }
    if (node.call_params) {
        node.call_params->accept(this);
    }
}

void StructChecker::visit(FieldExpression& node) {
    if (node.expression) {
        node.expression->accept(this);
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

void StructChecker::visit(PathExpression& node) {
    if (node.path_in_expression) {
        node.path_in_expression->accept(this);
    }
}

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

void StructChecker::visit(ReferencePattern& node) {
    if (node.pattern) {
        node.pattern->accept(this);
    }
}

void StructChecker::visit(CharLiteral& node) {
    // 字面量，无需进一步处理
}

void StructChecker::visit(StringLiteral& node) {
    // 字面量，无需进一步处理
}

void StructChecker::visit(RawStringLiteral& node) {
    // 字面量，无需进一步处理
}

void StructChecker::visit(CStringLiteral& node) {
    // 字面量，无需进一步处理
}

void StructChecker::visit(RawCStringLiteral& node) {
    // 字面量，无需进一步处理
}

void StructChecker::visit(IntegerLiteral& node) {
    // 字面量，无需进一步处理
}

void StructChecker::visit(BoolLiteral& node) {
    // 字面量，无需进一步处理
}

void StructChecker::visit(BreakExpression& node) {
    if (node.expression) {
        node.expression->accept(this);
    }
}

void StructChecker::visit(ContinueExpression& node) {
    // continue 表达式，无需进一步处理
}

void StructChecker::visit(ReturnExpression& node) {
    if (node.expression) {
        node.expression->accept(this);
    }
}