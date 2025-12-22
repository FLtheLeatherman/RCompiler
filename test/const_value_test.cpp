#include <iostream>
#include <memory>
#include <cassert>
#include "../include/semantic/const_value.hpp"
#include "../include/parser/astnode.hpp"

void testConstValueInt() {
    std::cout << "=== 测试 ConstValueInt ===" << std::endl;
    
    auto int_literal = std::make_shared<IntegerLiteral>("42");
    auto const_int = std::make_shared<ConstValueInt>(42, int_literal);
    
    assert(const_int->isInt());
    assert(!const_int->isBool());
    assert(const_int->getValue() == 42);
    assert(const_int->getValueType() == "i32");
    assert(const_int->toString() == "42");
    assert(const_int->getExpressionNode() == int_literal);
    
    const_int->setValue(100);
    assert(const_int->getValue() == 100);
    assert(const_int->toString() == "100");
    
    std::cout << "✅ ConstValueInt 测试通过" << std::endl;
}

void testConstValueBool() {
    std::cout << "=== 测试 ConstValueBool ===" << std::endl;
    
    auto bool_literal = std::make_shared<BoolLiteral>(true);
    auto const_bool = std::make_shared<ConstValueBool>(true, bool_literal);
    
    assert(const_bool->isBool());
    assert(!const_bool->isInt());
    assert(const_bool->getValue() == true);
    assert(const_bool->getValueType() == "bool");
    assert(const_bool->toString() == "true");
    assert(const_bool->getExpressionNode() == bool_literal);
    
    const_bool->setValue(false);
    assert(const_bool->getValue() == false);
    assert(const_bool->toString() == "false");
    
    std::cout << "✅ ConstValueBool 测试通过" << std::endl;
}

void testConstValueChar() {
    std::cout << "=== 测试 ConstValueChar ===" << std::endl;
    
    auto char_literal = std::make_shared<CharLiteral>("A");
    auto const_char = std::make_shared<ConstValueChar>('A', char_literal);
    
    assert(const_char->isChar());
    assert(!const_char->isInt());
    assert(const_char->getValue() == 'A');
    assert(const_char->getValueType() == "char");
    assert(const_char->toString() == "'A'");
    assert(const_char->getExpressionNode() == char_literal);
    
    const_char->setValue('B');
    assert(const_char->getValue() == 'B');
    assert(const_char->toString() == "'B'");
    
    std::cout << "✅ ConstValueChar 测试通过" << std::endl;
}

void testConstValueString() {
    std::cout << "=== 测试 ConstValueString ===" << std::endl;
    
    auto string_literal = std::make_shared<StringLiteral>("Hello");
    auto const_string = std::make_shared<ConstValueString>("Hello", string_literal);
    
    assert(const_string->isString());
    assert(!const_string->isInt());
    assert(const_string->getValue() == "Hello");
    assert(const_string->getValueType() == "str");
    assert(const_string->toString() == "\"Hello\"");
    assert(const_string->getExpressionNode() == string_literal);
    
    const_string->setValue("World");
    assert(const_string->getValue() == "World");
    assert(const_string->toString() == "\"World\"");
    
    std::cout << "✅ ConstValueString 测试通过" << std::endl;
}

void testConstValueStruct() {
    std::cout << "=== 测试 ConstValueStruct ===" << std::endl;
    
    auto struct_expr = std::make_shared<Expression>();
    auto const_struct = std::make_shared<ConstValueStruct>("Point", struct_expr);
    
    assert(const_struct->isStruct());
    assert(!const_struct->isInt());
    assert(const_struct->getStructName() == "Point");
    assert(const_struct->getValueType() == "Point");
    assert(const_struct->getExpressionNode() == struct_expr);
    
    // 添加字段
    auto x_field = std::make_shared<ConstValueInt>(10, nullptr);
    auto y_field = std::make_shared<ConstValueInt>(20, nullptr);
    
    const_struct->setField("x", x_field);
    const_struct->setField("y", y_field);
    
    assert(const_struct->hasField("x"));
    assert(const_struct->hasField("y"));
    assert(!const_struct->hasField("z"));
    
    assert(const_struct->getField("x") == x_field);
    assert(const_struct->getField("y") == y_field);
    assert(const_struct->getField("z") == nullptr);
    
    std::cout << "结构体内容: " << const_struct->toString() << std::endl;
    
    std::cout << "✅ ConstValueStruct 测试通过" << std::endl;
}

void testConstValueEnum() {
    std::cout << "=== 测试 ConstValueEnum ===" << std::endl;
    
    auto enum_expr = std::make_shared<Expression>();
    auto const_enum = std::make_shared<ConstValueEnum>("Color", "Red", enum_expr);
    
    assert(const_enum->isEnum());
    assert(!const_enum->isInt());
    assert(const_enum->getEnumName() == "Color");
    assert(const_enum->getVariantName() == "Red");
    assert(const_enum->getValueType() == "Color");
    assert(const_enum->getExpressionNode() == enum_expr);
    
    const_enum->setVariantName("Green");
    assert(const_enum->getVariantName() == "Green");
    
    std::cout << "枚举内容: " << const_enum->toString() << std::endl;
    
    std::cout << "✅ ConstValueEnum 测试通过" << std::endl;
}

void testCreateConstValueFromExpression() {
    std::cout << "=== 测试从表达式创建 ConstValue ===" << std::endl;
    
    // 测试整型字面量
    auto int_literal = std::make_shared<IntegerLiteral>("123");
    auto int_const = createConstValueFromExpression(int_literal);
    assert(int_const != nullptr);
    assert(int_const->isInt());
    assert(std::dynamic_pointer_cast<ConstValueInt>(int_const)->getValue() == 123);
    
    // 测试布尔字面量
    auto bool_literal = std::make_shared<BoolLiteral>(false);
    auto bool_const = createConstValueFromExpression(bool_literal);
    assert(bool_const != nullptr);
    assert(bool_const->isBool());
    assert(std::dynamic_pointer_cast<ConstValueBool>(bool_const)->getValue() == false);
    
    // 测试字符字面量
    auto char_literal = std::make_shared<CharLiteral>("X");
    auto char_const = createConstValueFromExpression(char_literal);
    assert(char_const != nullptr);
    assert(char_const->isChar());
    assert(std::dynamic_pointer_cast<ConstValueChar>(char_const)->getValue() == 'X');
    
    // 测试字符串字面量
    auto string_literal = std::make_shared<StringLiteral>("Test");
    auto string_const = createConstValueFromExpression(string_literal);
    assert(string_const != nullptr);
    assert(string_const->isString());
    assert(std::dynamic_pointer_cast<ConstValueString>(string_const)->getValue() == "Test");
    
    std::cout << "✅ 从表达式创建 ConstValue 测试通过" << std::endl;
}

int main() {
    try {
        testConstValueInt();
        testConstValueBool();
        testConstValueChar();
        testConstValueString();
        testConstValueStruct();
        testConstValueEnum();
        testCreateConstValueFromExpression();
        
        std::cout << "\n🎉 所有 ConstValue 测试通过！" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "❌ 测试失败: " << e.what() << std::endl;
        return 1;
    }
}