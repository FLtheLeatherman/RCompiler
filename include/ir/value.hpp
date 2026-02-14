#pragma once

#include "type.hpp"
#include <vector>
#include <memory>

namespace llvm {

class Value {
protected:
    Type* type;
public:
    Value(Type* type);
    ~Value() = default;
    virtual Type* getType() const;
    virtual std::string toString() const;
};

class Function : public Value {
private:
    std::string function_name;
    bool is_return_struct_or_array;
public:
    Function(const std::string& name, Type* type, bool is_return_struct_or_array);
    bool isReturnStructOrArray() const;
    std::string toString() const override;
};

class GlobalVariable : public Value {
private:
    std::string variable_name;
public:
    GlobalVariable(const std::string& name, Type* type);
    std::string toString() const override;
};

class LocalVariable : public Value {
private:
    std::string variable_name;
public:
    LocalVariable(const std::string& name, Type* type);
    std::string toString() const override;
};

class Instruction : public Value {
private:
    std::string result;
public:
    Instruction(const std::string& result, Type* type);
    std::string toString() const override;
};

}
