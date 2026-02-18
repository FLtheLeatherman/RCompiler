#include "ir/ir_generator.hpp"

myllvm::Type* IRGenerator::getLLVMType(std::string type_name) {
    if (type_name[0] == '[') {
        std::string num;
        size_t pos = type_name.find_first_of(']') + 1;
        while (pos < type_name.size() && std::isdigit(type_name[pos])) {
            num += type_name[pos];
            pos++;
        }
        uint32_t array_size = std::stoul(num);
        std::string element_type_name = type_name.substr(1, type_name.find_first_of(']') - 1);
        myllvm::Type* element_type = getLLVMType(element_type_name);
        auto array_type = new myllvm::ArrayType(element_type, array_size);
        return array_type;
    } else if (type_name == "i32" || type_name == "u32" || type_name == "isize" || type_name == "usize" || type_name == "integer") {
        return context[type_name]; // 直接从上下文中获取整数类型
    } else if (type_name == "char") {
        return context[type_name];
    } else if (type_name == "bool") {
        return context[type_name];
    } else if (type_name == "()") {
        return context[type_name];
    } else if (type_name[0] == '&') {
        return context["&"];
    } else {
        auto res = new myllvm::StructType(type_name);
        return res;
    }
}
myllvm::Value* IRGenerator::getVarValue(std::string var_name) {
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


IRGenerator::IRGenerator(std::ostream &os, Scope *root_scope, myllvm::IRBuilder *builder)
    : os(os), current_scope(root_scope), root_scope(root_scope), builder(builder) {
    context["i32"] = new myllvm::Int32Type();
    context["u32"] = new myllvm::Int32Type();
    context["isize"] = new myllvm::Int32Type();
    context["usize"] = new myllvm::Int32Type();
    context["integer"] = new myllvm::Int32Type();
    context["char"] = new myllvm::Int8Type();
    context["bool"] = new myllvm::Int1Type();
    context["()"] = new myllvm::VoidType();
    context["&"] = new myllvm::PointerType();
}

void IRGenerator::visit(Crate& node) {
    std::cerr << "Generating IR..." << std::endl;
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
    std::cerr << "Generating IR for function: " << node.identifier << std::endl;
    auto prev_scope = current_scope;
    current_scope = (current_scope->getChild()).get();
    local_variables_stack.push_back({}); // 进入新作用域，创建新的局部变量表

    if (node.function_return_type) {
        node.function_return_type->accept(this);
    }
    // std::cerr << "so far so good???" << std::endl;
    std::string type_str;
    if (node.function_return_type) {
        auto path_ident_segment = std::dynamic_pointer_cast<PathIdentSegment>(node.function_return_type->type->child);
        type_str = path_ident_segment->identifier; // 获取返回类型的字符串表示
    } else {
        type_str = "()"; // 默认返回类型为 unit
    }
    // std::cerr << "Return type string: " << type_str << std::endl;
    myllvm::Type* return_type = getLLVMType(type_str);
    // std::cerr << "LLVM return type: " << return_type->toString() << std::endl;
    // std::cerr << "Is return type void? " << (return_type->isVoid() ? "Yes" : "No") << std::endl;
    bool is_large = return_type->isStruct() || return_type->isArray();
    auto function_val = new myllvm::Function(node.identifier, return_type, is_large);
    functions[node.identifier] = function_val;
    if (node.block_expression) {
        os << "define " << return_type->toString() << " @" << node.identifier << "(";
        // 处理函数参数
        // 有点困难
        if (!is_large) {
            
        } else {

        }
        if (node.function_parameters) {
            node.function_parameters->accept(this);
        }
        os << ") {" << std::endl;
        os << "entry:" << std::endl;
        current_basic_block = "entry";
        current_function_has_return = false;
        node.block_expression->accept(this);
        // 如果 block 最后没有 return，也可能是无分号的语句，需要在这里补上 return
        if (!current_function_has_return) {
            if (return_type->isVoid()) {
                builder->createRet(nullptr); // void 函数直接返回
            } else {
                node.ir_value = node.block_expression->ir_value; // 将 block 的结果作为返回值
                builder->createRet(node.block_expression->ir_value);
            }
        }
    } else {
        os << "declare " << return_type->toString() << " @" << node.identifier << "()" << std::endl;
    }
    os << "}" << std::endl;
    
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
    node.ir_value = node.child->ir_value; // 将子语句的 IR 值传递给父节点
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
    myllvm::Type* llvm_type = getLLVMType(type_name);
    auto var_node = std::dynamic_pointer_cast<IdentifierPattern>(node.pattern_no_top_alt->child);
    auto var_name = var_node->identifier;
    uint32_t var_id = var_counter[var_name]++;
    auto var_real_name = "%" + var_name + "_" + std::to_string(var_id);
    auto local_var = new myllvm::LocalVariable(var_real_name, llvm_type);
    local_variables_stack.back()[var_name] = local_var;
    auto alloca = builder->createAlloca(var_real_name, llvm_type);
    builder->createStore(llvm_type, node.expression->ir_value, alloca);
    node.ir_value = alloca; // 将变量的地址存储在 ir_value 中，以便后续使用
}

void IRGenerator::visit(ExpressionStatement& node) {
    if (node.child) {
        node.child->accept(this);
    }
    node.ir_value = node.child->ir_value; // 将表达式语句的 IR 值传递给父节点
}

void IRGenerator::visit(Statements& node) {
    std::cerr << "Generating IR for block with " << node.statements.size() << " statements" << std::endl;
    for (auto& stmt : node.statements) {
        if (stmt) {
            stmt->accept(this);
        }
    }
    if (!node.statements.empty()) {
        node.ir_value = node.statements.back()->ir_value; // 将 block 中最后一个语句的 IR 值作为 block 的 IR 值
    }
}

void IRGenerator::visit(Expression& node) {
    if (node.child) {
        node.child->accept(this);
    }
    node.ir_value = node.child->ir_value; // 将子表达式的 IR 值传递给父表达式
}

void IRGenerator::visit(ExpressionWithoutBlock& node) {
    if (node.child) {
        node.child->accept(this);
    }
    node.ir_value = node.child->ir_value; // 将子表达式的 IR 值传递给父表达式
    // if (std::dynamic_pointer_cast<IntegerLiteral>(node.child)) {
    //     std::cerr << "ExpressionWithoutBlock contains a IntegerLiteral" << std::endl;
    // } else {
    //     std::cerr << "ExpressionWithoutBlock does not contain a IntegerLiteral" << std::endl;
    // }
    // std::cerr << "ExpressionWithoutBlock IR value is void? " << node.ir_value->getType()->isVoid() << std::endl;
}

void IRGenerator::visit(ExpressionWithBlock& node) {
    if (node.child) {
        node.child->accept(this);
    }
    node.ir_value = node.child->ir_value; // 将子表达式的 IR 值传递给父表达式
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
    auto temp_value = node.value;
    std::string type_string = "";
    while (!isdigit(temp_value.back())) {
        type_string = temp_value.back() + type_string;
        temp_value.pop_back();
    }
    if (type_string.empty()) {
        type_string = "i32"; // 默认类型为 i32
    }
    myllvm::Type* llvm_type = getLLVMType(type_string);
    node.ir_value = new myllvm::Constant(temp_value, llvm_type);
}

void IRGenerator::visit(BoolLiteral& node) {
    myllvm::Type* llvm_type = getLLVMType("bool");
    node.ir_value = new myllvm::Constant(node.value ? "1" : "0", llvm_type);
}

void IRGenerator::visit(PathExpression& node) {
    if (node.path_in_expression) {
        node.path_in_expression->is_ptr = node.is_ptr; // 将路径表达式的 is_ptr 属性传递给子表达式
        node.path_in_expression->accept(this);
    }
    node.ir_value = node.path_in_expression->ir_value; // 将路径表达式的 IR 值传递给父表达式
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
    if (node.binary_type != BinaryExpression::AND_AND && node.binary_type != BinaryExpression::OR_OR) {
        if (node.lhs) {
            node.lhs->accept(this);
        }
        if (node.rhs) {
            node.rhs->accept(this);
        }
    }
    auto lhs = node.lhs->ir_value, rhs = node.rhs->ir_value;
    std::pair<std::string, std::string> labels;
    std::string true_label, false_label;
    std::string previous_basic_block = "";
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
            if (node.lhs->type == "i32" || node.lhs->type == "isize" || node.rhs->type == "i32" || node.rhs->type == "isize") {
                node.ir_value = builder->createIcmp("sgt", lhs, rhs); // 有符号比较
            } else {
                node.ir_value = builder->createIcmp("ugt", lhs, rhs); // 无符号比较
            }
            break;
        case BinaryExpression::LT:
            if (node.lhs->type == "i32" || node.lhs->type == "isize" || node.rhs->type == "i32" || node.rhs->type == "isize") {
                node.ir_value = builder->createIcmp("slt", lhs, rhs); // 有符号比较
            } else {
                node.ir_value = builder->createIcmp("ult", lhs, rhs); // 无符号比较
            }
            break;
        case BinaryExpression::GE:
            if (node.lhs->type == "i32" || node.lhs->type == "isize" || node.rhs->type == "i32" || node.rhs->type == "isize") {
                node.ir_value = builder->createIcmp("sge", lhs, rhs); // 有符号比较
            } else {
                node.ir_value = builder->createIcmp("uge", lhs, rhs); // 无符号比较
            }
            break;
        case BinaryExpression::LE:
            if (node.lhs->type == "i32" || node.lhs->type == "isize" || node.rhs->type == "i32" || node.rhs->type == "isize") {
                node.ir_value = builder->createIcmp("sle", lhs, rhs); // 有符号比较
            } else {
                node.ir_value = builder->createIcmp("ule", lhs, rhs); // 无符号比较
            }
            break;
        case BinaryExpression::AND_AND:
            node.lhs->accept(this);
            previous_basic_block = current_basic_block;
            lhs = node.lhs->ir_value;
            labels = builder->createBr(lhs);
            true_label = labels.first, false_label = labels.second;
            current_basic_block = true_label;
            node.rhs->accept(this);
            rhs = node.rhs->ir_value;
            builder->createUncondBr(false_label);
            builder->createLabel(false_label);
            node.ir_value = builder->createPHI(context["bool"], {{lhs, previous_basic_block}, {rhs, current_basic_block}});
            current_basic_block = false_label;
            break;
        case BinaryExpression::OR_OR:
            node.lhs->accept(this);
            previous_basic_block = current_basic_block;
            lhs = node.lhs->ir_value;
            auto lhs_is_false = builder->createIcmp("eq", lhs, new myllvm::Constant("0", context["bool"]));
            labels = builder->createBr(lhs_is_false);
            true_label = labels.first, false_label = labels.second;
            current_basic_block = true_label;
            node.rhs->accept(this);
            rhs = node.rhs->ir_value;
            builder->createUncondBr(false_label);
            builder->createLabel(false_label);
            node.ir_value = builder->createPHI(context["bool"], {{lhs, previous_basic_block}, {rhs, current_basic_block}});
            current_basic_block = false_label;
            break;
    }
}

void IRGenerator::visit(AssignmentExpression& node) {
    if (node.lhs) {
        // std::cerr << "!!" << std::endl;
        node.lhs->is_ptr = true;
        node.lhs->accept(this);
    }
    if (node.rhs) {
        node.rhs->accept(this);
    }

    auto lhs = node.lhs->ir_value;
    auto rhs = node.rhs->ir_value;
    builder->createStore(lhs->getType(), rhs, lhs);
    node.ir_value = new myllvm::Instruction("()", context["void"]);
}

void IRGenerator::visit(CompoundAssignmentExpression& node) {
    if (node.lhs) {
        // std::cerr << "??" << std::endl;
        node.lhs->is_ptr = true;
        node.lhs->accept(this);
    }
    if (node.rhs) {
        node.rhs->accept(this);
    }
    auto lhs = node.lhs->ir_value;
    auto rhs = node.rhs->ir_value;
    auto lhs_value = builder->createLoad(lhs->getType(), lhs);
    myllvm::Value* result;
    switch (node.type) {
        case CompoundAssignmentExpression::PLUS_EQ:
            result = builder->createBinaryOp("add", lhs_value, rhs);
            break;
        case CompoundAssignmentExpression::MINUS_EQ:
            result = builder->createBinaryOp("sub", lhs_value, rhs);
            break;
        case CompoundAssignmentExpression::STAR_EQ:
            result = builder->createBinaryOp("mul", lhs_value, rhs);
            break;
        case CompoundAssignmentExpression::SLASH_EQ:
            result = builder->createBinaryOp("div", lhs_value, rhs);
            break;
        case CompoundAssignmentExpression::PERCENT_EQ:
            result = builder->createBinaryOp("rem", lhs_value, rhs);
            break;
        case CompoundAssignmentExpression::CARET_EQ:
            result = builder->createBinaryOp("xor", lhs_value, rhs);
            break;
        case CompoundAssignmentExpression::AND_EQ:
            result = builder->createBinaryOp("and", lhs_value, rhs);
            break;
        case CompoundAssignmentExpression::OR_EQ:
            result = builder->createBinaryOp("or", lhs_value, rhs);
            break;
        case CompoundAssignmentExpression::SHL_EQ:
            result = builder->createBinaryOp("shl", lhs_value, rhs);
            break;
        case CompoundAssignmentExpression::SHR_EQ:
            result = builder->createBinaryOp("shr", lhs_value, rhs);
            break;
    }
    builder->createStore(lhs->getType(), result, lhs);
    node.ir_value = new myllvm::Instruction("()", context["void"]);
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

    node.ir_value = node.statements->ir_value; // 将 block 中最后一个语句的 IR 值作为 block 的 IR 值
    
    current_scope = prev_scope;
    current_scope->nextChild();
}

void IRGenerator::visit(IfExpression& node) {
    if (node.condition) {
        node.condition->accept(this);
    }
    std::cerr << "Generating IR for if expression" << std::endl;
    auto [true_label, false_label] = builder->createBr(node.condition->ir_value);
    std::cerr << "True label: " << true_label << ", False label: " << false_label << std::endl;
    auto end_label = builder->getLabel("if_end");
    current_basic_block = true_label;
    if (node.then_block) {
        node.then_block->accept(this);
    }
    if (!label_has_br_or_ret[current_basic_block]) builder->createUncondBr(end_label);
    builder->createLabel(false_label);
    current_basic_block = false_label;
    if (node.else_branch) {
        node.else_branch->accept(this);
    }
    if (!label_has_br_or_ret[current_basic_block]) builder->createUncondBr(end_label);
    builder->createLabel(end_label);
    current_basic_block = end_label;
    if (node.type != "()") {
        if (node.else_branch) {
            node.ir_value = builder->createPHI(getLLVMType(node.type), {{node.then_block->ir_value, true_label}, {node.else_branch->ir_value, false_label}});
        } else {
            node.ir_value = builder->createPHI(getLLVMType(node.type), {{node.then_block->ir_value, true_label}, {nullptr, false_label}});
        }
    }
}

void IRGenerator::visit(LoopExpression& node) {
    if (node.child) {
        node.child->accept(this);
        node.ir_value = node.child->ir_value; // 将循环表达式的 IR 值传递给父节点
    }
}

void IRGenerator::visit(InfiniteLoopExpression& node) {
    auto prev_scope = current_scope;
    current_scope = (current_scope->getChild()).get();
    local_variables_stack.push_back({}); // 进入新作用域，创建新的局部变量表

    auto loop_start_label = builder->getLabel("loop_start");
    auto loop_end_label = builder->getLabel("loop_end");
    continue_labels_stack.push_back(loop_start_label);
    break_labels_stack.push_back(loop_end_label);
    phi_nodes_stack.push_back({}); // 为新的循环创建一个空的 PHI 节点列表
    builder->createUncondBr(loop_start_label);
    builder->createLabel(loop_start_label);
    current_basic_block = loop_start_label;

    if (node.block_expression) {
        node.block_expression->accept(this);
    }

    if (!label_has_br_or_ret[current_basic_block]) builder->createUncondBr(loop_start_label);
    builder->createLabel(loop_end_label);
    current_basic_block = loop_end_label;
    if (!phi_nodes_stack.back().empty()) {
        node.ir_value = builder->createPHI(getLLVMType(node.type), phi_nodes_stack.back()); // 循环的返回值通过 PHI 节点来处理，初始时没有任何 incoming value
    } else {
        node.ir_value = nullptr; // 如果循环内没有任何 break 语句，循环的 IR 值为 nullptr
    }
    continue_labels_stack.pop_back();
    break_labels_stack.pop_back();
    phi_nodes_stack.pop_back(); // 循环结束，弹出 PHI 节点列表

    current_scope = prev_scope;
    current_scope->nextChild();
}

void IRGenerator::visit(PredicateLoopExpression& node) {
    auto prev_scope = current_scope;
    current_scope = (current_scope->getChild()).get();
    local_variables_stack.push_back({}); // 进入新作用域，创建新的局部变量表

    auto loop_cond_label = builder->getLabel("loop_cond");
    builder->createUncondBr(loop_cond_label);
    builder->createLabel(loop_cond_label);
    current_basic_block = loop_cond_label;

    if (node.condition) {
        node.condition->accept(this);
    }

    auto [loop_start_label, loop_end_label] = builder->createBr(node.condition->ir_value, "loop_start", "loop_end");
    current_basic_block = loop_start_label;
    continue_labels_stack.push_back(loop_cond_label);
    break_labels_stack.push_back(loop_end_label);
    phi_nodes_stack.push_back({}); // 为新的循环创建一个空的 PHI 节点列表
    
    if (node.block_expression) {
        node.block_expression->accept(this);
    }

    if (!label_has_br_or_ret[current_basic_block]) builder->createUncondBr(loop_cond_label);
    builder->createLabel(loop_end_label);
    current_basic_block = loop_end_label;
    if (!phi_nodes_stack.back().empty()) {
        node.ir_value = builder->createPHI(getLLVMType(node.type), phi_nodes_stack.back()); // 循环的返回值通过 PHI 节点来处理，初始时没有任何 incoming value
    } else {
        node.ir_value = nullptr; // 如果循环内没有任何 break 语句，循环的 IR 值为 nullptr
    }
    continue_labels_stack.pop_back();
    break_labels_stack.pop_back();
    phi_nodes_stack.pop_back(); // 循环结束，弹出 PHI 节点列表

    current_scope = prev_scope;
    current_scope->nextChild();
}

void IRGenerator::visit(BreakExpression& node) {
    if (node.expression) {
        node.expression->accept(this);
        phi_nodes_stack.back().push_back({node.expression->ir_value, current_basic_block}); // 将 break 的返回值和当前基本块添加到 PHI 节点列表中 
    }
    builder->createUncondBr(break_labels_stack.back());
    label_has_br_or_ret[current_basic_block] = true; // 标记当前基本块已经有分支指令，后续不再添加无条件分支
}

void IRGenerator::visit(ContinueExpression& node) {
    builder->createUncondBr(continue_labels_stack.back());
    label_has_br_or_ret[current_basic_block] = true; // 标记当前基本块已经有分支指令，后续不再添加无条件分支
}

void IRGenerator::visit(ReturnExpression& node) {
    if (node.expression) {
        node.expression->accept(this);
        builder->createRet(node.expression->ir_value);
    } else {
        auto void_val = new myllvm::Constant("", getLLVMType("()"));
        builder->createRet(void_val);
    }
}

void IRGenerator::visit(Condition& node) {
    if (node.expression) {
        node.expression->accept(this);
    }
    node.ir_value = node.expression->ir_value; // 将条件表达式的 IR 值传递给父节点
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
    // if (node.segment1) {
    //     node.segment1->accept(this);
    // }
    // if (node.segment2) {
    //     node.segment2->accept(this);
    // }
    auto str = node.segment1->identifier + (node.segment2 ? (".." + node.segment2->identifier) : "");
    if (node.is_ptr) {
        std::cerr << "is_ptr is true for PathInExpression with path: " << str << std::endl;
        auto var_value = getVarValue(str);
        if (var_value != nullptr) {
            node.ir_value = var_value; // 直接使用变量的地址
        }
    } else {
        auto var_value = getVarValue(str);
        if (var_value != nullptr) {
            auto load = builder->createLoad(getLLVMType(node.type), var_value);
            node.ir_value = load; // 加载变量的值
        }
    }
}

void IRGenerator::visit(PathIdentSegment& node) {
    if (node.path_type == 0) {
        auto var_value = getVarValue(node.identifier);
        if (var_value != nullptr) {
            auto load = builder->createLoad(getLLVMType(node.type), getVarValue(node.identifier));
            node.ir_value = load;
        }
    }
}
