#include "ir/type.hpp"

namespace myllvm {

VoidType::VoidType() {}
std::string VoidType::toString() const {
    return "void";
}
bool Type::isVoid() const {
    return false;
}
bool Type::isStruct() const {
    return false;
}
bool Type::isArray() const {
    return false;
}

bool VoidType::isVoid() const {
    return true;
}

Int32Type::Int32Type() {
    bit_width = 32;
}
uint32_t Int32Type::getBitWidth() const {
    return 32;
}
std::string Int32Type::toString() const {
    return "i32";
}

Int8Type::Int8Type() {
    bit_width = 8;
}
uint32_t Int8Type::getBitWidth() const {
    return 8;
}
std::string Int8Type::toString() const {
    return "i8";
}

Int1Type::Int1Type() {
    bit_width = 1;
}
uint32_t Int1Type::getBitWidth() const {
    return 1;
}
std::string Int1Type::toString() const {
    return "i1";
}

StructType::StructType(std::string name) : name(name) {}
std::string StructType::toString() const {
    return name;
}
bool StructType::isStruct() const {
    return true;
}

ArrayType::ArrayType(Type* element_type, size_t num_elements) : element_type(element_type), num_elements(num_elements) {}
std::string ArrayType::toString() const {
    return "[" + std::to_string(num_elements) + " x " + element_type->toString() + "]";
}
bool ArrayType::isArray() const {
    return true;
}

PointerType::PointerType() {}
std::string PointerType::toString() const {
    return "ptr";
}

}