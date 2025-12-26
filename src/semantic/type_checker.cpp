#include "semantic/type_checker.hpp"

bool TypeChecker::canAssign(SymbolType var_type, SymbolType expr_type) {
    if (var_type == expr_type) return true;
    if ((var_type == "i32" || var_type == "u32" || var_type == "isize" || var_type == "usize") && expr_type == "integer") return true;
    return false;
}

SymbolType TypeChecker::autoDereference(SymbolType type) {
    if (type[0] == '&') {
        return type.substr(1);
    }
    return type;
}

bool TypeChecker::isIntegerType(const SymbolType& type) {
    return type == "integer" || type == "i32" || type == "u32" || type == "isize" || type == "usize";
}

TypeChecker::TypeChecker(std::shared_ptr<Scope> root_scope) {
    this->root_scope = root_scope;
}

void TypeChecker::visit(Crate& node) {
    for (auto item: node.items) {
        item->accept(this);
    }
}

void TypeChecker::visit(Item& node) {
    if (node.item) {
        node.item->accept(this);
    }
    node.type = node.item->type;
}

void TypeChecker::visit(Function& node) {
    auto prev_scope = current_scope;
    current_scope = current_scope->getChild();

    auto func_symbol = prev_scope->getFuncSymbol(node.identifier);
    auto func_params = func_symbol->getParameters();
    for (size_t _ = 0; _ < func_params.size(); ++_) {
        current_scope->addVariable(func_params[_]->getIdentifier(), func_params[_]->getType(), func_params[_]->isMut());
    }

    if (node.block_expression && current_scope) {
        node.block_expression->accept(this);
    }
    current_scope = prev_scope;
    current_scope->nextChild();
}

void TypeChecker::visit(Struct& node) {
    if (node.struct_struct) {
        node.struct_struct->accept(this);
    }
}

void TypeChecker::visit(Enumeration& node) {
    if (node.enum_variants) {
        node.enum_variants->accept(this);
    }
}

void TypeChecker::visit(ConstantItem& node) {
    if (node.expression) {
        node.expression->accept(this);
    }
}

void TypeChecker::visit(Trait& node) {
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

void TypeChecker::visit(Implementation& node) {
    if (node.impl) {
        node.impl->accept(this);
    }
}

void TypeChecker::visit(InherentImpl& node) {
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

void TypeChecker::visit(TraitImpl& node) {
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

void TypeChecker::visit(AssociatedItem& node) {
    if (node.child) {
        node.child->accept(this);
    }
    node.type = node.child->type;
}

// 函数相关节点
void TypeChecker::visit(FunctionParameters& node) {
    if (node.self_param) {
        node.self_param->accept(this);
    }
    for (auto& param : node.function_param) {
        if (param) {
            param->accept(this);
        }
    }
}

void TypeChecker::visit(SelfParam& node) {
    if (node.child) {
        node.child->accept(this);
    }
}

void TypeChecker::visit(ShorthandSelf& node) {
    // ShorthandSelf 不包含类型信息，无需类型检查
}

void TypeChecker::visit(TypedSelf& node) {
    if (node.type) {
        node.type->accept(this);
    }
}

void TypeChecker::visit(FunctionParam& node) {
    if (node.type) {
        node.type->accept(this);
    }
    if (node.pattern_no_top_alt) {
        node.pattern_no_top_alt->accept(this);
    }
}

void TypeChecker::visit(FunctionReturnType& node) {
    if (node.type) {
        node.type->accept(this);
    }
}

// 结构体相关节点
void TypeChecker::visit(StructStruct& node) {
    if (node.struct_fields) {
        node.struct_fields->accept(this);
    }
}

void TypeChecker::visit(StructFields& node) {
    for (auto& field : node.struct_fields) {
        if (field) {
            field->accept(this);
        }
    }
}

void TypeChecker::visit(StructField& node) {
    if (node.type) {
        node.type->accept(this);
    }
}

// 枚举相关节点
void TypeChecker::visit(EnumVariants& node) {
    for (auto& variant : node.enum_variant) {
        if (variant) {
            variant->accept(this);
        }
    }
}

void TypeChecker::visit(EnumVariant& node) {

}

// 语句类节点
void TypeChecker::visit(Statement& node) {
    if (node.child) {
        node.child->accept(this);
    }
    if (node.child != nullptr) node.type = node.child->type;
    else node.type = "()";
}

void TypeChecker::visit(LetStatement& node) {
    if (node.type) {
        node.type->accept(this);
    }
    if (node.expression) {
        node.expression->accept(this);
    }
    if (node.pattern_no_top_alt) {
        node.pattern_no_top_alt->accept(this);
    }
    auto var_type = typeToString_(current_scope, node.type);
    auto expr_type = node.expression->type;
    if (!canAssign(var_type, expr_type)) {
        throw std::runtime_error("Semantic: Type Error in LetStmt");
    }
    if (node.pattern_no_top_alt && node.pattern_no_top_alt->child) {
        auto identifier_patther = std::dynamic_pointer_cast<IdentifierPattern>(node.pattern_no_top_alt->child);
        if (identifier_patther) {
            auto var_identifier = identifier_patther->identifier;
            auto var_mutability = identifier_patther->is_mutable;
            current_scope->addVariable(var_identifier, var_type, var_mutability);
        }
    }
    static_cast<ASTNode&>(node).type = "()";
}

void TypeChecker::visit(ExpressionStatement& node) {
    if (node.child) {
        node.child->accept(this);
    }
    node.type = node.child->type;
}

void TypeChecker::visit(Statements& node) {
    for (auto& stmt : node.statements) {
        if (stmt) {
            stmt->accept(this);
        }
    }
}

// 表达式类节点
void TypeChecker::visit(Expression& node) {
    if (node.child) {
        node.child->accept(this);
    }
    node.type = node.child->type;
}

void TypeChecker::visit(ExpressionWithoutBlock& node) {
    if (node.child) {
        node.child->accept(this);
    }
    node.type = node.child->type;
}

void TypeChecker::visit(ExpressionWithBlock& node) {
    if (node.child) {
        node.child->accept(this);
    }
    node.type = node.child->type;
}

// 字面量表达式
void TypeChecker::visit(CharLiteral& node) {
    // 字面量不包含类型信息，无需类型检查
    node.type = "char";
}

void TypeChecker::visit(StringLiteral& node) {
    // 字面量不包含类型信息，无需类型检查
    node.type = "str";
}

void TypeChecker::visit(RawStringLiteral& node) {
    // 字面量不包含类型信息，无需类型检查
    node.type = "str";
}

void TypeChecker::visit(CStringLiteral& node) {
    // 字面量不包含类型信息，无需类型检查
    node.type = "str";
}

void TypeChecker::visit(RawCStringLiteral& node) {
    // 字面量不包含类型信息，无需类型检查
    node.type = "str";
}

void TypeChecker::visit(IntegerLiteral& node) {
    // 字面量不包含类型信息，无需类型检查
    if (node.value.length() > 3) {
        if (node.value.substr(node.value.length() - 3, 3) == "u32") {
            node.type = "u32";
        } else if (node.value.substr(node.value.length() - 3, 3) == "i32") {
            node.type = "i32";
        } else {
            node.type = "integer";
        }
    } else {
        node.type = "integer";
    }
}

void TypeChecker::visit(BoolLiteral& node) {
    // 字面量不包含类型信息，无需类型检查
    node.type = "bool";
}

// 路径和访问表达式
void TypeChecker::visit(PathExpression& node) {
    if (node.path_in_expression) {
        node.path_in_expression->accept(this);
    }
    // PathExpression 的类型就是变量的类型
    if (node.path_in_expression && node.path_in_expression->segment1) {
        auto var_name = node.path_in_expression->segment1->identifier;
        node.type = current_scope->findVariableType(var_name);
    }
}

void TypeChecker::visit(FieldExpression& node) {
    if (node.expression) {
        node.expression->accept(this);
    }
    auto var_type = current_scope->findVariableType(node.expression->type);
    auto struct_symbol = current_scope->findStructSymbol(var_type);
    if (!struct_symbol) {
        throw std::runtime_error("Semantic: FieldExpr struct not found");
    }
    if (!struct_symbol->hasField(node.identifier)) {
        throw std::runtime_error("Semantic: FieldExpr field not found");
    }
    node.mutability = node.expression->mutability;
    node.type = struct_symbol->getField(node.identifier)->getType();
}

// 运算符表达式
void TypeChecker::visit(UnaryExpression& node) {
    if (node.expression) {
        node.expression->accept(this);
    }
    
    // 根据不同的 unary_type 进行特定的类型检查
    switch (node.type) {
        case UnaryExpression::MINUS:  // -
            // '-' 可以作用到 integer（或 i32, u32, isize, usize）上
            if (!isIntegerType(node.expression->type)) {
                throw std::runtime_error("Semantic: Unary minus operator can only be applied to integer types");
            }
            // UnaryExpression 的类型与其成员 expr 相同
            static_cast<ASTNode&>(node).type = node.expression->type;
            break;
            
        case UnaryExpression::NOT:    // !
            // '!' 可以作用到 integer 或者 bool 上
            if (!isIntegerType(node.expression->type) && node.expression->type != "bool") {
                throw std::runtime_error("Semantic: Unary logical not operator can only be applied to integer or bool types");
            }
            // UnaryExpression 的类型与其成员 expr 相同
            static_cast<ASTNode&>(node).type = node.expression->type;
            break;
            
        case UnaryExpression::TRY:    // ?
            // 不用实现 '?' 相关
            throw std::runtime_error("Semantic: Try operator (?) is not supported");
            
        default:
            throw std::runtime_error("Semantic: UnaryExpression unknown unary type");
    }
}

void TypeChecker::visit(BorrowExpression& node) {
    if (node.expression) {
        node.expression->accept(this);
    }
    
    // 借用表达式的类型为引用类型
    // 根据借用表达式的属性构造引用类型
    std::string ref_prefix = "&";
    if (node.is_double) {
        ref_prefix = "&&";
    }
    if (node.is_mutable) {
        ref_prefix += "mut ";
    }
    
    // 借用表达式的类型为 &T 或 &mut T
    node.type = ref_prefix + node.expression->type;
}

void TypeChecker::visit(DereferenceExpression& node) {
    if (node.expression) {
        node.expression->accept(this);
    }
    
    // 检查表达式类型是否为引用类型
    if (node.expression->type.empty() || node.expression->type[0] != '&') {
        throw std::runtime_error("Semantic: Cannot dereference non-reference type");
    }
    
    // 解引用表达式的类型为去掉引用标记后的类型
    // 使用 autoDereference 函数来正确处理多重引用
    node.type = autoDereference(node.expression->type);
}

void TypeChecker::visit(BinaryExpression& node) {
    if (node.lhs) {
        node.lhs->accept(this);
    }
    if (node.rhs) {
        node.rhs->accept(this);
    }
    
    // 根据不同的 binary_type 进行特定的类型检查
    switch (node.binary_type) {
        // 算术运算符：要求操作数为整数类型或字符串类型（仅限 +），不能是 bool
        case BinaryExpression::PLUS:        // +
            if (node.lhs->type == "bool" || node.rhs->type == "bool") {
                throw std::runtime_error("Semantic: Arithmetic operators cannot be applied to bool type");
            }
            // 处理字符串连接
            if (node.lhs->type == "str" && node.rhs->type == "str") {
                node.type = "str";
            } else if (node.lhs->type == "str" || node.rhs->type == "str") {
                throw std::runtime_error("Semantic: String concatenation requires both operands to be string type");
            } else {
                // 处理整数类型匹配和 integer 类型推断
                if (node.lhs->type == node.rhs->type) {
                    node.type = node.lhs->type;
                } else if (node.lhs->type == "integer" && (node.rhs->type == "i32" || node.rhs->type == "u32" || node.rhs->type == "isize" || node.rhs->type == "usize")) {
                    node.type = node.rhs->type;
                } else if (node.rhs->type == "integer" && (node.lhs->type == "i32" || node.lhs->type == "u32" || node.lhs->type == "isize" || node.lhs->type == "usize")) {
                    node.type = node.lhs->type;
                } else {
                    throw std::runtime_error("Semantic: Arithmetic operators require matching types (integer or string)");
                }
            }
            break;
        case BinaryExpression::MINUS:       // -
        case BinaryExpression::STAR:        // *
        case BinaryExpression::SLASH:       // /
        case BinaryExpression::PERCENT:     // % {
            if (node.lhs->type == "bool" || node.rhs->type == "bool") {
                throw std::runtime_error("Semantic: Arithmetic operators cannot be applied to bool type");
            }
            // 处理类型匹配和 integer 类型推断
            if (node.lhs->type == node.rhs->type) {
                node.type = node.lhs->type;
            } else if (node.lhs->type == "integer" && (node.rhs->type == "i32" || node.rhs->type == "u32" || node.rhs->type == "isize" || node.rhs->type == "usize")) {
                node.type = node.rhs->type;
            } else if (node.rhs->type == "integer" && (node.lhs->type == "i32" || node.lhs->type == "u32" || node.lhs->type == "isize" || node.lhs->type == "usize")) {
                node.type = node.lhs->type;
            } else {
                throw std::runtime_error("Semantic: Arithmetic operators require matching integer types");
            }
            break;
            
        // 位运算符：要求操作数为整数类型
        case BinaryExpression::CARET:       // ^
        case BinaryExpression::AND:         // &
        case BinaryExpression::OR:          // |
        case BinaryExpression::SHL:         // <<
        case BinaryExpression::SHR:         // >>
            if (node.lhs->type == "bool" || node.rhs->type == "bool") {
                throw std::runtime_error("Semantic: Bitwise operators cannot be applied to bool type");
            }
            // 处理类型匹配和 integer 类型推断
            if (node.lhs->type == node.rhs->type) {
                node.type = node.lhs->type;
            } else if (node.lhs->type == "integer" && (node.rhs->type == "i32" || node.rhs->type == "u32" || node.rhs->type == "isize" || node.rhs->type == "usize")) {
                node.type = node.rhs->type;
            } else if (node.rhs->type == "integer" && (node.lhs->type == "i32" || node.lhs->type == "u32" || node.lhs->type == "isize" || node.lhs->type == "usize")) {
                node.type = node.lhs->type;
            } else {
                throw std::runtime_error("Semantic: Bitwise operators require matching integer types");
            }
            break;
            
        // 比较运算符：可以应用于整数类型和字符串类型，返回 bool
        case BinaryExpression::EQ_EQ:       // ==
        case BinaryExpression::NE:          // !=
        case BinaryExpression::GT:          // >
        case BinaryExpression::LT:          // <
        case BinaryExpression::GE:          // >=
        case BinaryExpression::LE:          // <= {
            // 处理类型匹配和 integer 类型推断
            if (node.lhs->type == node.rhs->type) {
                // 类型相同，直接接受
            } else if (node.lhs->type == "integer" && (node.rhs->type == "i32" || node.rhs->type == "u32" || node.rhs->type == "isize" || node.rhs->type == "usize")) {
                // integer 提升到具体类型
            } else if (node.rhs->type == "integer" && (node.lhs->type == "i32" || node.lhs->type == "u32" || node.lhs->type == "isize" || node.lhs->type == "usize")) {
                // integer 提升到具体类型
            } else {
                throw std::runtime_error("Semantic: Comparison operators require matching types (integer, string, or same types)");
            }
            node.type = "bool";
            break;
            
        // 逻辑运算符：要求操作数必须为 bool 类型，返回 bool
        case BinaryExpression::AND_AND:     // &&
        case BinaryExpression::OR_OR:       // || {
            // 检查操作数是否为 bool 类型
            if (node.lhs->type != "bool" || node.rhs->type != "bool") {
                throw std::runtime_error("Semantic: Logical operators require bool type operands");
            }
            node.type = "bool";
            break;
            
        default:
            throw std::runtime_error("Semantic: BinaryExpression unknown binary type");
    }
}

void TypeChecker::visit(AssignmentExpression& node) {
    if (node.lhs) {
        node.lhs->accept(this);
    }
    if (node.rhs) {
        node.rhs->accept(this);
    }
    
    // 获取左边表达式的类型，如果是引用类型则需要解引用
    SymbolType lhs_type = autoDereference(node.lhs->type);
    SymbolType rhs_type = node.rhs->type;
    
    // 检查类型是否兼容
    if (!canAssign(lhs_type, rhs_type)) {
        throw std::runtime_error("Semantic: Assignment type mismatch");
    }
    
    // 检查左边是否是可赋值的左值
    // 这里需要检查左边表达式是否为可修改的变量、字段访问、数组访问等
    if (auto path_expr = std::dynamic_pointer_cast<PathExpression>(node.lhs)) {
        // 简单变量赋值
        if (path_expr->path_in_expression && path_expr->path_in_expression->segment1) {
            auto var_name = path_expr->path_in_expression->segment1->identifier;
            if (!current_scope->findVariableMutable(var_name)) {
                throw std::runtime_error("Semantic: Cannot assign to immutable variable");
            }
        }
    } else if (auto field_expr = std::dynamic_pointer_cast<FieldExpression>(node.lhs)) {
        // 字段赋值，需要检查字段的可变性
        if (!field_expr->mutability) {
            throw std::runtime_error("Semantic: Cannot assign to immutable field");
        }
    } else if (auto index_expr = std::dynamic_pointer_cast<IndexExpression>(node.lhs)) {
        // 数组索引赋值，需要检查数组的可变性
        if (auto path_expr = std::dynamic_pointer_cast<PathExpression>(index_expr->base_expression)) {
            if (path_expr->path_in_expression && path_expr->path_in_expression->segment1) {
                auto var_name = path_expr->path_in_expression->segment1->identifier;
                if (!current_scope->findVariableMutable(var_name)) {
                    throw std::runtime_error("Semantic: Cannot assign to immutable array");
                }
            }
        }
    } else {
        // 其他类型的左值，暂时允许
        // TODO: 添加更多左值类型的检查
    }
    
    // 赋值表达式的类型为单元类型
    node.type = "()";
}

void TypeChecker::visit(CompoundAssignmentExpression& node) {
    if (node.lhs) {
        node.lhs->accept(this);
    }
    if (node.rhs) {
        node.rhs->accept(this);
    }
    
    // 获取左边表达式的类型，如果是引用类型则需要解引用
    SymbolType lhs_type = autoDereference(node.lhs->type);
    SymbolType rhs_type = node.rhs->type;
    
    // 检查类型是否兼容
    if (!canAssign(lhs_type, rhs_type)) {
        throw std::runtime_error("Semantic: Compound assignment type mismatch");
    }
    
    // 检查操作数是否为整数类型（复合赋值运算符只支持整数，不支持字符串）
    if (lhs_type == "bool" || rhs_type == "bool" || lhs_type == "str" || rhs_type == "str") {
        throw std::runtime_error("Semantic: Compound assignment operators cannot be applied to bool or string type");
    }
    
    // 检查左边是否是可赋值的左值
    if (auto path_expr = std::dynamic_pointer_cast<PathExpression>(node.lhs)) {
        // 简单变量赋值
        if (path_expr->path_in_expression && path_expr->path_in_expression->segment1) {
            auto var_name = path_expr->path_in_expression->segment1->identifier;
            if (!current_scope->findVariableMutable(var_name)) {
                throw std::runtime_error("Semantic: Cannot assign to immutable variable");
            }
        }
    } else if (auto field_expr = std::dynamic_pointer_cast<FieldExpression>(node.lhs)) {
        // 字段赋值，需要检查字段的可变性
        if (!field_expr->mutability) {
            throw std::runtime_error("Semantic: Cannot assign to immutable field");
        }
    } else if (auto index_expr = std::dynamic_pointer_cast<IndexExpression>(node.lhs)) {
        // 数组索引赋值，需要检查数组的可变性
        if (auto path_expr = std::dynamic_pointer_cast<PathExpression>(index_expr->base_expression)) {
            if (path_expr->path_in_expression && path_expr->path_in_expression->segment1) {
                auto var_name = path_expr->path_in_expression->segment1->identifier;
                if (!current_scope->findVariableMutable(var_name)) {
                    throw std::runtime_error("Semantic: Cannot assign to immutable array");
                }
            }
        }
    } else {
        // 其他类型的左值，暂时允许
        // TODO: 添加更多左值类型的检查
    }
    
    // 复合赋值表达式的类型为左边操作数的类型
    // 使用继承自 ASTNode 的 type 字段，而不是 CompoundAssignmentType 字段
    static_cast<ASTNode&>(node).type = lhs_type;
}

void TypeChecker::visit(TypeCastExpression& node) {
    if (node.expression) {
        node.expression->accept(this);
    }
    if (node.type) {
        node.type->accept(this);
    }
    
    // 获取源类型和目标类型
    SymbolType source_type = node.expression->type;
    SymbolType target_type = typeToString(node.type);
    
    // 检查类型转换是否合法
    bool is_valid_cast = false;
    
    // 规则1: Numeric cast - 相同大小的整数之间转换
    if (isIntegerType(source_type) && isIntegerType(target_type)) {
        is_valid_cast = true;
    }
    
    // 规则2: Primitive to integer cast
    // bool -> integer: false -> 0, true -> 1
    else if (source_type == "bool" && isIntegerType(target_type)) {
        is_valid_cast = true;
    }
    // char -> integer: 转换为 Unicode 码点值
    else if (source_type == "char" && isIntegerType(target_type)) {
        is_valid_cast = true;
    }
    
    if (!is_valid_cast) {
        throw std::runtime_error("Semantic: Invalid type cast from " + source_type + " to " + target_type);
    }
    
    // 类型转换表达式的类型就是目标类型
    // 使用继承自 ASTNode 的 type 字段
    static_cast<ASTNode&>(node).type = target_type;
}

void TypeChecker::checkFunctionParams(const std::vector<std::shared_ptr<Expression>>& call_params, const std::vector<std::shared_ptr<VariableSymbol>>& func_params) {
    if (call_params.size() != func_params.size()) {
        throw std::runtime_error("Semantic: CallExpr function param number not match");
    }
    for (size_t _ = 0; _ < call_params.size(); ++_) {
        if (call_params[_]->type != func_params[_]->getType()) {
            throw std::runtime_error("Semantic: CallExpr function param type not match");
        }
        if (func_params[_]->isMut()) {
            if (auto path_expr = std::dynamic_pointer_cast<PathExpression>(func_params[_])) {
                if (auto path_ident_seg = std::dynamic_pointer_cast<PathIdentSegment>(path_expr)) {
                    auto identifier = path_ident_seg->identifier;
                    if (!current_scope->findVariableMutable(identifier)) {
                        throw std::runtime_error("Semantic: CallExpr function param mutability not match");
                    }
                } else {
                    throw std::runtime_error("Semantic: CallExpr function param mutability not match");
                }
            } else {
                throw std::runtime_error("Semantic: CallExpr function param mutability not match");
            }
        } else {
            throw std::runtime_error("Semantic: CallExpr function param mutability not match");
        }
    }
}

// 调用和索引表达式
void TypeChecker::visit(CallExpression& node) {
    if (node.expression) {
        node.expression->accept(this);
    }
    if (node.call_params) {
        node.call_params->accept(this);
    }
    if (auto path_expr = std::dynamic_pointer_cast<PathExpression>(node.expression)) {
        auto path_in_expr = std::dynamic_pointer_cast<PathInExpression>(path_expr);
        if (path_in_expr->segment2) {
            auto var_identifier = path_in_expr->segment1->identifier;
            auto var_type = autoDereference(current_scope->findVariableType(var_identifier));
            auto struct_symbol = current_scope->findStructSymbol(var_type);
            if (struct_symbol) {
                auto func_symbol = struct_symbol->getAssociatedFunction(path_in_expr->segment2->identifier);
                if (func_symbol) {
                    if (func_symbol->isMethod()) {
                        throw std::runtime_error("Semantic: CallExpr function is a method");
                    }                    
                    auto call_params = node.call_params->expressions;
                    auto func_params = func_symbol->getParameters();
                    checkFunctionParams(call_params, func_params);
                    node.type = func_symbol->getReturnType();
                } else {
                    throw std::runtime_error("Semantic: CallExpr function not found");
                }
            } else {
                throw std::runtime_error("Semantic: CallExpr struct not found");
            }
        } else {
            auto func_symbol = current_scope->findFuncSymbol(path_in_expr->segment1->identifier);
            if (func_symbol) {
                if (func_symbol->isMethod()) {
                    throw std::runtime_error("Semantic: CallExpr function is a method");
                }
                auto call_params = node.call_params->expressions;
                auto func_params = func_symbol->getParameters();
                checkFunctionParams(call_params, func_params);
                node.type = func_symbol->getReturnType();
            } else {
                throw std::runtime_error("Semantic: CallExpr function not found");
            }
        }
    } else {
        throw std::runtime_error("Semantic: CallExpr not function");
    }
}

void TypeChecker::visit(MethodCallExpression& node) {
    if (node.expression) {
        node.expression->accept(this);
    }
    if (node.call_params) {
        node.call_params->accept(this);
    }
    if (auto path_expr = std::dynamic_pointer_cast<PathExpression>(node.expression)) {
        auto var_identifier = path_expr->path_in_expression->segment1->identifier;
        auto var_type = autoDereference(current_scope->findVariableType(var_identifier));
        auto struct_symbol = current_scope->findStructSymbol(var_type);
        if (struct_symbol) {
            auto func_symbol = struct_symbol->getAssociatedFunction(node.path_ident_segment->identifier);
            if (func_symbol) {
                if (!func_symbol->isMethod()) {
                    throw std::runtime_error("Semantic: MethodCallExpr function is not a method");
                }
                auto method_type = func_symbol->getMethodType();
                if (method_type == MethodType::SELF_MUT_REF || method_type == MethodType::SELF_MUT_VALUE) {
                    if (!current_scope->findVariableMutable(var_identifier)) {
                        throw std::runtime_error("Semantic: MethodCallExpr not mutable");
                    }
                }
                auto call_params = node.call_params->expressions;
                auto func_params = func_symbol->getParameters();
                checkFunctionParams(call_params, func_params);
                node.type = func_symbol->getReturnType();
            } else {
                throw std::runtime_error("Semantic: MethodCallExpr function not found");
            }
        } else {
            throw std::runtime_error("Semantic: MethodCallExpr struct not found");
        }
    } else {
        throw std::runtime_error("Semantic: MethodCallExpr not struct");
    }
}

void TypeChecker::visit(IndexExpression& node) {
    if (node.base_expression) {
        node.base_expression->accept(this);
    }
    if (node.index_expression) {
        node.index_expression->accept(this);
    }
    if (node.index_expression->type != "integer" && node.index_expression->type != "usize") {
        throw std::runtime_error("Semantic: IndexExpr index not usize");
    }
    auto arr_type = node.base_expression->type;
    if (arr_type[0] != '[') {
        throw std::runtime_error("Semantic: IndexExpr not array");
    }
    auto type = arr_type;
    while (type.back() != ']') type.pop_back();
    type = type.substr(1, type.length() - 2);
    type = autoDereference(type);
    node.type = type;
}

// 结构体和数组表达式
void TypeChecker::visit(StructExpression& node) {
    if (node.path_in_expression) {
        node.path_in_expression->accept(this);
    }
    if (node.struct_expr_fields) {
        node.struct_expr_fields->accept(this);
    }
    auto struct_symbol = current_scope->findStructSymbol(node.path_in_expression->segment1->identifier);
    auto struct_expr_fields = node.struct_expr_fields->struct_expr_fields;
    auto struct_fields_size = struct_symbol->getFieldSize();
    if (struct_fields_size != struct_expr_fields.size()) {
        throw std::runtime_error("Semantic: StructExpression fields size not match");
    }
    for (size_t _ = 0; _ < struct_expr_fields.size(); ++_) {
        auto identifier = struct_expr_fields[_]->identifier;
        auto type = struct_expr_fields[_]->type;
        auto struct_field = struct_symbol->getField(identifier);
        if (struct_field->getType() != type) {
            throw std::runtime_error("Semantic: StructExpression field type not match");
        }
    }
    node.type = struct_symbol->getIdentifier();
}

void TypeChecker::visit(ArrayExpression& node) {
    if (node.array_elements) {
        node.array_elements->accept(this);
    }
    node.type = node.array_elements->type;
}

void TypeChecker::visit(GroupedExpression& node) {
    if (node.expression) {
        node.expression->accept(this);
    }
    node.type = node.expression->type;
}

// 控制流表达式
void TypeChecker::visit(BlockExpression& node) {
    // BlockExpression 会创建新的 scope，需要进入
    auto prev_scope = current_scope;
    current_scope = current_scope->getChild();
    
    if (node.statements) {
        node.statements->accept(this);
    }

    // 实现尾表达式检测和类型推断
    if (node.statements && !node.statements->statements.empty()) {
        // 检测尾表达式
        std::shared_ptr<ASTNode> tail_expression = nullptr;
        
        // 从最后一个语句开始向前查找尾表达式
        auto it = node.statements->statements.rbegin();
        auto stmt = *it;
        // 检查是否为 ExpressionStatement
        if (auto expr_stmt = std::dynamic_pointer_cast<ExpressionStatement>(stmt)) {
            if (!expr_stmt->has_semi) {
                // 没有分号的 ExpressionStatement 是尾表达式
                tail_expression = expr_stmt->child;
            }
        }
        
        if (tail_expression) {
            // 有尾表达式的情况，使用该表达式的类型
            node.type = tail_expression->type;
        } else {
            // 没有尾表达式，需要判断是否为 ! 类型
            auto last_stmt = node.statements->statements.back();
            std::string last_stmt_type = last_stmt ? last_stmt->type : "()";
            
            // 检查最后一句是否是 break | continue | return
            bool is_never_type = false;
            if (last_stmt_type == "!") {
                is_never_type = true;
            }
            
            if (is_never_type) {
                node.type = "!";
            } else {
                node.type = "()";
            }
        }
    } else {
        // 没有语句，类型为 ()
        node.type = "()";
    }
    
    current_scope = prev_scope;
    current_scope->nextChild();
}

void TypeChecker::visit(IfExpression& node) {
    if (node.condition) {
        node.condition->accept(this);
    }
    
    // 检查条件必须是 bool 类型
    if (node.condition->type != "bool") {
        throw std::runtime_error("Semantic: If condition must be bool type");
    }
    
    if (node.then_block) {
        node.then_block->accept(this);
    }
    if (node.else_branch) {
        node.else_branch->accept(this);
    }
    
    // 类型推断逻辑
    std::string then_type = node.then_block ? node.then_block->type : "()";
    std::string else_type = node.else_branch ? node.else_branch->type : "()";
    
    // 处理 never 类型（!）
    bool then_is_never = (then_type == "!");
    bool else_is_never = (else_type == "!");
    
    if (then_is_never && else_is_never) {
        // 两个分支都是 never，结果是 never
        node.type = "!";
    } else if (then_is_never) {
        // then 分支是 never，结果是 else 分支类型
        node.type = else_type;
    } else if (else_is_never) {
        // else 分支是 never，结果是 then 分支类型
        node.type = then_type;
    } else {
        // 两个分支都不是 never，需要类型兼容
        bool types_compatible = false;
        
        // 如果类型相同，则兼容
        if (then_type == else_type) {
            types_compatible = true;
            node.type = then_type;
        }
        // 处理 integer 类型推断
        else if (then_type == "integer" && isIntegerType(else_type)) {
            types_compatible = true;
            node.type = else_type; // 使用具体类型
        }
        else if (else_type == "integer" && isIntegerType(then_type)) {
            types_compatible = true;
            node.type = then_type; // 使用具体类型
        }
        
        if (!types_compatible) {
            throw std::runtime_error("Semantic: If expression branches have incompatible types: then branch is " + then_type + ", else branch is " + else_type);
        }
    }
}

void TypeChecker::visit(LoopExpression& node) {
    if (node.child) {
        node.child->accept(this);
    }
    node.type = node.child->type;
}

void TypeChecker::visit(InfiniteLoopExpression& node) {
    // InfiniteLoopExpression 会创建新的 scope，需要进入
    auto prev_scope = current_scope;
    current_scope = current_scope->getChild();
    
    if (node.block_expression) {
        node.block_expression->accept(this);
    }
    
    // 获取 LOOP scope 的 break_type 作为 InfiniteLoopExpression 的类型
    std::string break_type = current_scope->getBreakType();
    if (break_type.empty()) {
        // 如果没有 break 语句，类型为 ()
        node.type = "()";
    } else {
        node.type = break_type;
    }
    
    current_scope = prev_scope;
    current_scope->nextChild();
}

void TypeChecker::visit(PredicateLoopExpression& node) {
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

    node.type = "()";
}

void TypeChecker::visit(BreakExpression& node) {
    if (node.expression) {
        node.expression->accept(this);
    }

    node.type = "!";

    auto scope = current_scope;
    bool in_loop = false;
    while (scope) {
        if (scope->getType() == ScopeType::LOOP) {
            in_loop = true;
            break;
        }
        scope = scope->getParent();
    }
    if (!in_loop) {
        throw std::runtime_error("Control Flow: Break not in loop");
    }

    // 记录 break 表达式的类型到 LOOP scope
    std::string break_expr_type = node.expression ? node.expression->type : "()";
    std::string current_break_type = scope->getBreakType();
    
    if (current_break_type.empty()) {
        // 第一次设置 break_type
        scope->setBreakType(break_expr_type);
    } else {
        // 检查类型唯一性，考虑 integer 类型的影响
        bool types_compatible = false;
        
        // 如果类型相同，则兼容
        if (current_break_type == break_expr_type) {
            types_compatible = true;
        }
        // 如果一个是 integer，另一个是具体整型，则兼容
        else if (current_break_type == "integer" && isIntegerType(break_expr_type)) {
            types_compatible = true;
            // 将 break_type 更新为具体类型
            scope->setBreakType(break_expr_type);
        }
        else if (break_expr_type == "integer" && isIntegerType(current_break_type)) {
            types_compatible = true;
            // 保持当前的具体类型
        }
        
        if (!types_compatible) {
            throw std::runtime_error("Semantic: Break expression type mismatch: expected " + current_break_type + ", got " + break_expr_type);
        }
    }
}

void TypeChecker::visit(ContinueExpression& node) {
    node.type = "!";

    auto scope = current_scope;
    bool in_loop = false;
    while (scope) {
        if (scope->getType() == ScopeType::LOOP) {
            in_loop = true;
            break;
        }
        scope = scope->getParent();
    }
    if (!in_loop) {
        throw std::runtime_error("Control Flow: Continue not in loop");
    }
}

void TypeChecker::visit(ReturnExpression& node) {
    if (node.expression) {
        node.expression->accept(this);
    }

    // 查找包含当前 return 语句的函数作用域
    auto scope = current_scope;
    std::string func_name = "";
    
    // 向上遍历作用域链，找到第一个 FUNCTION 类型的作用域
    while (scope) {
        if (scope->getType() == ScopeType::FUNCTION) {
            // 获取函数名
            func_name = scope->getSelfType();
            break;
        }
        scope = scope->getParent();
    }
    
    if (!func_name.empty()) {
        // 找到函数符号
        auto func_scope = current_scope;
        while (func_scope && func_scope->getType() != ScopeType::FUNCTION) {
            func_scope = func_scope->getParent();
        }
        func_scope = func_scope->getParent();
        
        if (func_scope) {
            auto func_symbol = func_scope->getFuncSymbol(func_name);
            if (func_symbol) {
                auto return_type = func_symbol->getReturnType();
                
                // 检查返回表达式类型是否与函数返回类型匹配
                if (node.expression) {
                    auto expr_type = node.expression->type;
                    if (return_type != expr_type) {
                        throw std::runtime_error("Semantic: Return type mismatch: expected " + return_type + ", got " + expr_type);
                    }
                } else {
                    // 无返回值的函数，返回类型必须是 "()"
                    if (return_type != "()") {
                        throw std::runtime_error("Semantic: Return type mismatch: expected " + return_type + ", got ()");
                    }
                }
            }
        }
    }
    
    // ReturnExpression 的类型为 "!" (never type)
    node.type = "!";
}

// 辅助表达式节点
void TypeChecker::visit(Condition& node) {
    if (node.expression) {
        node.expression->accept(this);
    }
    if (node.expression->type != "bool") {
        throw std::runtime_error("Semantic: Condition not bool");
    }
    node.type = "bool";
}

void TypeChecker::visit(ArrayElements& node) {
    for (auto& expr : node.expressions) {
        if (expr) {
            expr->accept(this);
        }
    }
    if (node.is_semicolon_separated) {
        auto len = createConstValueFromExpression(current_scope, node.expressions[1]);
        if (!len->isInt()) {
            throw std::runtime_error("Semantic: Array length not integer");
        }
        auto len_int = std::dynamic_pointer_cast<ConstValueInt>(len);
        SymbolType type = '[' + node.expressions[0]->type + ']' + std::to_string(len_int->getValue());
        node.type = type;
    } else {
        SymbolType base_type = node.expressions[0]->type;
        for (auto & expr : node.expressions) {
            if (base_type == "integer" && (expr->type == "i32" || expr->type == "u32" || expr->type == "isize" || expr->type == "usize")) {
                base_type = expr->type;
            } else if (base_type != expr->type) {
                throw std::runtime_error("Semantic: Array expr type not match");
            }
        }
        SymbolType type = '[' + base_type + ']' + std::to_string(node.expressions.size());
        node.type = type;
    }
}

void TypeChecker::visit(StructExprFields& node) {
    for (auto& field : node.struct_expr_fields) {
        if (field) {
            field->accept(this);
        }
    }
}

void TypeChecker::visit(StructExprField& node) {
    if (node.expression) {
        node.expression->accept(this);
    }
}

void TypeChecker::visit(CallParams& node) {
    for (auto& expr : node.expressions) {
        if (expr) {
            expr->accept(this);
        }
    }
}

// 模式类节点
void TypeChecker::visit(PatternNoTopAlt& node) {
    if (node.child) {
        node.child->accept(this);
    }
    node.type = node.child->type;
}

void TypeChecker::visit(IdentifierPattern& node) {
    // IdentifierPattern 不包含类型信息，无需类型检查
    node.type = current_scope->findVariableType(node.identifier);
}

void TypeChecker::visit(ReferencePattern& node) {
    if (node.pattern) {
        node.pattern->accept(this);
    }
    node.type = "&" + node.pattern->type;
    if (node.is_mutable) node.mutability = true; 
}

// 类型类节点
void TypeChecker::visit(Type& node) {
    if (node.child) {
        node.child->accept(this);
    }
    node.type = typeToString_(current_scope, std::make_shared<Type>(node));
}

void TypeChecker::visit(ReferenceType& node) {
    if (node.type) {
        node.type->accept(this);
    }
}

void TypeChecker::visit(ArrayType& node) {
    if (node.type) {
        node.type->accept(this);
    }
    if (node.expression) {
        node.expression->accept(this);
    }
}

void TypeChecker::visit(UnitType& node) {
    // UnitType 不包含类型信息，无需类型检查
}

// 路径类节点
void TypeChecker::visit(PathInExpression& node) {
    if (node.segment1) {
        node.segment1->accept(this);
    }
    if (node.segment2) {
        node.segment2->accept(this);
        node.type = node.segment1->type + "::" + node.segment2->type;
    } else {
        node.type = node.segment1->type;
    }
}

void TypeChecker::visit(PathIdentSegment& node) {
    // PathIdentSegment 不包含类型信息，无需类型检查
    if (node.path_type == 0) {
        node.type = node.identifier;
    } else if (node.path_type == 1) {
        node.type = "Self";
    } else {
        node.type = "self";
    }
}
