#include "ir/value.hpp"

namespace myllvm {

Value::Value(Type* type) : type(type) {}
Type* Value::getType() const {
    return type;
}
void Value::setType(Type* type) {
    this->type = type;
}
std::string Value::toString() const {
    return "";
}

Function::Function(const std::string& name, Type* type, bool has_self, bool self_ref) 
    : Value(type), function_name(name), has_self(has_self), self_ref(self_ref) {
    param_is_ref.clear();
}

bool Function::hasSelf() const {
    return has_self;
}
bool Function::isSelfRef() const {
    return self_ref;
}
bool Function::isParamRef(size_t index) const {
    if (index >= param_is_ref.size()) {
        throw std::out_of_range("Parameter index out of range");
    }
    return param_is_ref[index];
}
void Function::newParamIsRef(bool is_ref) {
    param_is_ref.push_back(is_ref);
}
std::string Function::toString() const {
    return function_name;
}

GlobalVariable::GlobalVariable(const std::string& name, Type* type) 
    : Value(type), variable_name(name) {}
std::string GlobalVariable::toString() const {
    return variable_name;
}

LocalVariable::LocalVariable(const std::string& name, Type* type) 
    : Value(type), variable_name(name) {}
std::string LocalVariable::toString() const {
    return variable_name;
}

Instruction::Instruction(const std::string& result, Type* type)
    : Value(type), result(result) {}
std::string Instruction::toString() const {
    return result;
}

Constant::Constant(const std::string& value, Type* type)
    : Value(type), value(value) {}
std::string Constant::toString() const {
    return value;
}

}