#include "ir/value.hpp"

namespace llvm {

Value::Value(const std::string& name, Type* type) : name(name), type(type) {}

std::string Value::getName() const {
    return name;
}

Type* Value::getType() const {
    return type;
}

std::string Value::toString() const {
    return name + ": " + type->toString();
}

}