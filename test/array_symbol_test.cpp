#include <iostream>
#include <memory>
#include <cassert>
#include "../include/semantic/symbol.hpp"
#include "../include/semantic/const_value.hpp"
#include "../include/semantic/scope.hpp"

void testArraySymbolBasic() {
    std::cout << "=== 测试 ArraySymbol 基本功能 ===" << std::endl;
    
    // 创建基本的数组符号
    auto array_symbol = std::make_shared<ArraySymbol>("numbers", "i32");
    
    assert(array_symbol->getIdentifier() == "numbers");
    assert(array_symbol->getElementType() == "i32");
    assert(array_symbol->getType() == "array");
    assert(!array_symbol->hasLength());
    assert(array_symbol->getLength() == nullptr);
    
    std::cout << "✅ ArraySymbol 基本功能测试通过" << std::endl;
}

void testArraySymbolWithLength() {
    std::cout << "=== 测试 ArraySymbol 带长度 ===" << std::endl;
    
    // 创建带长度的数组符号
    auto length_value = std::make_shared<ConstValueInt>(5, nullptr);
    auto array_symbol = std::make_shared<ArraySymbol>("fixed_numbers", "i32", length_value);
    
    assert(array_symbol->getIdentifier() == "fixed_numbers");
    assert(array_symbol->getElementType() == "i32");
    assert(array_symbol->getType() == "array");
    assert(array_symbol->hasLength());
    assert(array_symbol->getLength() == length_value);
    assert(array_symbol->getLength()->isInt());
    assert(std::dynamic_pointer_cast<ConstValueInt>(array_symbol->getLength())->getValue() == 5);
    
    std::cout << "数组信息: " << array_symbol->getIdentifier() 
              << " [" << array_symbol->getElementType() << "]"
              << " (length: " << array_symbol->getLength()->toString() << ")" << std::endl;
    
    std::cout << "✅ ArraySymbol 带长度测试通过" << std::endl;
}

void testArraySymbolSetters() {
    std::cout << "=== 测试 ArraySymbol 设置方法 ===" << std::endl;
    
    auto array_symbol = std::make_shared<ArraySymbol>("data", "bool");
    
    // 测试设置元素类型
    array_symbol->setElementType("char");
    assert(array_symbol->getElementType() == "char");
    
    // 测试设置长度
    auto new_length = std::make_shared<ConstValueInt>(10, nullptr);
    array_symbol->setLength(new_length);
    assert(array_symbol->hasLength());
    assert(array_symbol->getLength() == new_length);
    assert(std::dynamic_pointer_cast<ConstValueInt>(array_symbol->getLength())->getValue() == 10);
    
    // 测试修改长度
    auto modified_length = std::make_shared<ConstValueInt>(15, nullptr);
    array_symbol->setLength(modified_length);
    assert(array_symbol->getLength() == modified_length);
    assert(std::dynamic_pointer_cast<ConstValueInt>(array_symbol->getLength())->getValue() == 15);
    
    std::cout << "✅ ArraySymbol 设置方法测试通过" << std::endl;
}

void testArraySymbolInScope() {
    std::cout << "=== 测试 ArraySymbol 在作用域中的使用 ===" << std::endl;
    
    auto scope = std::make_shared<Scope>(ScopeType::GLOBAL);
    
    // 创建不同类型的数组符号
    auto int_array = std::make_shared<ArraySymbol>("int_arr", "i32");
    auto bool_array = std::make_shared<ArraySymbol>("bool_arr", "bool");
    auto string_array = std::make_shared<ArraySymbol>("str_arr", "str");
    
    // 添加到作用域
    scope->addArraySymbol("int_arr", int_array);
    scope->addArraySymbol("bool_arr", bool_array);
    scope->addArraySymbol("str_arr", string_array);
    
    // 测试查找
    assert(scope->hasArraySymbol("int_arr"));
    assert(scope->hasArraySymbol("bool_arr"));
    assert(scope->hasArraySymbol("str_arr"));
    assert(!scope->hasArraySymbol("nonexistent"));
    
    auto found_int = scope->getArraySymbol("int_arr");
    auto found_bool = scope->getArraySymbol("bool_arr");
    auto found_str = scope->getArraySymbol("str_arr");
    auto not_found = scope->getArraySymbol("nonexistent");
    
    assert(found_int == int_array);
    assert(found_bool == bool_array);
    assert(found_str == string_array);
    assert(not_found == nullptr);
    
    // 测试作用域链查找
    assert(scope->findArraySymbol("int_arr") == int_array);
    assert(scope->findArraySymbol("nonexistent") == nullptr);
    
    std::cout << "作用域中的数组符号:" << std::endl;
    for (const auto& [name, symbol] : scope->getArraySymbols()) {
        std::cout << "  " << name << ": [" << symbol->getElementType() << "]" << std::endl;
    }
    
    std::cout << "✅ ArraySymbol 作用域测试通过" << std::endl;
}

void testConstSymbolWithValue() {
    std::cout << "=== 测试 ConstSymbol 支持 ConstValue ===" << std::endl;
    
    // 创建带值的常量符号
    auto const_value = std::make_shared<ConstValueInt>(42, nullptr);
    auto const_symbol = std::make_shared<ConstSymbol>("MAX_VALUE", "i32", const_value);
    
    assert(const_symbol->getIdentifier() == "MAX_VALUE");
    assert(const_symbol->getType() == "i32");
    assert(const_symbol->hasValue());
    assert(const_symbol->getValue() == const_value);
    assert(const_symbol->getValue()->isInt());
    assert(std::dynamic_pointer_cast<ConstValueInt>(const_symbol->getValue())->getValue() == 42);
    
    // 测试修改值
    auto new_value = std::make_shared<ConstValueInt>(100, nullptr);
    const_symbol->setValue(new_value);
    assert(const_symbol->getValue() == new_value);
    assert(std::dynamic_pointer_cast<ConstValueInt>(const_symbol->getValue())->getValue() == 100);
    
    // 测试创建不带值的常量符号
    auto empty_const = std::make_shared<ConstSymbol>("EMPTY_CONST", "i32");
    assert(!empty_const->hasValue());
    assert(empty_const->getValue() == nullptr);
    
    std::cout << "常量值: " << const_symbol->getIdentifier() 
              << " = " << const_symbol->getValue()->toString() << std::endl;
    
    std::cout << "✅ ConstSymbol 支持 ConstValue 测试通过" << std::endl;
}

int main() {
    try {
        testArraySymbolBasic();
        testArraySymbolWithLength();
        testArraySymbolSetters();
        testArraySymbolInScope();
        testConstSymbolWithValue();
        
        std::cout << "\n🎉 所有 ArraySymbol 和 ConstSymbol 测试通过！" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "❌ 测试失败: " << e.what() << std::endl;
        return 1;
    }
}