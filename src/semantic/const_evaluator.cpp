#include "semantic/const_evaluator.hpp"

// 辅助函数实现
std::shared_ptr<ConstValue> ConstEvaluator::createConstValueFromExpression(std::shared_ptr<ASTNode> expression) {
    if (!expression) {
        return nullptr;
    }
    
    // 处理 Expression 包装器
    if (auto expr_wrapper = std::dynamic_pointer_cast<Expression>(expression)) {
        if (expr_wrapper->child) {
            return createConstValueFromExpression(expr_wrapper->child);
        }
    }
    
    // 尝试转换为不同的字面量类型
    if (auto int_literal = std::dynamic_pointer_cast<IntegerLiteral>(expression)) {
        try {
            int value = std::stoi(int_literal->value);
            auto result = std::make_shared<ConstValueInt>(value, expression);
            return result;
        } catch (const std::exception& e) {
            // 转换失败，返回 null
            return nullptr;
        }
    }
    if (auto bool_literal = std::dynamic_pointer_cast<BoolLiteral>(expression)) {
        return std::make_shared<ConstValueBool>(bool_literal->value, expression);
    }
    if (auto char_literal = std::dynamic_pointer_cast<CharLiteral>(expression)) {
        if (!char_literal->value.empty()) {
            return std::make_shared<ConstValueChar>(char_literal->value[0], expression);
        }
    }
    if (auto string_literal = std::dynamic_pointer_cast<StringLiteral>(expression)) {
        return std::make_shared<ConstValueString>(string_literal->value, expression);
    }

    if (auto path_expr = std::dynamic_pointer_cast<PathExpression>(expression)) {
        if (path_expr->path_in_expression) {
            return createConstValueFromExpression(std::dynamic_pointer_cast<ASTNode>(path_expr->path_in_expression));
        }
    }
    
    if (auto path_in_expr = std::dynamic_pointer_cast<PathInExpression>(expression)) {
        if (path_in_expr->segment2) {
            auto struct_identifier = path_in_expr->segment1->identifier;
            auto identifier = path_in_expr->segment2->identifier;
            auto struct_symbol = current_scope->findStructSymbol(struct_identifier);
            if (!struct_symbol) throw std::runtime_error("Const Evaluation Error");
            auto const_symbol = struct_symbol->getAssociatedConst(identifier);
            if (!const_symbol) throw std::runtime_error("Const Evaluation Error");
            return const_symbol->getValue();
        } else {
            auto identifier = path_in_expr->segment1->identifier;
            auto const_symbol = current_scope->getConstSymbol(identifier);
            if (!const_symbol) throw std::runtime_error("Const Evaluation Error");
            return const_symbol->getValue();
        }
    }

    // 处理括号表达式
    if (auto grouped_expr = std::dynamic_pointer_cast<GroupedExpression>(expression)) {
        return createConstValueFromExpression(std::dynamic_pointer_cast<ASTNode>(grouped_expr->expression));
    }

    // 处理一元表达式（负号）
    if (auto unary_expr = std::dynamic_pointer_cast<UnaryExpression>(expression)) {
        if (unary_expr->type == UnaryExpression::MINUS) {
            auto operand_value = createConstValueFromExpression(std::dynamic_pointer_cast<ASTNode>(unary_expr->expression));
            if (!operand_value || !operand_value->isInt()) {
                throw std::runtime_error("Const Evaluation Error: Unary minus can only be applied to integer constants");
            }
            auto int_value = std::dynamic_pointer_cast<ConstValueInt>(operand_value);
            return std::make_shared<ConstValueInt>(-int_value->getValue(), expression);
        } else {
            throw std::runtime_error("Const Evaluation Error: Only unary minus is supported in constant expressions");
        }
    }

    // 处理二元表达式（算术运算和位运算）
    if (auto binary_expr = std::dynamic_pointer_cast<BinaryExpression>(expression)) {
        auto left_value = createConstValueFromExpression(std::dynamic_pointer_cast<ASTNode>(binary_expr->lhs));
        auto right_value = createConstValueFromExpression(std::dynamic_pointer_cast<ASTNode>(binary_expr->rhs));
        
        if (!left_value || !right_value) {
            throw std::runtime_error("Const Evaluation Error: Invalid operands in binary expression");
        }
        
        // 只支持整型运算
        if (!left_value->isInt() || !right_value->isInt()) {
            throw std::runtime_error("Const Evaluation Error: Binary operations only support integer constants");
        }
        
        auto left_int = std::dynamic_pointer_cast<ConstValueInt>(left_value);
        auto right_int = std::dynamic_pointer_cast<ConstValueInt>(right_value);
        
        int result;
        switch (binary_expr->type) {
            case BinaryExpression::PLUS:
                result = left_int->getValue() + right_int->getValue();
                break;
            case BinaryExpression::MINUS:
                result = left_int->getValue() - right_int->getValue();
                break;
            case BinaryExpression::STAR:
                result = left_int->getValue() * right_int->getValue();
                break;
            case BinaryExpression::SLASH:
                if (right_int->getValue() == 0) {
                    throw std::runtime_error("Const Evaluation Error: Division by zero");
                }
                result = left_int->getValue() / right_int->getValue();
                break;
            case BinaryExpression::PERCENT:
                if (right_int->getValue() == 0) {
                    throw std::runtime_error("Const Evaluation Error: Modulo by zero");
                }
                result = left_int->getValue() % right_int->getValue();
                break;
            // 位运算
            case BinaryExpression::CARET:  // ^
                result = left_int->getValue() ^ right_int->getValue();
                break;
            case BinaryExpression::AND:    // &
                result = left_int->getValue() & right_int->getValue();
                break;
            case BinaryExpression::OR:     // |
                result = left_int->getValue() | right_int->getValue();
                break;
            case BinaryExpression::SHL:   // <<
                result = left_int->getValue() << right_int->getValue();
                break;
            case BinaryExpression::SHR:   // >>
                result = left_int->getValue() >> right_int->getValue();
                break;
            // 不支持的操作符
            case BinaryExpression::EQ_EQ:   // ==
            case BinaryExpression::NE:      // !=
            case BinaryExpression::GT:      // >
            case BinaryExpression::LT:      // <
            case BinaryExpression::GE:      // >=
            case BinaryExpression::LE:      // <=
            case BinaryExpression::AND_AND: // &&
            case BinaryExpression::OR_OR:   // ||
                throw std::runtime_error("Const Evaluation Error: Comparison and logical operators are not supported in constant expressions");
            default:
                throw std::runtime_error("Const Evaluation Error: Unsupported binary operator");
        }
        
        return std::make_shared<ConstValueInt>(result, expression);
    }

    throw std::runtime_error("Const Evaluation Error: Unsupported expression type in constant context");
}

std::string ConstEvaluator::handleArraySymbol(std::shared_ptr<Type> node) {
    if (auto ref_type = std::dynamic_pointer_cast<ReferenceType>(node->child)) {
        // std::cout << "啊？" << std::endl;
        std::string res = "&";
        if (ref_type->is_mutable) res += "mut";
        return res + handleArraySymbol(ref_type->type);
    } else if (auto type_path = std::dynamic_pointer_cast<PathIdentSegment>(node->child)) {
        // std::cout << "cast to pathtype!" << std::endl;
        // std::cout << type_path->identifier << std::endl;
        return type_path->identifier;
    } else if (auto array_type = std::dynamic_pointer_cast<ArrayType>(node->child)) {
        // std::cout << "cast to arraytype!" << std::endl;
        auto expression = array_type->expression;
        auto length = createConstValueFromExpression(expression);
        if (!length->isInt()) {
            throw std::runtime_error("Const Evaluation Error: Array length not integer");
        }
        int len = std::dynamic_pointer_cast<ConstValueInt>(length)->getValue();
        // std::cout << len << std::endl;
        return "[" + handleArraySymbol(array_type->type) + "]" + std::to_string(len);
    } else {
        // std::cout << "啊！" << std::endl;
        throw std::runtime_error("Const Evaluation Error: Array symbol parse failed");
    }
}

ConstEvaluator::ConstEvaluator(std::shared_ptr<Scope> root_scope) {
    this->root_scope = root_scope;
    this->current_scope = root_scope;
}

void ConstEvaluator::visit(Crate& node) {
    for (auto item: node.items) {
        item->accept(this);
    }
}

void ConstEvaluator::visit(ConstantItem& node) {
    std::string identifier = node.identifier;
    auto const_symbol = current_scope->getConstSymbol(identifier);
    const_symbol->setValue(createConstValueFromExpression(node.expression));
}

void ConstEvaluator::visit(Function& node) {
    // std::cout << "visit Function: " << node.identifier << std::endl;
    if (node.function_parameters) {
        auto func_params = node.function_parameters->function_param;
        auto func_symbol = current_scope->getFuncSymbol(node.identifier);
        auto func_symbol_params = func_symbol->getParameters();
        for (size_t i = 0; i < func_params.size(); ++i) {
            if (func_symbol_params[i]->getType()[0] == '[') {
                auto type = handleArraySymbol(func_params[i]->type);
                func_symbol_params[i]->setType(type);
            }
        }
    }

    auto func_symbol = current_scope->getFuncSymbol(node.identifier);
    auto func_return_type = func_symbol->getReturnType();
    if (func_return_type[0] == '[') {
        auto type = node.function_return_type->type;
        auto new_type = handleArraySymbol(type);
        func_symbol->setReturnType(new_type);
    }

    // std::cout << "Function handle done" << std::endl;
    auto prev_scope = current_scope;
    current_scope = current_scope->getChild();

    if (node.block_expression && current_scope) {
        node.block_expression->accept(this);
    }
    current_scope = prev_scope;
    current_scope->nextChild();
}

void ConstEvaluator::visit(StructStruct& node) {
    // std::cout << "visit Struct: " << node.identifier << std::endl;
    auto struct_fields = node.struct_fields->struct_fields;
    auto struct_symbol = current_scope->getStructSymbol(node.identifier);
    for (auto struct_field: struct_fields) {
        auto type = typeToString(struct_field->type);
        // std::cout << type << std::endl;
        if (type[0] == '[') {
            auto arr_type = handleArraySymbol(struct_field->type);
            // std::cout << arr_type << std::endl;
            auto field_symbol = struct_symbol->getField(struct_field->identifier);
            struct_symbol->eraseField(struct_field->identifier);
            struct_symbol->addField(std::make_shared<VariableSymbol>(
                field_symbol->getIdentifier(), arr_type, field_symbol->isRef(), field_symbol->isMut()));
        }
    }
    // std::cout << "Struct visit done" << std::endl;
}

// 访问 Item
void ConstEvaluator::visit(Item& node) {
    if (node.item) {
        node.item->accept(this);
    }
}

// 访问 Struct - 处理结构体符号
void ConstEvaluator::visit(Struct& node) {
    if (node.struct_struct) {
        node.struct_struct->accept(this);
    }
}


void ConstEvaluator::visit(Enumeration& node) {
    // 枚举不会创建新的 scope，直接访问其内容
    if (node.enum_variants) {
        node.enum_variants->accept(this);
    }
}

void ConstEvaluator::visit(Trait& node) {

    // std::cout << "visit trait: " << node.identifier << std::endl;

    auto trait_symbol = std::make_shared<TraitSymbol>(node.identifier);
    
    // 处理关联项
    for (auto& item : node.associated_item) {
        if (item) {
            if (item->child) {
                if (auto const_item = std::dynamic_pointer_cast<ConstantItem>(item->child)) {
                    std::string type_str = "unknown";
                    if (const_item->type) {
                        type_str = typeToString(const_item->type);
                    }
                    if (type_str[0] == '[') {
                        type_str = handleArraySymbol(const_item->type);
                    }
                    auto const_symbol = std::make_shared<ConstSymbol>(const_item->identifier, type_str);
                    const_symbol->setValue(createConstValueFromExpression(const_item->expression));
                    trait_symbol->addConstSymbol(const_symbol);
                } else if (auto func = std::dynamic_pointer_cast<Function>(item->child)) {
                    // std::cout << "trait func " << func->identifier << std::endl;
                    std::string return_type_str = "()";
                    if (func->function_return_type && func->function_return_type->type) {
                        return_type_str = typeToString(func->function_return_type->type);
                    }
                    // 分析 self 参数类型
                    MethodType method_type = MethodType::NOT_METHOD;
                    if (func->function_parameters && func->function_parameters->self_param) {
                        if (auto shorthand_self = std::dynamic_pointer_cast<ShorthandSelf>(func->function_parameters->self_param->child)) {
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
                        } else if (auto typed_self = std::dynamic_pointer_cast<TypedSelf>(func->function_parameters->self_param->child)) {
                            // 处理带类型注解的 self: self: Type, mut self: Type
                            if (typed_self->is_mutable) {
                                method_type = MethodType::SELF_MUT_VALUE; // mut self: Type
                            } else {
                                method_type = MethodType::SELF_VALUE;     // self: Type
                            }
                        }
                    }
                    
                    auto func_symbol = std::make_shared<FuncSymbol>(func->identifier, return_type_str, func->is_const, method_type);

                    // 访问函数参数
                    if (func->function_parameters) {
                        // 将参数添加到函数符号中
                        for (auto& param : func->function_parameters->function_param) {
                            if (param) {
                                auto var_symbol = createVariableSymbolFromPattern(param->pattern_no_top_alt, param->type);
                                if (var_symbol) {
                                    if (var_symbol->getType()[0] == '[') {
                                        auto new_type = handleArraySymbol(param->type);
                                        var_symbol->setType(new_type);
                                    }
                                    func_symbol->addParameter(var_symbol);
                                }
                            }
                        }
                    }
                    
                    if (method_type != MethodType::NOT_METHOD) {
                        trait_symbol->addMethod(func_symbol);
                    } else {
                        trait_symbol->addAssociatedFunction(func_symbol);
                    }
                }
            }
        }
    }

    // std::cout << "done!" << std::endl;

    current_scope->addTraitSymbol(node.identifier, trait_symbol);

    // trait 会创建新的 scope，需要进入
    auto prev_scope = current_scope;

    current_scope = current_scope->getChild();
    
    for (auto& item : node.associated_item) {
        if (item) {
            item->accept(this);
        }
    }

    current_scope = prev_scope;
    current_scope->nextChild();
}

void ConstEvaluator::visit(Implementation& node) {
    if (node.impl) {
        node.impl->accept(this);
    }
}

void ConstEvaluator::visit(InherentImpl& node) {
    // 进入 impl 作用域
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

void ConstEvaluator::visit(TraitImpl& node) {
    // 进入 impl 作用域
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

void ConstEvaluator::visit(AssociatedItem& node) {
    if (node.child) {
        node.child->accept(this);
    }
}

// 函数相关节点
void ConstEvaluator::visit(FunctionParameters& node) {
    if (node.self_param) {
        node.self_param->accept(this);
    }
    for (auto& param : node.function_param) {
        if (param) {
            param->accept(this);
        }
    }
}

void ConstEvaluator::visit(SelfParam& node) {
    if (node.child) {
        node.child->accept(this);
    }
}

void ConstEvaluator::visit(ShorthandSelf& node) {
    // ShorthandSelf 不包含类型信息，无需类型检查
}

void ConstEvaluator::visit(TypedSelf& node) {
    if (node.type) {
        node.type->accept(this);
    }
}

void ConstEvaluator::visit(FunctionParam& node) {
    if (node.type) {
        node.type->accept(this);
    }
    if (node.pattern_no_top_alt) {
        node.pattern_no_top_alt->accept(this);
    }
}

void ConstEvaluator::visit(FunctionReturnType& node) {
    if (node.type) {
        node.type->accept(this);
    }
}

void ConstEvaluator::visit(StructFields& node) {
    for (auto& field : node.struct_fields) {
        if (field) {
            field->accept(this);
        }
    }
}

void ConstEvaluator::visit(StructField& node) {
    if (node.type) {
        node.type->accept(this);
    }
}

// 枚举相关节点
void ConstEvaluator::visit(EnumVariants& node) {
    for (auto& variant : node.enum_variant) {
        if (variant) {
            variant->accept(this);
        }
    }
}

void ConstEvaluator::visit(EnumVariant& node) {
    // EnumVariant 不包含类型信息，无需类型检查
}

// 语句类节点
void ConstEvaluator::visit(Statement& node) {
    if (node.child) {
        node.child->accept(this);
    }
}

void ConstEvaluator::visit(LetStatement& node) {
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

void ConstEvaluator::visit(ExpressionStatement& node) {
    if (node.child) {
        node.child->accept(this);
    }
}

void ConstEvaluator::visit(Statements& node) {
    for (auto& stmt : node.statements) {
        if (stmt) {
            stmt->accept(this);
        }
    }
}

// 表达式类节点
void ConstEvaluator::visit(Expression& node) {
    if (node.child) {
        node.child->accept(this);
    }
}

void ConstEvaluator::visit(ExpressionWithoutBlock& node) {
    if (node.child) {
        node.child->accept(this);
    }
}

void ConstEvaluator::visit(ExpressionWithBlock& node) {
    if (node.child) {
        node.child->accept(this);
    }
}

// 字面量表达式
void ConstEvaluator::visit(CharLiteral& node) {
    // 字面量不包含类型信息，无需类型检查
}

void ConstEvaluator::visit(StringLiteral& node) {
    // 字面量不包含类型信息，无需类型检查
}

void ConstEvaluator::visit(RawStringLiteral& node) {
    // 字面量不包含类型信息，无需类型检查
}

void ConstEvaluator::visit(CStringLiteral& node) {
    // 字面量不包含类型信息，无需类型检查
}

void ConstEvaluator::visit(RawCStringLiteral& node) {
    // 字面量不包含类型信息，无需类型检查
}

void ConstEvaluator::visit(IntegerLiteral& node) {
    // 字面量不包含类型信息，无需类型检查
}

void ConstEvaluator::visit(BoolLiteral& node) {
    // 字面量不包含类型信息，无需类型检查
}

// 路径和访问表达式
void ConstEvaluator::visit(PathExpression& node) {
    if (node.path_in_expression) {
        node.path_in_expression->accept(this);
    }
}

void ConstEvaluator::visit(FieldExpression& node) {
    if (node.expression) {
        node.expression->accept(this);
    }
}

// 运算符表达式
void ConstEvaluator::visit(UnaryExpression& node) {
    if (node.expression) {
        node.expression->accept(this);
    }
}

void ConstEvaluator::visit(BorrowExpression& node) {
    if (node.expression) {
        node.expression->accept(this);
    }
}

void ConstEvaluator::visit(DereferenceExpression& node) {
    if (node.expression) {
        node.expression->accept(this);
    }
}

void ConstEvaluator::visit(BinaryExpression& node) {
    if (node.lhs) {
        node.lhs->accept(this);
    }
    if (node.rhs) {
        node.rhs->accept(this);
    }
}

void ConstEvaluator::visit(AssignmentExpression& node) {
    if (node.lhs) {
        node.lhs->accept(this);
    }
    if (node.rhs) {
        node.rhs->accept(this);
    }
}

void ConstEvaluator::visit(CompoundAssignmentExpression& node) {
    if (node.lhs) {
        node.lhs->accept(this);
    }
    if (node.rhs) {
        node.rhs->accept(this);
    }
}

void ConstEvaluator::visit(TypeCastExpression& node) {
    if (node.expression) {
        node.expression->accept(this);
    }
    if (node.type) {
        node.type->accept(this);
    }
}

// 调用和索引表达式
void ConstEvaluator::visit(CallExpression& node) {
    if (node.expression) {
        node.expression->accept(this);
    }
    if (node.call_params) {
        node.call_params->accept(this);
    }
}

void ConstEvaluator::visit(MethodCallExpression& node) {
    if (node.expression) {
        node.expression->accept(this);
    }
    if (node.call_params) {
        node.call_params->accept(this);
    }
}

void ConstEvaluator::visit(IndexExpression& node) {
    if (node.base_expression) {
        node.base_expression->accept(this);
    }
    if (node.index_expression) {
        node.index_expression->accept(this);
    }
}

// 结构体和数组表达式
void ConstEvaluator::visit(StructExpression& node) {
    if (node.path_in_expression) {
        node.path_in_expression->accept(this);
    }
    if (node.struct_expr_fields) {
        node.struct_expr_fields->accept(this);
    }
}

void ConstEvaluator::visit(ArrayExpression& node) {
    if (node.array_elements) {
        node.array_elements->accept(this);
    }
}

void ConstEvaluator::visit(GroupedExpression& node) {
    if (node.expression) {
        node.expression->accept(this);
    }
}

// 控制流表达式
void ConstEvaluator::visit(BlockExpression& node) {
    // BlockExpression 会创建新的 scope，需要进入
    auto prev_scope = current_scope;
    current_scope = current_scope->getChild();
    
    if (node.statements) {
        node.statements->accept(this);
    }
    
    current_scope = prev_scope;
    current_scope->nextChild();
}

void ConstEvaluator::visit(IfExpression& node) {
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

void ConstEvaluator::visit(LoopExpression& node) {
    if (node.child) {
        node.child->accept(this);
    }
}

void ConstEvaluator::visit(InfiniteLoopExpression& node) {
    // InfiniteLoopExpression 会创建新的 scope，需要进入
    auto prev_scope = current_scope;
    current_scope = current_scope->getChild();
    
    if (node.block_expression) {
        node.block_expression->accept(this);
    }
    
    current_scope = prev_scope;
    current_scope->nextChild();
}

void ConstEvaluator::visit(PredicateLoopExpression& node) {
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

void ConstEvaluator::visit(BreakExpression& node) {
    if (node.expression) {
        node.expression->accept(this);
    }
}

void ConstEvaluator::visit(ContinueExpression& node) {
    // ContinueExpression 不包含类型信息，无需类型检查
}

void ConstEvaluator::visit(ReturnExpression& node) {
    if (node.expression) {
        node.expression->accept(this);
    }
}

// 辅助表达式节点
void ConstEvaluator::visit(Condition& node) {
    if (node.expression) {
        node.expression->accept(this);
    }
}

void ConstEvaluator::visit(ArrayElements& node) {
    for (auto& expr : node.expressions) {
        if (expr) {
            expr->accept(this);
        }
    }
}

void ConstEvaluator::visit(StructExprFields& node) {
    for (auto& field : node.struct_expr_fields) {
        if (field) {
            field->accept(this);
        }
    }
}

void ConstEvaluator::visit(StructExprField& node) {
    if (node.expression) {
        node.expression->accept(this);
    }
}

void ConstEvaluator::visit(CallParams& node) {
    for (auto& expr : node.expressions) {
        if (expr) {
            expr->accept(this);
        }
    }
}

// 模式类节点
void ConstEvaluator::visit(PatternNoTopAlt& node) {
    if (node.child) {
        node.child->accept(this);
    }
}

void ConstEvaluator::visit(IdentifierPattern& node) {
    // IdentifierPattern 不包含类型信息，无需类型检查
}

void ConstEvaluator::visit(ReferencePattern& node) {
    if (node.pattern) {
        node.pattern->accept(this);
    }
}

// 类型类节点
void ConstEvaluator::visit(Type& node) {
    if (node.child) {
        node.child->accept(this);
    }
}

void ConstEvaluator::visit(ReferenceType& node) {
    if (node.type) {
        node.type->accept(this);
    }
}

void ConstEvaluator::visit(ArrayType& node) {
    if (node.type) {
        node.type->accept(this);
    }
    if (node.expression) {
        node.expression->accept(this);
    }
}

void ConstEvaluator::visit(UnitType& node) {
    // UnitType 不包含类型信息，无需类型检查
}

// 路径类节点
void ConstEvaluator::visit(PathInExpression& node) {
    if (node.segment1) {
        node.segment1->accept(this);
    }
    if (node.segment2) {
        node.segment2->accept(this);
    }
}

void ConstEvaluator::visit(PathIdentSegment& node) {
    // PathIdentSegment 不包含类型信息，无需类型检查
}
