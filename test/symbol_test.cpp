#include <iostream>
#include <memory>
#include <string>
#include "../include/semantic/symbol.hpp"
#include "../include/semantic/scope.hpp"

void testSymbolTypes() {
    std::cout << "=== 测试符号类型 ===" << std::endl;
    
    // 测试 ConstSymbol
    auto const_symbol = std::make_shared<ConstSymbol>("MAX_SIZE", "i32");
    std::cout << "ConstSymbol: " << const_symbol->getIdentifier() 
              << ", type: " << const_symbol->getType() << std::endl;
    
    // 测试 VariableSymbol
    auto var_symbol = std::make_shared<VariableSymbol>("x", "i32", false, true);
    std::cout << "VariableSymbol: " << var_symbol->getIdentifier() 
              << ", type: " << var_symbol->getType()
              << ", is_ref: " << var_symbol->isRef()
              << ", is_mut: " << var_symbol->isMut() << std::endl;
    
    // 测试 StructSymbol
    auto struct_symbol = std::make_shared<StructSymbol>("Point", "struct");
    auto field1 = std::make_shared<VariableSymbol>("x", "i32");
    auto field2 = std::make_shared<VariableSymbol>("y", "i32");
    struct_symbol->addField(field1);
    struct_symbol->addField(field2);
    
    std::cout << "StructSymbol: " << struct_symbol->getIdentifier() 
              << ", type: " << struct_symbol->getType() << std::endl;
    std::cout << "Fields: ";
    for (const auto& field : struct_symbol->getFields()) {
        std::cout << field->getIdentifier() << ":" << field->getType() << " ";
    }
    std::cout << std::endl;
    
    // 测试 EnumSymbol
    auto enum_symbol = std::make_shared<EnumSymbol>("Color", "enum");
    auto variant1 = std::make_shared<EnumVar>("Red");
    auto variant2 = std::make_shared<EnumVar>("Green");
    auto variant3 = std::make_shared<EnumVar>("Blue");
    enum_symbol->addVariant(variant1);
    enum_symbol->addVariant(variant2);
    enum_symbol->addVariant(variant3);
    
    std::cout << "EnumSymbol: " << enum_symbol->getIdentifier() 
              << ", type: " << enum_symbol->getType() << std::endl;
    std::cout << "Variants: ";
    for (const auto& variant : enum_symbol->getVariants()) {
        std::cout << variant->getIdentifier() << " ";
    }
    std::cout << std::endl;
    
    // 测试 FuncSymbol
    auto func_symbol = std::make_shared<FuncSymbol>("add", "i32", false);
    auto param1 = std::make_shared<VariableSymbol>("a", "i32");
    auto param2 = std::make_shared<VariableSymbol>("b", "i32");
    func_symbol->addParameter(param1);
    func_symbol->addParameter(param2);
    
    std::cout << "FuncSymbol: " << func_symbol->getIdentifier() 
              << ", return_type: " << func_symbol->getReturnType()
              << ", is_const: " << func_symbol->isConst() << std::endl;
    std::cout << "Parameters: ";
    for (const auto& param : func_symbol->getParameters()) {
        std::cout << param->getIdentifier() << ":" << param->getType() << " ";
    }
    std::cout << std::endl;
    
    // 测试 TraitSymbol
    auto trait_symbol = std::make_shared<TraitSymbol>("Drawable");
    auto trait_const = std::make_shared<ConstSymbol>("DEFAULT_COLOR", "Color");
    auto trait_func = std::make_shared<FuncSymbol>("draw", "()", false);
    trait_symbol->addConstSymbol(trait_const);
    trait_symbol->addAssociatedFunction(trait_func);
    
    std::cout << "TraitSymbol: " << trait_symbol->getIdentifier() 
              << ", type: " << trait_symbol->getType() << std::endl;
    std::cout << "Associated consts: ";
    for (const auto& const_sym : trait_symbol->getConstSymbols()) {
        std::cout << const_sym->getIdentifier() << " ";
    }
    std::cout << std::endl;
    std::cout << "Associated functions: ";
    for (const auto& func : trait_symbol->getAssociatedFunctions()) {
        std::cout << func->getIdentifier() << " ";
    }
    std::cout << std::endl;
}

void testScopeManagement() {
    std::cout << "\n=== 测试作用域管理 ===" << std::endl;
    
    // 创建全局作用域
    auto global_scope = std::make_shared<Scope>(ScopeType::GLOBAL);
    
    // 添加全局符号
    auto global_const = std::make_shared<ConstSymbol>("PI", "f64");
    auto global_func = std::make_shared<FuncSymbol>("main", "i32", false);
    global_scope->addSymbol("PI", global_const);
    global_scope->addSymbol("main", global_func);
    
    std::cout << "Global scope symbols:" << std::endl;
    for (const auto& [name, symbol] : global_scope->getSymbols()) {
        std::cout << "  " << name << ": " << symbol->getType() << std::endl;
    }
    
    // 创建函数作用域
    auto func_scope = std::make_shared<Scope>(ScopeType::FUNCTION, global_scope);
    global_scope->addChild(func_scope);
    
    // 添加函数参数和局部变量
    auto param = std::make_shared<VariableSymbol>("x", "i32");
    auto local_var = std::make_shared<VariableSymbol>("y", "i32", false, true);
    func_scope->addSymbol("x", param);
    func_scope->addSymbol("y", local_var);
    
    std::cout << "Function scope symbols:" << std::endl;
    for (const auto& [name, symbol] : func_scope->getSymbols()) {
        std::cout << "  " << name << ": " << symbol->getType() << std::endl;
    }
    
    // 测试符号查找
    std::cout << "Symbol lookup tests:" << std::endl;
    std::cout << "  Looking for 'y' in func_scope: " 
              << (func_scope->hasSymbol("y") ? "found" : "not found") << std::endl;
    std::cout << "  Looking for 'PI' in func_scope: " 
              << (func_scope->hasSymbol("PI") ? "found" : "not found") << std::endl;
    std::cout << "  Looking for 'PI' in global_scope: " 
              << (global_scope->hasSymbolInScope("PI") ? "found" : "not found") << std::endl;
    std::cout << "  Looking for 'y' in global_scope: " 
              << (global_scope->hasSymbolInScope("y") ? "found" : "not found") << std::endl;
    
    // 创建块作用域
    auto block_scope = std::make_shared<Scope>(ScopeType::BLOCK, func_scope);
    func_scope->addChild(block_scope);
    
    auto block_var = std::make_shared<VariableSymbol>("z", "i32");
    block_scope->addSymbol("z", block_var);
    
    std::cout << "Block scope symbols:" << std::endl;
    for (const auto& [name, symbol] : block_scope->getSymbols()) {
        std::cout << "  " << name << ": " << symbol->getType() << std::endl;
    }
    
    // 测试作用域链查找
    std::cout << "Scope chain lookup from block_scope:" << std::endl;
    std::cout << "  'z': " << (block_scope->getSymbol("z") ? "found" : "not found") << std::endl;
    std::cout << "  'y': " << (block_scope->getSymbol("y") ? "found" : "not found") << std::endl;
    std::cout << "  'PI': " << (block_scope->getSymbol("PI") ? "found" : "not found") << std::endl;
    std::cout << "  'nonexistent': " << (block_scope->getSymbol("nonexistent") ? "found" : "not found") << std::endl;
}

int main() {
    testSymbolTypes();
    testScopeManagement();
    return 0;
}