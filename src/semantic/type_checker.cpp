#include "semantic/type_checker.hpp"
#include <iostream>

std::pair<std::string, std::string> TypeChecker::getBaseType(const SymbolType& type) {
    std::string base_type = "", len_type = "";
    size_t pos = 0;
    while (pos < type.length() && type[pos] == '[') {
        len_type += "[";
        pos++;
    }
    while (pos < type.length() && type[pos] != ']') {
        base_type += type[pos];
        pos++;
    }
    while (pos < type.length()) {
        len_type += type[pos];
        pos++;
    }
    return std::make_pair(base_type, len_type);
}

bool TypeChecker::canAssign(SymbolType var_type, SymbolType expr_type) {
    var_type = autoDereference(var_type), expr_type = autoDereference(expr_type);
    auto res1 = getBaseType(var_type), res2 = getBaseType(expr_type);
    if (res1 == res2) return true;
    if (((res1.first == "i32" || res1.first == "u32" || res1.first == "isize" || res1.first == "usize") && res2.first == "integer") && (res1.second == res2.second)) return true;
    if (((res1.first == "usize" && res2.first == "u32") || (res1.first == "u32" && res2.first == "usize")) && (res1.second == res2.second)) return true;
    if (((res1.first == "isize" && res2.first == "i32") || (res1.first == "i32" && res2.first == "isize")) && (res1.second == res2.second)) return true;
    if (res2.first == "!") return true;
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
    exit_num = 0;
    this->root_scope = root_scope;
    this->current_scope = root_scope;
}

void TypeChecker::visit(Crate& node) {
    std::cout << "[TypeChecker] Entering Crate node" << std::endl;
    for (auto item: node.items) {
        item->accept(this);
    }
    if (exit_num > 1) {
        throw std::runtime_error("Semantic: more than 1 exit!");
    }
}

void TypeChecker::visit(Item& node) {
    std::cout << "[TypeChecker] Entering Item node" << std::endl;
    if (node.item) {
        node.item->accept(this);
    }
    node.type = node.item->type;
}

void TypeChecker::visit(Function& node) {
    std::cout << "[TypeChecker] Entering Function node: " << node.identifier << std::endl;
    auto prev_scope = current_scope;
    current_scope = current_scope->getChild();

    auto func_symbol = prev_scope->getFuncSymbol(node.identifier);
    auto func_params = func_symbol->getParameters();
    for (size_t _ = 0; _ < func_params.size(); ++_) {
        current_scope->addVariable(func_params[_]->getIdentifier(), func_params[_]->getType(), func_params[_]->isMut());
    }
    if (func_symbol->getMethodType() == MethodType::SELF_VALUE || func_symbol->getMethodType() == MethodType::SELF_REF) {
        auto self_type = current_scope->getImplSelfType();
        current_scope->addVariable("self", self_type, false);
    } else if (func_symbol->getMethodType() == MethodType::SELF_MUT_VALUE || func_symbol->getMethodType() == MethodType::SELF_MUT_REF) {
        auto self_type = current_scope->getImplSelfType();
        current_scope->addVariable("self", self_type, true);
    } 

    if (node.identifier == "main" && func_symbol->getReturnType() != "()") {
        throw std::runtime_error("Semantic: Function main should return ()");
    }

    if (node.block_expression && current_scope) {
        node.block_expression->accept(this);
    }

    if (node.identifier == "main") {
        auto block_expr = node.block_expression;
        if (!block_expr || !block_expr->statements) {
            throw std::runtime_error("Semantic: exit missing0");
        }
        auto stmts = block_expr->statements->statements;
        if (stmts.empty()) {
            throw std::runtime_error("Semantic: exit wrong place1");
        }
        std::shared_ptr<ExpressionWithoutBlock> expr_without_block = std::dynamic_pointer_cast<ExpressionWithoutBlock>(stmts.back());
        if (!expr_without_block) {
            auto stmt = std::dynamic_pointer_cast<Statement>(stmts.back());
            auto expr_stmt = std::dynamic_pointer_cast<ExpressionStatement>(stmt->child);
            expr_without_block = std::dynamic_pointer_cast<ExpressionWithoutBlock>(expr_stmt->child);
            if (!expr_without_block) {
                throw std::runtime_error("Semantic: exit wrong place2");
            }
        }
        auto call_expr = std::dynamic_pointer_cast<CallExpression>(expr_without_block->child);
        if (!call_expr) {
            throw std::runtime_error("Semantic: exit wrong place3");
        }
        auto path_expr = std::dynamic_pointer_cast<PathExpression>(call_expr->expression);
        auto path_in_expr = std::dynamic_pointer_cast<PathInExpression>(path_expr->path_in_expression);
        auto identifier = path_in_expr->segment1->identifier;
        if (identifier != "exit") {
            throw std::runtime_error("Semantic: exit missing!");
        }
    }

    if (node.block_expression) {
        auto return_type = node.block_expression->type;
        // std::cout << func_symbol->getReturnType() << ' ' << return_type << ' ' << node.block_expression->is_last_stmt_return << std::endl;
        if (!canAssign(func_symbol->getReturnType(), return_type) && (!node.block_expression->is_last_stmt_return)) {
            throw std::runtime_error("Semantic: Function return type not match " + func_symbol->getIdentifier());
        }
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
    std::cout << "[TypeChecker] Entering Trait node" << std::endl;
    auto prev_scope = current_scope;

    current_scope = current_scope->getChild();
    std::cout << "GOOD" << std::endl;
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
    std::cout << "[TypeChecker] Entering AssociatedItem node" << std::endl;
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
    std::cout << "[TypeChecker] Entering LetStatement node" << std::endl;
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
    std::cout << "[TypeChecker] LetStatement: var_type = " << var_type << ", expr_type = " << expr_type << std::endl;
    if (!canAssign(var_type, expr_type)) {
        throw std::runtime_error("Semantic: Type Error in LetStmt");
    }
    if (node.pattern_no_top_alt && node.pattern_no_top_alt->child) {
        auto identifier_patther = std::dynamic_pointer_cast<IdentifierPattern>(node.pattern_no_top_alt->child);
        if (identifier_patther) {
            auto var_identifier = identifier_patther->identifier;
            auto var_mutability = identifier_patther->is_mutable;
            current_scope->addVariable(var_identifier, var_type, var_mutability);
            std::cout << "[TypeChecker] LetStatement: added variable " << var_identifier << " with type " << var_type << std::endl;
        }
    }
    static_cast<ASTNode&>(node).type = "()";
}

void TypeChecker::visit(ExpressionStatement& node) {
    if (node.child) {
        node.child->accept(this);
    }
    node.mutability = node.child->mutability;
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
    std::cout << "[TypeChecker] Entering Expression node" << std::endl;
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
    std::cout << "[TypeChecker] Entering PathExpr node" << std::endl;
    if (node.path_in_expression) {
        node.path_in_expression->accept(this);
    }
    if (node.path_in_expression && node.path_in_expression->segment2) {
        auto struct_name = node.path_in_expression->segment1->identifier;
        if (struct_name == "Self") {
            struct_name = current_scope->getImplSelfType();
        }
        if (auto struct_symbol = current_scope->findStructSymbol(struct_name)) {
            if (auto const_symbol = struct_symbol->getAssociatedConst(node.path_in_expression->segment2->identifier)) {
                node.type = const_symbol->getType();
            }
        } else if (current_scope->enumSymbolExists(struct_name)) {
            node.type = struct_name;
        }
    } else if (node.path_in_expression && node.path_in_expression->segment1) {
        auto var_name = node.path_in_expression->segment1->identifier;
        if (current_scope->constSymbolExists(var_name)) {
            auto const_symbol = current_scope->findConstSymbol(var_name);
            node.type = const_symbol->getType();
        } else if (current_scope->variableExists(var_name)) {
            node.type = current_scope->findVariableType(var_name);
        } else {
            node.type = node.path_in_expression->type;
        }
        node.mutability = node.path_in_expression->mutability;
    }
}

void TypeChecker::visit(FieldExpression& node) {
    std::cout << "[TypeChecker] Entering FieldExpression node" << std::endl;
    if (node.expression) {
        node.expression->accept(this);
    }
    auto deref_type = autoDereference(node.expression->type);
    std::shared_ptr<StructSymbol> struct_symbol;
    struct_symbol = current_scope->findStructSymbol(deref_type);
    if (!struct_symbol) {
        throw std::runtime_error("Semantic: FieldExpr struct not found");
    }
    if (!struct_symbol->hasField(node.identifier)) {
        throw std::runtime_error("Semantic: FieldExpr field not found");
    }
    node.mutability = node.expression->mutability;
    // std::cout << node.expression->mutability << std::endl;
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
        // ref_prefix += "mut ";
        node.mutability = true;
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
    std::cout << "[TypeChecker] Entering BinaryExpression node" << std::endl;
    if (node.lhs) {
        node.lhs->accept(this);
    }
    if (node.rhs) {
        node.rhs->accept(this);
    }
    
    std::cout << "[TypeChecker] BinaryExpression: LHS type = " << node.lhs->type << ", RHS type = " << node.rhs->type << std::endl;
    
    // 根据不同的 binary_type 进行特定的类型检查
    switch (node.binary_type) {
        // 算术运算符：要求操作数为整数类型或字符串类型（仅限 +），不能是 bool
        case BinaryExpression::PLUS:        // +
            if (node.lhs->type == "bool" || node.rhs->type == "bool") {
                throw std::runtime_error("Semantic: Arithmetic operators cannot be applied to bool type");
            }
            // 处理字符串连接
            if (node.lhs->type == "String" && node.lhs->mutability && node.rhs->type == "str") {
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
    
    std::cout << "[TypeChecker] BinaryExpression result type: " << node.type << std::endl;
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
    // std::cout << call_params.size() << ' ' << func_params.size() << std::endl;
    if (call_params.size() != func_params.size()) {
        throw std::runtime_error("Semantic: CallExpr function param number not match");
    }
    for (size_t _ = 0; _ < call_params.size(); ++_) {
        if (!canAssign(func_params[_]->getType(), call_params[_]->type)) {
            std::cout << func_params[_]->getType() << ' ' << call_params[_]->type << std::endl;
            throw std::runtime_error("Semantic: CallExpr function param type not match");
        }
        if (func_params[_]->isMut()) {
            if (auto path_expr = std::dynamic_pointer_cast<PathExpression>(call_params[_]->child)) {
                if (auto path_in_expr = std::dynamic_pointer_cast<PathInExpression>(path_expr->path_in_expression)) {
                    auto identifier = path_in_expr->segment1->identifier;
                    if (!current_scope->findVariableMutable(identifier)) {
                        std::cout << identifier << std::endl;
                        throw std::runtime_error("Semantic: CallExpr function param mutability not match1");
                    }
                } else {
                    throw std::runtime_error("Semantic: CallExpr function param mutability not match2");
                }
            } else {
                if (auto borrow_expr = std::dynamic_pointer_cast<BorrowExpression>(call_params[_]->child)) {
                    if (!borrow_expr->is_mutable) {
                        throw std::runtime_error("Semantic: CallExpr function param mutability not match3");
                    }
                } else {
                    throw std::runtime_error("Semantic: CallExpr function param mutability not match4");
                }
            }
        }
    }
}

// 调用和索引表达式
void TypeChecker::visit(CallExpression& node) {
    std::cout << "[TypeChecker] Entering CallExpression node" << std::endl;
    if (node.expression) {
        node.expression->accept(this);
    }
    if (node.call_params) {
        node.call_params->accept(this);
    }
    if (auto path_expr = std::dynamic_pointer_cast<PathExpression>(node.expression)) {
        // std::cout << "GOOD" << std::endl;
        auto path_in_expr = std::dynamic_pointer_cast<PathInExpression>(path_expr->path_in_expression);
        // std::cout << "GOOD" << std::endl;
        // std::cout << (path_in_expr == nullptr) << std::endl;
        if (path_in_expr->segment2) {
            // std::cout << "?" << std::endl;
            auto struct_identifier = path_in_expr->segment1->identifier;
            // std::cout << struct_identifier << std::endl;
            auto struct_symbol = current_scope->findStructSymbol(struct_identifier);
            if (struct_symbol) {
                // std::cout << path_in_expr->segment2->identifier << std::endl;
                auto func_symbol = struct_symbol->getAssociatedFunction(path_in_expr->segment2->identifier);
                if (func_symbol) {
                    if (func_symbol->isMethod()) {
                        throw std::runtime_error("Semantic: CallExpr function is a method");
                    }
                    auto func_params = func_symbol->getParameters();
                    if (node.call_params) {
                        auto call_params = node.call_params->expressions;
                        checkFunctionParams(call_params, func_params);
                    } else {
                        if (!func_params.empty()) {
                            throw std::runtime_error("Semantic: CallExpr param number not match");
                        }
                    }
                    node.type = func_symbol->getReturnType();
                    std::cout << "[TypeChecker] CallExpression to associated function: " << path_in_expr->segment2->identifier << ", return type: " << node.type << std::endl;
                } else {
                    throw std::runtime_error("Semantic: CallExpr function not found");
                }
            } else {
                throw std::runtime_error("Semantic: CallExpr struct not found");
            }
        } else {
            // std::cout << "!" << std::endl;
            auto identifier = path_in_expr->segment1->identifier;
            if (identifier == "exit") {
                exit_num++;
                // std::cout << "?" << std::endl;
                auto scope = current_scope;
                while (scope && scope->getType() != ScopeType::FUNCTION) {
                    scope = scope->getParent();
                }
                if (!scope) {
                    throw std::runtime_error("Semantic: exit wrong place");
                }
                if (scope->getSelfType() != "main") {
                    throw std::runtime_error("Semantic: exit wrong place");
                }
            }
            auto func_symbol = current_scope->findFuncSymbol(path_in_expr->segment1->identifier);
            if (func_symbol) {
                if (func_symbol->isMethod()) {
                    throw std::runtime_error("Semantic: CallExpr function is a method");
                }
                auto func_params = func_symbol->getParameters();
                if (node.call_params) {
                    auto call_params = node.call_params->expressions;
                    checkFunctionParams(call_params, func_params);
                } else {
                    if (!func_params.empty()) {
                        throw std::runtime_error("Semantic: CallExpr param number not match");
                    }
                }
                node.type = func_symbol->getReturnType();
                std::cout << "[TypeChecker] CallExpression to function: " << path_in_expr->segment1->identifier << ", return type: " << node.type << std::endl;
            } else {
                throw std::runtime_error("Semantic: CallExpr function not found");
            }
        }
    } else {
        throw std::runtime_error("Semantic: CallExpr not function");
    }
}

void TypeChecker::visit(MethodCallExpression& node) {
    std::cout << "[TypeChecker] Entering MethodCallExpression node" << std::endl;
    if (node.expression) {
        node.expression->accept(this);
    }
    if (node.call_params) {
        node.call_params->accept(this);
    }
    std::string var_identifier, var_type;
    if (auto path_expr = std::dynamic_pointer_cast<PathExpression>(node.expression)) {
        var_identifier = path_expr->path_in_expression->segment1->identifier;
    }

    var_type = autoDereference(node.expression->type);
    if (var_type == "integer") {
        var_type = "u32";
    }

    if (node.path_ident_segment->identifier == "len") {
        if (var_type[0] == '[') {
            if (node.call_params != nullptr) {
                throw std::runtime_error("Semantic: MethodCallExpr param number not match");
            }
            node.type = "u32";
            return;
        }
    }
    // std::cout << var_type << std::endl;
    auto struct_symbol = current_scope->findStructSymbol(var_type);
    // std::cout << struct_symbol->getIdentifier() << std::endl;
    if (struct_symbol) {
        auto func_symbol = struct_symbol->getMethod(node.path_ident_segment->identifier);
        // std::cout << node.path_ident_segment->identifier << std::endl;
        if (func_symbol) {
            if (!func_symbol->isMethod()) {
                throw std::runtime_error("Semantic: MethodCallExpr function is not a method");
            }
            auto method_type = func_symbol->getMethodType();
            // std::cout << func_symbol->getMethodTypeString() << std::endl;
            if (method_type == MethodType::SELF_MUT_REF || method_type == MethodType::SELF_MUT_VALUE) {
                if (!current_scope->findVariableMutable(var_identifier)) {
                    throw std::runtime_error("Semantic: MethodCallExpr not mutable");
                }
            }
            auto func_params = func_symbol->getParameters();
            if (node.call_params) {
                auto call_params = node.call_params->expressions;
                checkFunctionParams(call_params, func_params);
            } else {
                if (!func_params.empty()) {
                    throw std::runtime_error("Semantic: MethodCallExpr param number not match");
                }
            }
            node.type = func_symbol->getReturnType();
            std::cout << "[TypeChecker] MethodCallExpression to method: " << node.path_ident_segment->identifier << " on type " << var_type << ", return type: " << node.type << std::endl;
        } else {
            throw std::runtime_error("Semantic: MethodCallExpr function not found");
        }
    } else {
        throw std::runtime_error("Semantic: MethodCallExpr struct not found");
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
    auto arr_type = autoDereference(node.base_expression->type);
    if (arr_type[0] != '[') {
        std::cout << arr_type << std::endl;
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
        if (!canAssign(struct_field->getType(), type)) {
            std::cout << struct_field->getType() << ' ' << type << std::endl;
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
    std::cout << "[TypeChecker] Entering BlockExpression node" << std::endl;
    // BlockExpression 会创建新的 scope，需要进入
    auto prev_scope = current_scope;
    current_scope = current_scope->getChild();
    
    if (node.statements) {
        node.statements->accept(this);
    }

    std::cout << "[TypeChecker] checking BlockExpression node" << std::endl;
    // 实现尾表达式检测和类型推断
    if (node.statements && !node.statements->statements.empty()) {
        // 检测尾表达式
        std::shared_ptr<ASTNode> tail_expression = nullptr;
        
        // 从最后一个语句开始向前查找尾表达式
        auto it = node.statements->statements.rbegin();

        // 检查是否为 ExpressionStatement
        if (auto stmt = std::dynamic_pointer_cast<Statement>(*it)) {
            if (auto expr_stmt = std::dynamic_pointer_cast<ExpressionStatement>(stmt->child)) {
                if (!expr_stmt->has_semi) {
                    // 没有分号的 ExpressionStatement 是尾表达式
                    tail_expression = expr_stmt->child;
                }
                if (auto expr_without_block = std::dynamic_pointer_cast<ExpressionWithoutBlock>(expr_stmt->child)) {
                    if (auto return_expr = std::dynamic_pointer_cast<ReturnExpression>(expr_without_block->child)) {
                        node.is_last_stmt_return = true;
                    }
                }
            }
        } else if (auto expr_without_block = std::dynamic_pointer_cast<ExpressionWithoutBlock>(*it)) {
            tail_expression = expr_without_block;
            if (auto return_expr = std::dynamic_pointer_cast<ReturnExpression>(expr_without_block->child)) {
                node.is_last_stmt_return = true;
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
    
    std::cout << "[TypeChecker] BlockExpression type: " << node.type << std::endl;
    current_scope = prev_scope;
    current_scope->nextChild();
}

void TypeChecker::visit(IfExpression& node) {
    std::cout << "[TypeChecker] Entering IfExpression node" << std::endl;
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

    if (!node.else_branch) {
        node.type = "()";
    } else {
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
    std::cout << "[TypeChecker] IfExpression type: " << node.type << std::endl;
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
                    if (!canAssign(return_type, expr_type)) {
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
            if (canAssign(expr->type, base_type)) {
                base_type = expr->type;
            } else if (canAssign(base_type, expr->type)) {
                
            } else {
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
    node.type = node.expression->type;
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
    std::cout << "[TypeChecker] Entering PathInExpr node" << std::endl;
    if (node.segment1) {
        node.segment1->accept(this);
    }
    if (node.segment2) {
        node.segment2->accept(this);
        node.type = node.segment1->type + "::" + node.segment2->type;
    } else {
        node.mutability = node.segment1->mutability;
        node.type = node.segment1->type;
    }
}

void TypeChecker::visit(PathIdentSegment& node) {
    std::cout << "[TypeChecker] Entering PathIdentSegment node" << std::endl;
    // PathIdentSegment 不包含类型信息，无需类型检查
    if (node.path_type == 0) {
        if (current_scope->constSymbolExists(node.identifier)) {
            auto const_symbol = current_scope->findConstSymbol(node.identifier);
            node.type = const_symbol->getType();
        } else if (current_scope->variableExists(node.identifier)) {
            node.mutability = current_scope->findVariableMutable(node.identifier);
            node.type = current_scope->findVariableType(node.identifier);
        } else {
            node.type = node.identifier;
        }
    } else if (node.path_type == 1) {
        if (current_scope->variableExists("self")) {
            node.mutability = current_scope->findVariableMutable(node.identifier);
            node.type = current_scope->findVariableType(node.identifier);
        } else {
            throw std::runtime_error("Semantic: PathIdentSegment unexpected self");
        }
    } else {
        node.type = current_scope->getImplSelfType();
    }
}
