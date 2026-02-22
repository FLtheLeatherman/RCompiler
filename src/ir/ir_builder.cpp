#include "ir/ir_builder.hpp"

namespace myllvm {

IRBuilder::IRBuilder(std::ostream &output_stream) : os(output_stream) {
    temp_count = 0;
}
std::string IRBuilder::newTempReg() {
    return "%" + std::to_string(temp_count++);
}
std::string IRBuilder::getLabel(std::string label) {
    if (label_count.find(label) == label_count.end()) {
        label_count[label] = 0;
    }
    return label + std::to_string(label_count[label]++);
}

Instruction* IRBuilder::createBinaryOp(std::string op, Value* lhs, Value* rhs) {
    Instruction* result = new Instruction(newTempReg(), lhs->getType()); // 假设结果类型与操作数相同
    os << "  " << result->toString() << " = " << op << " " << lhs->getType()->toString() << " " << lhs->toString() << ", " << rhs->toString() << std::endl;
    return result;
}
Instruction* IRBuilder::createUnaryOp(std::string op, Value* operand) {
    Instruction* result = new Instruction(newTempReg(), operand->getType()); // 假设结果类型与操作数相同
    if (op == "-") {
        os << "  " << result->toString() << " = " << op << " " << operand->getType()->toString() << " 0, " << operand->toString() << std::endl;
    } else {
        throw std::runtime_error("Unsupported unary operator: " + op);
    }
    return result;
}
LocalVariable* IRBuilder::createAlloca(std::string var_name, Type* type) {
    LocalVariable* result = new LocalVariable(var_name, type);
    os << "  " << result->toString() << " = alloca " << type->toString() << std::endl;
    return result;
}
Instruction* IRBuilder::createLoad(Type* type, Value* ptr) {
    Instruction* result = new Instruction(newTempReg(), type);
    os << "  " << result->toString() << " = load " << type->toString() << ", ptr " << ptr->toString() << std::endl;
    return result;
}
void IRBuilder::createStore(Type* type, Value* value, Value* ptr) {
    os << "  store " << type->toString() << " " << value->toString() << ", ptr " << ptr->toString() << std::endl;
    return; // store指令没有返回值
}
void IRBuilder::createLabel(std::string label) {
    os << label << ":" << std::endl;
    return; // 标签定义没有返回值
}
Instruction* IRBuilder::createIcmp(std::string cmp, Value* lhs, Value* rhs) {
    Instruction* result = new Instruction(newTempReg(), new Int1Type()); // icmp的结果类型是i1
    os << "  " << result->toString() << " = icmp " << cmp << " " << lhs->getType()->toString() << " " << lhs->toString() << ", " << rhs->toString() << std::endl;
    return result;
}
void IRBuilder::createRet(Value* value) {
    if (value == nullptr) {
        os << "  ret void" << std::endl;
    } else {
        os << "  ret " << value->getType()->toString() << " " << value->toString() << std::endl;
    }
    return; // ret指令没有返回值
}
std::pair<std::string, std::string> IRBuilder::createBr(Value* condition, std::string true_label_prefix, std::string false_label_prefix) {
    std::string true_label = getLabel(true_label_prefix);
    std::string false_label = getLabel(false_label_prefix);
    // std::cerr << "Creating conditional branch with condition: " << condition->toString() << std::endl;
    os << "  br i1 " << condition->toString() << ", label %" << true_label << ", label %" << false_label << std::endl;
    createLabel(true_label);
    return make_pair(true_label, false_label);
}
void IRBuilder::createUncondBr(std::string label) {
    os << "  br label %" << label << std::endl;
    return; // br指令没有返回值
}
Instruction* IRBuilder::createPHI(Type* type, std::vector<std::pair<Value*, std::string>> incoming) {
    Instruction* result = new Instruction(newTempReg(), type);
    os << "  " << result->toString() << " = phi " << type->toString() << " ";
    for (size_t i = 0; i < incoming.size(); i++) {
        auto& [value, label] = incoming[i];
        os << "[ " << value->toString() << ", %" << label << " ]";
        if (i != incoming.size() - 1) {
            os << ", ";
        }
    }
    os << std::endl;
    return result;
}
void IRBuilder::createTypeDef(std::string struct_name, std::vector<std::pair<std::string, Type*>> field_types) {
    os << "%struct." << struct_name << " = type { ";
    for (size_t i = 0; i < field_types.size(); ++i) {
        os << field_types[i].second->toString();
        if (i != field_types.size() - 1) {
            os << ", ";
        }
    }
    os << " }" << std::endl;
    return; // 类型定义没有返回值
}
Instruction* IRBuilder::createGetElementPtr(Type* type, Value* ptr, size_t idx) {
    Instruction* result = new Instruction(newTempReg(), new PointerType()); // GEP的结果类型是指向元素类型的指针
    os << "  " << result->toString() << " = getelementptr " << type->toString() << ", ptr " << ptr->toString() << ", i32 " << idx << std::endl;
    return result;
}

}