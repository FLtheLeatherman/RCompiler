#pragma once

#include "type.hpp"
#include "value.hpp"
#include <cstdint>
#include <unordered_map>
#include <ostream>

namespace llvm {

class IRBuilder {
private:
    std::ostream &os; // 输出流，用于输出生成的IR代码
    uint32_t temp_count; // 用于生成临时变量的计数器
    std::unordered_map<std::string, uint32_t> label_count; // 用于生成标签的计数器
public:
    IRBuilder(std::ostream &output_stream);
    std::string newTempReg();
    std::string getLabel(std::string);

    llvm::Instruction* createBinaryOp(std::string op, llvm::Value* lhs, llvm::Value* rhs);
    llvm::Instruction* createUnaryOp(std::string op, llvm::Value* operand);
    llvm::LocalVariable* createAlloca(std::string var_name, llvm::Type* type);
    llvm::Instruction* createLoad(llvm::Type* type, llvm::Value* ptr);
    void createStore(llvm::Type* type, llvm::Value* value, llvm::Value* ptr);
    void createBranch(std::string label);
    void createLable(std::string label);
};

}
