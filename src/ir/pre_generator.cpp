#include "ir/pre_generator.hpp"

myllvm::Type* PreGenerator::getLLVMType(std::string type_name) {
    if (type_name == "Self") {
        if (current_impl != "") {
            auto res = context["struct." + current_impl];
            return res;
        } else {
            std::cerr << "Error: 'Self' type used outside of an impl block" << std::endl;
            exit(1);
        }
    } else if (type_name[0] == '[') {
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
        auto pointee_type = getLLVMType(type_name.substr(1));
        auto res = new myllvm::PointerType(pointee_type);
        return res;
    } else {
        auto res = context["struct." + type_name];
        return res;
    }
}

PreGenerator::PreGenerator(myllvm::IRBuilder *ir_builder, Scope *root_scope)
    : ir_builder(ir_builder), current_scope(root_scope), root_scope(root_scope) {
    context["i32"] = new myllvm::Int32Type();
    context["u32"] = new myllvm::Int32Type();
    context["isize"] = new myllvm::Int32Type();
    context["usize"] = new myllvm::Int32Type();
    context["integer"] = new myllvm::Int32Type();
    context["char"] = new myllvm::Int8Type();
    context["bool"] = new myllvm::Int1Type();
    context["()"] = new myllvm::VoidType();
    context["&"] = new myllvm::PointerType(nullptr);

    auto exit_val = new myllvm::Function("exit", context["()"], false, false);
    exit_val->newParamIsRef(false); // exit 的参数不是引用类型
    functions["exit"] = exit_val;

    auto printlnInt_val = new myllvm::Function("printlnInt", context["()"], false, false);
    printlnInt_val->newParamIsRef(false); // printlnInt 的参数不是引用类型
    functions["printlnInt"] = printlnInt_val;

    auto getInt_val = new myllvm::Function("getInt", context["i32"], false, false);
    functions["getInt"] = getInt_val;
}

const std::unordered_map<std::string, myllvm::Function*>& PreGenerator::getFunctions() {
    return this->functions;
}
const std::unordered_map<std::string, myllvm::Type*>& PreGenerator::getContext() {
    return this->context;
}

void PreGenerator::visit(Crate& node) {
    // std::cerr << "Generating IR..." << std::endl;
    for (auto item: node.items) {
        item->accept(this);
    }
}

void PreGenerator::visit(Item& node) {
    if (node.item) {
        node.item->accept(this);
    }
}

void PreGenerator::visit(Function& node) {
    // std::cerr << "Function?" << std::endl;
    auto prev_scope = current_scope;
    current_scope = (current_scope->getChild()).get();

    if (node.function_return_type) {
        node.function_return_type->accept(this);
    }
    std::string type_str;
    if (node.function_return_type) {
        if (auto path_ident_segment = std::dynamic_pointer_cast<PathIdentSegment>(node.function_return_type->type->child)) {
            type_str = path_ident_segment->identifier; // 获取返回类型的字符串表示
        } else {
            type_str = "()";
        }
    } else {
        type_str = "()"; // 默认返回类型为 unit
    }
    // std::cerr << type_str << std::endl;
    myllvm::Type* return_type = getLLVMType(type_str);
    // std::cerr << return_type->toString() << std::endl;
    auto function_name = node.identifier;
    if (current_impl != "") {
        function_name = current_impl + ".." + node.identifier;
    }
    bool has_self = node.function_parameters && node.function_parameters->self_param;
    bool self_ref = false;
    if (has_self) {
        auto shorthand_self = std::dynamic_pointer_cast<ShorthandSelf>(node.function_parameters->self_param->child);
        self_ref = shorthand_self->is_reference;
    }
    auto function_val = new myllvm::Function(function_name, return_type, has_self, self_ref);

    func_param.push_back({});
    if (node.function_parameters) {
        node.function_parameters->accept(this);
    }
    auto func_param_list = func_param.back();
    func_param.pop_back();
    for (size_t i = 0; i < func_param_list.size(); ++i) {
        if (func_param_list[i]->getType()->isPointer()) {
            function_val->newParamIsRef(true);
        } else {
            function_val->newParamIsRef(false);
        }
    }
    functions[function_name] = function_val;
    if (node.block_expression) {
        node.block_expression->accept(this);
    }
    
    current_scope = prev_scope;
    current_scope->nextChild();
    // std::cerr << "Function!" << std::endl;
}

void PreGenerator::visit(Struct& node) {
    if (node.struct_struct) {
        node.struct_struct->accept(this);
    }
}

void PreGenerator::visit(Enumeration& node) {
    if (node.enum_variants) {
        node.enum_variants->accept(this);
    }
}

void PreGenerator::visit(ConstantItem& node) {
    if (node.expression) {
        node.expression->accept(this);
    }
}

void PreGenerator::visit(Trait& node) {
    auto prev_scope = current_scope;
    current_scope = (current_scope->getChild()).get();
    
    // for (auto& item : node.associated_item) {
    //     if (item) {
    //         item->accept(this);
    //     }
    // }

    current_scope = prev_scope;
    current_scope->nextChild();
}

void PreGenerator::visit(Implementation& node) {
    if (node.impl) {
        node.impl->accept(this);
    }
}

void PreGenerator::visit(InherentImpl& node) {
    auto prev_scope = current_scope;
    current_scope = (current_scope->getChild()).get();

    current_impl = current_scope->getImplSelfType();
    
    // 处理关联项
    for (auto& item : node.associated_item) {
        if (item) {
            item->accept(this);
        }
    }

    current_impl = "";
    
    current_scope = prev_scope;
    current_scope->nextChild();
}

void PreGenerator::visit(TraitImpl& node) {
    auto prev_scope = current_scope;
    current_scope = (current_scope->getChild()).get();

    current_impl = current_scope->getImplSelfType();
    
    // 处理关联项
    for (auto& item : node.associated_item) {
        if (item) {
            item->accept(this);
        }
    }
    
    current_impl = "";

    current_scope = prev_scope;
    current_scope->nextChild();
}

void PreGenerator::visit(AssociatedItem& node) {
    if (node.child) {
        node.child->accept(this);
    }
}

void PreGenerator::visit(FunctionParameters& node) {
    if (node.self_param) {
        node.self_param->accept(this);
        auto shorthand_self = std::dynamic_pointer_cast<ShorthandSelf>(node.self_param->child);
        if (shorthand_self->is_reference) {
            auto self_type = new myllvm::PointerType(getLLVMType("Self"));
            func_param.back().push_back(new myllvm::LocalVariable("%self", self_type));
        } else {
            func_param.back().push_back(new myllvm::LocalVariable("%self", getLLVMType("Self")));
        }
    }
    for (auto& param : node.function_param) {
        if (param) {
            param->accept(this);
        }
    }
}

void PreGenerator::visit(SelfParam& node) {
    if (node.child) {
        node.child->accept(this);
    }
}

void PreGenerator::visit(ShorthandSelf& node) {
    
}

void PreGenerator::visit(TypedSelf& node) {
    if (node.type) {
        node.type->accept(this);
    }
}

void PreGenerator::visit(FunctionParam& node) {
    if (node.type) {
        node.type->accept(this);
    }
    if (node.pattern_no_top_alt) {
        node.pattern_no_top_alt->accept(this);
    }
    auto llvm_type = getLLVMType(node.type->type);
    auto identifier = "%" + std::dynamic_pointer_cast<IdentifierPattern>(node.pattern_no_top_alt->child)->identifier;
    auto param_value = new myllvm::LocalVariable(identifier, llvm_type);
    func_param.back().push_back(param_value);
}

void PreGenerator::visit(FunctionReturnType& node) {
    if (node.type) {
        node.type->accept(this);
    }
}

void PreGenerator::visit(StructStruct& node) {
    struct_fields.clear();
    if (node.struct_fields) {
        node.struct_fields->accept(this);
    }
    ir_builder->createTypeDef(node.identifier, struct_fields);
    context["struct." + node.identifier] = new myllvm::StructType("%struct." + node.identifier, struct_fields); // 将结构体类型添加到上下文中，以便后续使用
}

void PreGenerator::visit(StructFields& node) {
    for (auto& field : node.struct_fields) {
        if (field) {
            field->accept(this);
            struct_fields.push_back(std::make_pair(field->identifier, getLLVMType(field->type->type))); // 将字段类型添加到当前结构体的字段类型列表中
        }
    }
}

void PreGenerator::visit(StructField& node) {
    if (node.type) {
        node.type->accept(this);
    }
}

void PreGenerator::visit(EnumVariants& node) {
    for (auto& variant : node.enum_variant) {
        if (variant) {
            variant->accept(this);
        }
    }
}

void PreGenerator::visit(EnumVariant& node) {

}

void PreGenerator::visit(Statement& node) {
    if (node.child) {
        node.child->accept(this);
    }
}

void PreGenerator::visit(LetStatement& node) {
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

void PreGenerator::visit(ExpressionStatement& node) {
    if (node.child) {
        node.child->accept(this);
    }
}

void PreGenerator::visit(Statements& node) {
    for (auto& stmt : node.statements) {
        if (stmt) {
            stmt->accept(this);
        }
    }
}

void PreGenerator::visit(Expression& node) {
    if (node.child) {
        node.child->accept(this);
    }
}

void PreGenerator::visit(ExpressionWithoutBlock& node) {
    if (node.child) {
        node.child->accept(this);
    }
}

void PreGenerator::visit(ExpressionWithBlock& node) {
    if (node.child) {
        node.child->accept(this);
    }
}

void PreGenerator::visit(CharLiteral& node) {

}

void PreGenerator::visit(StringLiteral& node) {

}

void PreGenerator::visit(RawStringLiteral& node) {

}

void PreGenerator::visit(CStringLiteral& node) {

}

void PreGenerator::visit(RawCStringLiteral& node) {

}

void PreGenerator::visit(IntegerLiteral& node) {
    
}

void PreGenerator::visit(BoolLiteral& node) {
    
}

void PreGenerator::visit(PathExpression& node) {
    if (node.path_in_expression) {
        node.path_in_expression->accept(this);
    }
}

void PreGenerator::visit(FieldExpression& node) {
    if (node.expression) {
        node.expression->accept(this);
    }
}

void PreGenerator::visit(UnaryExpression& node) {
    if (node.expression) {
        node.expression->accept(this);
    }
}

void PreGenerator::visit(BorrowExpression& node) {
    if (node.expression) {
        node.expression->accept(this);
    }
}

void PreGenerator::visit(DereferenceExpression& node) {
    if (node.expression) {
        node.expression->accept(this);    
    }
}

void PreGenerator::visit(BinaryExpression& node) {
    if (node.lhs) {
        node.lhs->accept(this);
    }
    if (node.rhs) {
        node.rhs->accept(this);
    }
}

void PreGenerator::visit(AssignmentExpression& node) {
    if (node.lhs) {
        node.lhs->accept(this);
    }
    if (node.rhs) {
        node.rhs->accept(this);
    }
}

void PreGenerator::visit(CompoundAssignmentExpression& node) {
    if (node.lhs) {
        node.lhs->accept(this);
    }
    if (node.rhs) {
        node.rhs->accept(this);
    }
}

void PreGenerator::visit(TypeCastExpression& node) {
    if (node.expression) {
        node.expression->accept(this);
    }
    if (node.type) {
        node.type->accept(this);
    }
}

void PreGenerator::visit(CallExpression& node) {
    if (node.expression) {
        node.expression->accept(this);
    }
}

void PreGenerator::visit(MethodCallExpression& node) {
    if (node.expression) {
        node.expression->accept(this);
    }
}

void PreGenerator::visit(IndexExpression& node) {
    if (node.base_expression) {
        node.base_expression->accept(this);
    }
    if (node.index_expression) {
        node.index_expression->accept(this);
    }
}

void PreGenerator::visit(StructExpression& node) {
    if (node.path_in_expression) {
        node.path_in_expression->accept(this);
    }
    if (node.struct_expr_fields) {
        node.struct_expr_fields->accept(this);
    }
}

void PreGenerator::visit(ArrayExpression& node) {
    if (node.array_elements) {
        node.array_elements->accept(this);
    }
}

void PreGenerator::visit(GroupedExpression& node) {
    if (node.expression) {
        node.expression->accept(this);
    }
}

void PreGenerator::visit(BlockExpression& node) {
    auto prev_scope = current_scope;
    current_scope = (current_scope->getChild()).get();
    
    if (node.statements) {
        node.statements->accept(this);
    }
    
    current_scope = prev_scope;
    current_scope->nextChild();
}

void PreGenerator::visit(IfExpression& node) {
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

void PreGenerator::visit(LoopExpression& node) {
    if (node.child) {
        node.child->accept(this);
    }
}

void PreGenerator::visit(InfiniteLoopExpression& node) {
    auto prev_scope = current_scope;
    current_scope = (current_scope->getChild()).get();

    if (node.block_expression) {
        node.block_expression->accept(this);
    }

    current_scope = prev_scope;
    current_scope->nextChild();
}

void PreGenerator::visit(PredicateLoopExpression& node) {
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

void PreGenerator::visit(BreakExpression& node) {
    if (node.expression) {
        node.expression->accept(this);
    }
}

void PreGenerator::visit(ContinueExpression& node) {

}

void PreGenerator::visit(ReturnExpression& node) {
    if (node.expression) {
        node.expression->accept(this);
    }
}

void PreGenerator::visit(Condition& node) {
    if (node.expression) {
        node.expression->accept(this);
    }
}

void PreGenerator::visit(ArrayElements& node) {
    for (auto& expr : node.expressions) {
        if (expr) {
            expr->accept(this);
        }
    }
}

void PreGenerator::visit(StructExprFields& node) {
    for (auto& field : node.struct_expr_fields) {
        if (field) {
            field->accept(this);
        }
    }
}

void PreGenerator::visit(StructExprField& node) {
    if (node.expression) {
        node.expression->accept(this);
    }
}

void PreGenerator::visit(CallParams& node) {
    
}

void PreGenerator::visit(PatternNoTopAlt& node) {
    if (node.child) {
        node.child->accept(this);
    }
}

void PreGenerator::visit(IdentifierPattern& node) {

}

void PreGenerator::visit(ReferencePattern& node) {
    if (node.pattern) {
        node.pattern->accept(this);
    }
}

void PreGenerator::visit(Type& node) {
    if (node.child) {
        node.child->accept(this);
    }
}

void PreGenerator::visit(ReferenceType& node) {
    if (node.type) {
        node.type->accept(this);
    }
}

void PreGenerator::visit(ArrayType& node) {
    if (node.type) {
        node.type->accept(this);
    }
    if (node.expression) {
        node.expression->accept(this);
    }
}

void PreGenerator::visit(UnitType& node) {

}

void PreGenerator::visit(PathInExpression& node) {
    if (node.segment1) {
        node.segment1->accept(this);
    }
    if (node.segment2) {
        node.segment2->accept(this);
    }
}

void PreGenerator::visit(PathIdentSegment& node) {
    
}
