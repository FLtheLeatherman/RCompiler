#pragma once

#include "type.hpp"
#include "value.hpp"
#include <memory>
#include <unordered_map>
#include <vector>

namespace llvm {

class LLVMContext {
private:
    std::unique_ptr<VoidType> void_type;
    std::unordered_map<unsigned, std::unique_ptr<IntegerType>> integer_types;
    std::unordered_map<std::string, std::unique_ptr<StructType>> struct_types;
    std::unordered_map<std::string, std::unique_ptr<ArrayType>> array_types;
    std::unique_ptr<PointerType> pointer_type;

    std::unordered_map<std::pair<IntegerType*, uint32_t>, std::unique_ptr<ConstantInt>> constant_ints;
    std::unordered_map<std::pair<StructType*, std::vector<Constant*>>, std::unique_ptr<ConstantStruct>> constant_structs;
    std::unordered_map<std::pair<ArrayType*, std::vector<Constant*>>, std::unique_ptr<ConstantArray>> constant_arrays;
public:
    LLVMContext() = default;

    // 若没有这些 Type 对应的 Value，就创建一个。否则返回那个对应的东西。
    VoidType* getVoidType();
    IntegerType* getIntegerType(unsigned bits);
    StructType* getStructType(const std::string& name, const std::vector<Type*>& members);
    ArrayType* getArrayType(Type* element_type, size_t num_elements);
    PointerType* getPointerType();

    ConstantInt* getConstantInt(IntegerType* type, uint32_t value);
    ConstantStruct* getConstantStruct(StructType* type, const std::vector<Constant*>& elements);
    ConstantArray* getConstantArray(ArrayType* type, const std::vector<Constant*>& elements);
};

} // namespace llvm