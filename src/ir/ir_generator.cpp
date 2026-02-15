#include "ir/ir_generator.hpp"

llvm::Type* IRGenerator::getLLVMType(std::string type_name) {
    if (type_name[0] == '[') {
        std::string num;
        size_t pos = type_name.find_first_of(']') + 1;
        while (pos < type_name.size() && std::isdigit(type_name[pos])) {
            num += type_name[pos];
            pos++;
        }
        uint32_t array_size = std::stoul(num);
        std::string element_type_name = type_name.substr(1, type_name.find_first_of(']') - 1);
        llvm::Type* element_type = getLLVMType(element_type_name);
        auto array_type = new llvm::ArrayType(element_type, array_size);
        return array_type;
    } else if (type_name == "i32" || type_name == "u32" || type_name == "isize" || type_name == "usize") {
        auto res = new llvm::Int32Type();
        return res;
    } else if (type_name == "char") {
        auto res = new llvm::Int8Type();
        return res;
    } else if (type_name == "bool") {
        auto res = new llvm::Int1Type();
        return res;
    } else if (type_name == "()") {
        auto res = new llvm::VoidType();
        return res;
    } else if (type_name[0] == '&') {
        auto res = new llvm::PointerType();
        return res;
    } else {
        auto res = new llvm::StructType(type_name);
        return res;
    }
}
llvm::Value* IRGenerator::getVarValue(std::string var_name) {
    for (auto it = local_variables_stack.rbegin(); it != local_variables_stack.rend(); ++it) {
        auto& local_vars = *it;
        if (local_vars.find(var_name) != local_vars.end()) {
            return local_vars[var_name];
        }
    }
    if (global_variables.find(var_name) != global_variables.end()) {
        return global_variables[var_name];
    }
    return nullptr; // 变量未找到
}


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
    local_variables_stack.push_back({}); // 进入新作用域，创建新的局部变量表
    
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
    local_variables_stack.push_back({}); // 进入新作用域，创建新的局部变量表
    
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
    local_variables_stack.push_back({}); // 进入新作用域，创建新的局部变量表
    
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
    local_variables_stack.push_back({}); // 进入新作用域，创建新的局部变量表
    
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
    auto type_name = node.type->type;
    llvm::Type* llvm_type = getLLVMType(type_name);
    auto var_node = std::dynamic_pointer_cast<IdentifierPattern>(node.pattern_no_top_alt->child);
    auto var_name = var_node->identifier;
    uint32_t var_id = var_counter[var_name]++;
    auto var_real_name = "%" + var_name + "_" + std::to_string(var_id);
    auto local_var = new llvm::LocalVariable(var_real_name, llvm_type);
    local_variables_stack.back()[var_name] = local_var;
    auto alloca = builder->createAlloca(var_real_name, llvm_type);
    builder->createStore(llvm_type, node.expression->ir_value, alloca);
    node.ir_value = alloca; // 将变量的地址存储在 ir_value 中，以便后续使用
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
    auto lhs = node.lhs->ir_value, rhs = node.rhs->ir_value;
    switch (node.binary_type) {
        case BinaryExpression::PLUS:
            node.ir_value = builder->createBinaryOp("add", lhs, rhs);
            break;
        case BinaryExpression::MINUS:
            node.ir_value = builder->createBinaryOp("sub", lhs, rhs);
            break;
        case BinaryExpression::STAR:
            node.ir_value = builder->createBinaryOp("mul", lhs, rhs);
            break;
        case BinaryExpression::SLASH:
            node.ir_value = builder->createBinaryOp("div", lhs, rhs);
            break;
        case BinaryExpression::PERCENT:
            node.ir_value = builder->createBinaryOp("rem", lhs, rhs);
            break;
        case BinaryExpression::CARET:
            node.ir_value = builder->createBinaryOp("xor", lhs, rhs);
            break;
        case BinaryExpression::AND:
            node.ir_value = builder->createBinaryOp("and", lhs, rhs);
            break;
        case BinaryExpression::OR:
            node.ir_value = builder->createBinaryOp("or", lhs, rhs);
            break;
        case BinaryExpression::SHL:
            node.ir_value = builder->createBinaryOp("shl", lhs, rhs);
            break;
        case BinaryExpression::SHR:
            node.ir_value = builder->createBinaryOp("shr", lhs, rhs);
            break;
        case BinaryExpression::EQ_EQ:
            node.ir_value = builder->createIcmp("eq", lhs, rhs);
            break;
        case BinaryExpression::NE:
            node.ir_value = builder->createIcmp("ne", lhs, rhs);
            break;
        case BinaryExpression::GT:
            if (node.lhs->type == "i32" || node.lhs->type == "isize") {
                node.ir_value = builder->createIcmp("sgt", lhs, rhs); // 有符号比较
            } else {
                node.ir_value = builder->createIcmp("ugt", lhs, rhs); // 无符号比较
            }
            break;
        case BinaryExpression::LT:
            if (node.lhs->type == "i32" || node.lhs->type == "isize") {
                node.ir_value = builder->createIcmp("slt", lhs, rhs); // 有符号比较
            } else {
                node.ir_value = builder->createIcmp("ult", lhs, rhs); // 无符号比较
            }
            node.ir_value = builder->createIcmp("lt", lhs, rhs);
            break;
        case BinaryExpression::GE:
            if (node.lhs->type == "i32" || node.lhs->type == "isize") {
                node.ir_value = builder->createIcmp("sge", lhs, rhs); // 有符号比较
            } else {
                node.ir_value = builder->createIcmp("uge", lhs, rhs); // 无符号比较
            }
            node.ir_value = builder->createIcmp("ge", lhs, rhs);
            break;
        case BinaryExpression::LE:
            if (node.lhs->type == "i32" || node.lhs->type == "isize") {
                node.ir_value = builder->createIcmp("sle", lhs, rhs); // 有符号比较
            } else {
                node.ir_value = builder->createIcmp("ule", lhs, rhs); // 无符号比较
            }
            break;
        case BinaryExpression::AND_AND:
            node.ir_value = builder->createBinaryOp("and", lhs, rhs); // 逻辑与可以用位与实现
            break;
        case BinaryExpression::OR_OR:
            node.ir_value = builder->createBinaryOp("or", lhs, rhs); // 逻辑或可以用位或实现
            break;
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
    local_variables_stack.push_back({}); // 进入新作用域，创建新的局部变量表
    
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
    local_variables_stack.push_back({}); // 进入新作用域，创建新的局部变量表

    if (node.block_expression) {
        node.block_expression->accept(this);
    }

    current_scope = prev_scope;
    current_scope->nextChild();
}

void IRGenerator::visit(PredicateLoopExpression& node) {
    auto prev_scope = current_scope;
    current_scope = (current_scope->getChild()).get();
    local_variables_stack.push_back({}); // 进入新作用域，创建新的局部变量表

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
    node.ir_value = node.segment1->ir_value;
    // 先不管 segment2，后续如果需要处理方法调用等情况再来完善
}

void IRGenerator::visit(PathIdentSegment& node) {
    if (node.path_type == 0) {
        node.ir_value = getVarValue(node.identifier);
    }
}
