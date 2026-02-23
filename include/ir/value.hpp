#pragma once

#include "type.hpp"
#include <vector>
#include <memory>

namespace myllvm {

class Value {
protected:
    Type* type;
public:
    Value(Type* type);
    ~Value() = default;
    virtual Type* getType() const;
    void setType(Type* type);
    virtual std::string toString() const;
};

class Function : public Value {
private:
    std::string function_name;
    bool has_self;
    bool self_ref;
    std::vector<bool> param_is_ref;
public:
    Function(const std::string& name, Type* type, bool has_self, bool self_ref);
    bool hasSelf() const;
    bool isSelfRef() const;
    bool isParamRef(size_t index) const;
    void newParamIsRef(bool is_ref);
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

class Constant : public Value {
private:    
    std::string value;
public:    
    Constant(const std::string& value, Type* type);
    std::string toString() const override;
};

}
