#include "ir/ir_builder.hpp"

namespace myllvm {

IRBuilder::IRBuilder(std::ostream &output_stream) : os(output_stream) {
    temp_count = 0;
}
std::string IRBuilder::newTempReg() {
    return "%t" + std::to_string(temp_count++);
}
std::string IRBuilder::getLabel(std::string label) {
    if (label_count.find(label) == label_count.end()) {
        label_count[label] = 0;
    }
    return label + std::to_string(label_count[label]++);
}

Instruction* IRBuilder::createBinaryOp(std::string op, Value* lhs, Value* rhs) {
    Instruction* result = new Instruction(newTempReg(), lhs->getType()); // 假设结果类型与操作数相同
    os << "  " << result->toString() << " = " << op << " " << lhs->toString() << ", " << rhs->toString() << std::endl;
    return result;
}
Instruction* IRBuilder::createUnaryOp(std::string op, Value* operand) {
    Instruction* result = new Instruction(newTempReg(), operand->getType()); // 假设结果类型与操作数相同
    if (op == "-") {
        os << "  " << result->toString() << " = " << op << "i32, 0, " << operand->toString() << std::endl;
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
void IRBuilder::createBranch(std::string label) {
    os << "  br label %" << label << std::endl;
    return; // br指令没有返回值
}
void IRBuilder::createLable(std::string label) {
    auto real_label = getLabel(label);
    os << real_label << ":" << std::endl;
    return; // 标签定义没有返回值
}
Instruction* IRBuilder::createIcmp(std::string cmp, Value* lhs, Value* rhs) {
    Instruction* result = new Instruction(newTempReg(), new Int1Type()); // icmp的结果类型是i1
    os << "  " << result->toString() << " = icmp " << cmp << " " << lhs->toString() << ", " << rhs->toString() << std::endl;
    return result;
}

}