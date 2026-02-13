#include "ir/type.hpp"

namespace llvm {

std::string VoidType::toString() const {
    return "void";
}

uint32_t Int32Type::getBitWidth() const {
    return 32;
}
std::string Int32Type::toString() const {
    return "i32";
}

uint32_t Int8Type::getBitWidth() const {
    return 8;
}
std::string Int8Type::toString() const {
    return "i8";
}

uint32_t Int1Type::getBitWidth() const {
    return 1;
}
std::string Int1Type::toString() const {
    return "i1";
}

std::string StructType::toString() const {
    return name;
}

std::string ArrayType::toString() const {
    return "[" + std::to_string(num_elements) + " x " + element_type->toString() + "]";
}

std::string PointerType::toString() const {
    return "ptr";
}

}