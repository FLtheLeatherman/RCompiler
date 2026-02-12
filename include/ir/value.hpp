#pragma once

#include "type.hpp"
#include <vector>
#include <memory>

namespace llvm {

class Value;
class User;

class Value {
public:
    Type* type;
    std::vector<User*> users;
    virtual ~Value() = default;
    virtual std::string toString() const { return ""; }
};

class User : public Value {
public:
    std::vector<Value*> operands;
    virtual ~User() = default;
};

class GlobalVariable : public Value {
public:
    std::string name;
};

class Function : public Value {
public:
    std::string name;
};

class BasicBlock : public Value {
public:
    std::string label;
};

class Instruction : public User {
public:
    virtual ~Instruction() = default;
};

class Constant : public Value {
public:
    virtual ~Constant() = default;
};

class ConstantInt : public Constant {
public:
    uint32_t value;
};

class ConstantStruct : public Constant {
public:
    std::vector<Constant*> elements;
};

class ConstantArray : public Constant {
public:
    std::vector<Constant*> elements;
};

}
