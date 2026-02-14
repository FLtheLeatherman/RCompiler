#include "ir/value.hpp"

namespace llvm {

Value::Value(Type* type) : type(type) {}
Type* Value::getType() const {
    return type;
}
std::string Value::toString() const {
    return "";
}

Function::Function(const std::string& name, Type* type, bool is_return_struct_or_array) 
    : Value(type), function_name(name), is_return_struct_or_array(is_return_struct_or_array) {}

bool Function::isReturnStructOrArray() const {
    return is_return_struct_or_array;
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

}