#pragma once

#include <cstdint>
#include <string>
#include <cstring>
#include <vector>

namespace myllvm {

class Type {
public:
    virtual ~Type() = default;
    virtual std::string toString() const = 0;
    virtual bool isVoid() const;
    virtual bool isStruct() const;
    virtual bool isArray() const;
    virtual bool isPointer() const;
};

class VoidType : public Type {
public:
    VoidType();
    std::string toString() const override;
    bool isVoid() const override;
};

class IntegerType : public Type {
protected:
    uint32_t bit_width;
public:
    virtual uint32_t getBitWidth() const = 0;
};

class Int32Type : public IntegerType {
public:
    Int32Type();
    uint32_t getBitWidth() const override;
    std::string toString() const override;
};

class Int8Type : public IntegerType {
public:
    Int8Type();
    uint32_t getBitWidth() const override;
    std::string toString() const override;
};

class Int1Type : public IntegerType {
public:
    Int1Type();
    uint32_t getBitWidth() const override;
    std::string toString() const override;
};

class StructType : public Type {
private:
    std::string name;
public:
    StructType(std::string name);
    std::string toString() const override;
    bool isStruct() const override;
};

class ArrayType : public Type {
private:
    Type* element_type;
    size_t num_elements;
public:
    ArrayType(Type* element_type, size_t num_elements);
    std::string toString() const override;
    bool isArray() const override;
};

class PointerType : public Type {
public:
    PointerType();
    std::string toString() const override;
    bool isPointer() const override;
};

}
