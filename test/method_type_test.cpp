#include "semantic/symbol.hpp"
#include "semantic/symbol_collector.hpp"
#include "parser/astnode.hpp"
#include <iostream>
#include <memory>

int main() {
    std::cout << "=== 测试 FuncSymbol 方法类型区分 ===" << std::endl;
    
    // 测试不同类型的函数符号创建
    std::cout << "\n1. 测试普通函数（非方法）:" << std::endl;
    auto normal_func = std::make_shared<FuncSymbol>("add", "i32", false, MethodType::NOT_METHOD);
    std::cout << "函数名: " << normal_func->getIdentifier() << std::endl;
    std::cout << "是否为方法: " << (normal_func->isMethod() ? "是" : "否") << std::endl;
    std::cout << "方法类型: " << normal_func->getMethodTypeString() << std::endl;
    
    std::cout << "\n2. 测试 self 方法（按值）:" << std::endl;
    auto self_method = std::make_shared<FuncSymbol>("consume", "()", false, MethodType::SELF_VALUE);
    std::cout << "函数名: " << self_method->getIdentifier() << std::endl;
    std::cout << "是否为方法: " << (self_method->isMethod() ? "是" : "否") << std::endl;
    std::cout << "方法类型: " << self_method->getMethodTypeString() << std::endl;
    
    std::cout << "\n3. 测试 &self 方法（不可变引用）:" << std::endl;
    auto ref_self_method = std::make_shared<FuncSymbol>("get_value", "i32", false, MethodType::SELF_REF);
    std::cout << "函数名: " << ref_self_method->getIdentifier() << std::endl;
    std::cout << "是否为方法: " << (ref_self_method->isMethod() ? "是" : "否") << std::endl;
    std::cout << "方法类型: " << ref_self_method->getMethodTypeString() << std::endl;
    
    std::cout << "\n4. 测试 &mut self 方法（可变引用）:" << std::endl;
    auto mut_ref_self_method = std::make_shared<FuncSymbol>("set_value", "()", false, MethodType::SELF_MUT_REF);
    std::cout << "函数名: " << mut_ref_self_method->getIdentifier() << std::endl;
    std::cout << "是否为方法: " << (mut_ref_self_method->isMethod() ? "是" : "否") << std::endl;
    std::cout << "方法类型: " << mut_ref_self_method->getMethodTypeString() << std::endl;
    
    std::cout << "\n5. 测试 mut self 方法（按值可变）:" << std::endl;
    auto mut_self_method = std::make_shared<FuncSymbol>("transform", "Self", false, MethodType::SELF_MUT_VALUE);
    std::cout << "函数名: " << mut_self_method->getIdentifier() << std::endl;
    std::cout << "是否为方法: " << (mut_self_method->isMethod() ? "是" : "否") << std::endl;
    std::cout << "方法类型: " << mut_self_method->getMethodTypeString() << std::endl;
    
    std::cout << "\n=== 测试完成 ===" << std::endl;
    
    return 0;
}