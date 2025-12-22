#include "semantic/const_value.hpp"
#include "parser/astnode.hpp"
#include <sstream>
#include <stdexcept>

// 需要包含具体的字面量类型
#include "parser/astnode.hpp"

// ConstValue 基类实现
ConstValue::ConstValue(std::shared_ptr<ASTNode> node) : expression_node(node) {}

std::shared_ptr<ASTNode> ConstValue::getExpressionNode() const {
    return expression_node;
}

// ConstValueInt 实现
ConstValueInt::ConstValueInt(int value, std::shared_ptr<ASTNode> node) 
    : ConstValue(node), value(value) {}

int ConstValueInt::getValue() const {
    return value;
}

void ConstValueInt::setValue(int value) {
    this->value = value;
}

std::string ConstValueInt::getValueType() const {
    return "i32";
}

std::string ConstValueInt::toString() const {
    return std::to_string(value);
}

// ConstValueBool 实现
ConstValueBool::ConstValueBool(bool value, std::shared_ptr<ASTNode> node) 
    : ConstValue(node), value(value) {}

bool ConstValueBool::getValue() const {
    return value;
}

void ConstValueBool::setValue(bool value) {
    this->value = value;
}

std::string ConstValueBool::getValueType() const {
    return "bool";
}

std::string ConstValueBool::toString() const {
    return value ? "true" : "false";
}

// ConstValueChar 实现
ConstValueChar::ConstValueChar(char value, std::shared_ptr<ASTNode> node) 
    : ConstValue(node), value(value) {}

char ConstValueChar::getValue() const {
    return value;
}

void ConstValueChar::setValue(char value) {
    this->value = value;
}

std::string ConstValueChar::getValueType() const {
    return "char";
}

std::string ConstValueChar::toString() const {
    std::ostringstream oss;
    oss << "'" << value << "'";
    return oss.str();
}

// ConstValueString 实现
ConstValueString::ConstValueString(const std::string& value, std::shared_ptr<ASTNode> node) 
    : ConstValue(node), value(value) {}

const std::string& ConstValueString::getValue() const {
    return value;
}

void ConstValueString::setValue(const std::string& value) {
    this->value = value;
}

std::string ConstValueString::getValueType() const {
    return "str";
}

std::string ConstValueString::toString() const {
    return "\"" + value + "\"";
}

// ConstValueStruct 实现
ConstValueStruct::ConstValueStruct(const std::string& struct_name, std::shared_ptr<ASTNode> node) 
    : ConstValue(node), struct_name(struct_name) {}

const std::string& ConstValueStruct::getStructName() const {
    return struct_name;
}

void ConstValueStruct::setStructName(const std::string& name) {
    struct_name = name;
}

void ConstValueStruct::setField(const std::string& field_name, std::shared_ptr<ConstValue> value) {
    fields[field_name] = value;
}

std::shared_ptr<ConstValue> ConstValueStruct::getField(const std::string& field_name) const {
    auto it = fields.find(field_name);
    if (it != fields.end()) {
        return it->second;
    }
    return nullptr;
}

bool ConstValueStruct::hasField(const std::string& field_name) const {
    return fields.find(field_name) != fields.end();
}

const std::unordered_map<std::string, std::shared_ptr<ConstValue>>& ConstValueStruct::getFields() const {
    return fields;
}

std::string ConstValueStruct::getValueType() const {
    return struct_name;
}

std::string ConstValueStruct::toString() const {
    std::ostringstream oss;
    oss << struct_name << " { ";
    bool first = true;
    for (const auto& [name, value] : fields) {
        if (!first) {
            oss << ", ";
        }
        oss << name << ": " << (value ? value->toString() : "null");
        first = false;
    }
    oss << " }";
    return oss.str();
}

// ConstValueEnum 实现
ConstValueEnum::ConstValueEnum(const std::string& enum_name, const std::string& variant_name, std::shared_ptr<ASTNode> node) 
    : ConstValue(node), enum_name(enum_name), variant_name(variant_name) {}

const std::string& ConstValueEnum::getEnumName() const {
    return enum_name;
}

void ConstValueEnum::setEnumName(const std::string& name) {
    enum_name = name;
}

const std::string& ConstValueEnum::getVariantName() const {
    return variant_name;
}

void ConstValueEnum::setVariantName(const std::string& name) {
    variant_name = name;
}

std::string ConstValueEnum::getValueType() const {
    return enum_name;
}

std::string ConstValueEnum::toString() const {
    return enum_name + "::" + variant_name;
}

// 辅助函数实现
std::shared_ptr<ConstValue> createConstValueFromExpression(std::shared_ptr<ASTNode> expression) {
    if (!expression) {
        return nullptr;
    }
    
    // 尝试转换为不同的字面量类型
    if (auto int_literal = std::dynamic_pointer_cast<IntegerLiteral>(expression)) {
        std::cout << "Debug: Found IntegerLiteral with value: '" << int_literal->value << "'" << std::endl;
        try {
            int value = std::stoi(int_literal->value);
            auto result = std::make_shared<ConstValueInt>(value, expression);
            std::cout << "Debug: Created ConstValueInt with value: " << result->getValue() << std::endl;
            return result;
        } catch (const std::exception& e) {
            std::cout << "Debug: Failed to convert IntegerLiteral: " << e.what() << std::endl;
            // 转换失败，返回 null
            return nullptr;
        }
    }
    
    if (auto bool_literal = std::dynamic_pointer_cast<BoolLiteral>(expression)) {
        return std::make_shared<ConstValueBool>(bool_literal->value, expression);
    }
    
    if (auto char_literal = std::dynamic_pointer_cast<CharLiteral>(expression)) {
        if (!char_literal->value.empty()) {
            return std::make_shared<ConstValueChar>(char_literal->value[0], expression);
        }
    }
    
    if (auto string_literal = std::dynamic_pointer_cast<StringLiteral>(expression)) {
        return std::make_shared<ConstValueString>(string_literal->value, expression);
    }
    
    // 对于更复杂的表达式（如结构体、枚举），需要更复杂的处理
    // 这里暂时返回 null，后续可以根据需要扩展
    return nullptr;
}