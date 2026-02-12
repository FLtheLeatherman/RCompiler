#pragma once

#include <string>
#include <cstring>
#include <vector>

namespace llvm {

class Type {
public:
    virtual ~Type() = default;
    virtual std::string toString() const = 0;
};

class VoidType : public Type {
public:
};

class IntegerType : public Type {
public:
};

class Int32Type : public IntegerType {
public:
};

class Int8Type : public IntegerType {
public:
};

class Int1Type : public IntegerType {
public:
};

class StructType : public Type {
public:
    std::string name;
    std::vector<Type*> members;
};

class ArrayType : public Type {
public:
    Type* element_type;
    size_t num_elements;
};

class PointerType : public Type {
public:
};

class FunctionType : public Type {
public:
    Type* return_type;
    std::vector<Type*> param_types;
};

class InstructionType : public Type {
public:
};

}
