#pragma once

#include "type.hpp"
#include "value.hpp"
#include <cstdint>
#include <unordered_map>
#include <iostream>

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
    void createLabel(std::string label);
    Instruction* createIcmp(std::string cmp, Value* lhs, Value* rhs);
    void createRet(Value* value);
    std::pair<std::string, std::string> createBr(Value* condition, std::string true_label_prefix = "if_true", std::string false_label_prefix = "if_false");
    void createUncondBr(std::string label);
    Instruction* createPHI(Type* type, std::vector<std::pair<Value*, std::string>> incoming);
    void createTypeDef(std::string struct_name, std::vector<std::pair<std::string, Type*>> field_types);
    Instruction* createGetElementPtr(Type* type, Value* ptr, size_t idx);
};

}
