#include "ir/ir_builder.hpp"

namespace llvm {

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

llvm::Instruction* IRBuilder::createBinaryOp(std::string op, llvm::Value* lhs, llvm::Value* rhs) {
    llvm::Instruction* result = new llvm::Instruction(newTempReg(), lhs->getType()); // 假设结果类型与操作数相同
    os << "  " << result->toString() << " = " << op << " " << lhs->toString() << ", " << rhs->toString() << std::endl;
    return result;
}
llvm::Instruction* IRBuilder::createUnaryOp(std::string op, llvm::Value* operand) {
    llvm::Instruction* result = new llvm::Instruction(newTempReg(), operand->getType()); // 假设结果类型与操作数相同
    if (op == "-") {
        os << "  " << result->toString() << " = " << op << "i32, 0, " << operand->toString() << std::endl;
    } else {
        throw std::runtime_error("Unsupported unary operator: " + op);
    }
    return result;
}
llvm::LocalVariable* IRBuilder::createAlloca(std::string var_name, llvm::Type* type) {
    llvm::LocalVariable* result = new llvm::LocalVariable(var_name, type);
    os << "  " << result->toString() << " = alloca " << type->toString() << std::endl;
    return result;
}
llvm::Instruction* IRBuilder::createLoad(llvm::Type* type, llvm::Value* ptr) {
    llvm::Instruction* result = new llvm::Instruction(newTempReg(), type);
    os << "  " << result->toString() << " = load " << type->toString() << ", " << ptr->toString() << std::endl;
    return result;
}
void IRBuilder::createStore(llvm::Type* type, llvm::Value* value, llvm::Value* ptr) {
    os << "  store " << type->toString() << " " << value->toString() << ", " << ptr->toString() << std::endl;
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

}