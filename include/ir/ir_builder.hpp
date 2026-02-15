#pragma once

#include "type.hpp"
#include "value.hpp"
#include <cstdint>
#include <unordered_map>
#include <ostream>

namespace myllvm {

class IRBuilder {
private:
    std::ostream &os; // 输出流，用于输出生成的IR代码
    uint32_t temp_count; // 用于生成临时变量的计数器
    std::unordered_map<std::string, uint32_t> label_count; // 用于生成标签的计数器
public:
    IRBuilder(std::ostream &output_stream);
    std::string newTempReg();
    std::string getLabel(std::string);

    Instruction* createBinaryOp(std::string op, Value* lhs, Value* rhs);
    Instruction* createUnaryOp(std::string op, Value* operand);
    LocalVariable* createAlloca(std::string var_name, Type* type);
    Instruction* createLoad(Type* type, Value* ptr);
    void createStore(Type* type, Value* value, Value* ptr);
    void createBranch(std::string label);
    void createLable(std::string label);
    Instruction* createIcmp(std::string cmp, Value* lhs, Value* rhs);
    void createRet(Value* value);
};

}
