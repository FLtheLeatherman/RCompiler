#pragma once

#include "type.hpp"
#include <vector>
#include <memory>

namespace llvm {

class Value {
private:
    std::string name;
    Type* type;
public:
    Value(const std::string& name, Type* type);
    ~Value() = default;
    std::string getName() const;
    Type* getType() const;
    std::string toString() const;
};

}
