#pragma once

#include <cstdint>
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
    std::string toString() const override;
};

class IntegerType : public Type {
private:
    uint32_t bit_width;
    virtual uint32_t getBitWidth() const = 0;
};

class Int32Type : public IntegerType {
public:
    uint32_t getBitWidth() const override;
    std::string toString() const override;
};

class Int8Type : public IntegerType {
public:
    uint32_t getBitWidth() const override;
    std::string toString() const override;
};

class Int1Type : public IntegerType {
public:
    uint32_t getBitWidth() const override;
    std::string toString() const override;
};

class StructType : public Type {
private:
    std::string name;
    std::vector<Type*> members;
public:
    std::string toString() const override;
};

class ArrayType : public Type {
private:
    Type* element_type;
    size_t num_elements;
public:
    std::string toString() const override;
};

class PointerType : public Type {
public:
    std::string toString() const override;
};

}
