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
bool Type::isPointer() const {
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

StructType::StructType(std::string name, const std::vector<std::pair<std::string, Type*>>& field) 
    : name(name), field(field) {}
std::string StructType::toString() const {
    return name;
}
Type* StructType::getFieldType(size_t idx) const {
    return field[idx].second;
}
size_t StructType::getFieldIdx(std::string field_name) const {
    for (size_t i = 0; i < field.size(); ++i) {
        if (field[i].first == field_name) {
            return i;
        }
    }
    return -1; // 如果字段未找到，返回 -1
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

PointerType::PointerType(Type* pointee_type)
    : pointee_type(pointee_type) {}
std::string PointerType::toString() const {
    return "ptr";
}
Type* PointerType::getPointeeType() const {
    return pointee_type;
}
bool PointerType::isPointer() const {
    return true;
}

}