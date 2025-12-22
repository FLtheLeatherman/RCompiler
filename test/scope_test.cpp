#include <iostream>
#include <memory>
#include <string>
#include "../include/semantic/symbol.hpp"
#include "../include/semantic/scope.hpp"

void testBasicScopeOperations() {
    std::cout << "=== 测试基本作用域操作 ===" << std::endl;
    
    // 创建全局作用域
    auto global_scope = std::make_shared<Scope>(ScopeType::GLOBAL);
    
    // 添加各种类型的符号
    auto const_symbol = std::make_shared<ConstSymbol>("MAX_SIZE", "i32");
    auto struct_symbol = std::make_shared<StructSymbol>("Point", "struct");
    auto enum_symbol = std::make_shared<EnumSymbol>("Color", "enum");
    auto func_symbol = std::make_shared<FuncSymbol>("main", "i32");
    auto trait_symbol = std::make_shared<TraitSymbol>("Drawable");
    
    global_scope->addConstSymbol("MAX_SIZE", const_symbol);
    global_scope->addStructSymbol("Point", struct_symbol);
    global_scope->addEnumSymbol("Color", enum_symbol);
    global_scope->addFuncSymbol("main", func_symbol);
    global_scope->addTraitSymbol("Drawable", trait_symbol);
    
    // 测试基本访问器
    std::cout << "作用域类型: " << static_cast<int>(global_scope->getType()) << std::endl;
    std::cout << "父作用域: " << (global_scope->getParent() ? "存在" : "不存在") << std::endl;
    std::cout << "子作用域数量: " << global_scope->getChildren().size() << std::endl;
    
    // 测试符号查找
    std::cout << "常量符号查找: " << (global_scope->hasConstSymbol("MAX_SIZE") ? "找到" : "未找到") << std::endl;
    std::cout << "结构体符号查找: " << (global_scope->hasStructSymbol("Point") ? "找到" : "未找到") << std::endl;
    std::cout << "枚举符号查找: " << (global_scope->hasEnumSymbol("Color") ? "找到" : "未找到") << std::endl;
    std::cout << "函数符号查找: " << (global_scope->hasFuncSymbol("main") ? "找到" : "未找到") << std::endl;
    std::cout << "特征符号查找: " << (global_scope->hasTraitSymbol("Drawable") ? "找到" : "未找到") << std::endl;
    
    // 测试获取符号
    auto found_const = global_scope->getConstSymbol("MAX_SIZE");
    if (found_const) {
        auto const_sym = std::dynamic_pointer_cast<ConstSymbol>(found_const);
        if (const_sym) {
            std::cout << "找到常量: " << const_sym->getIdentifier() << ", 类型: " << const_sym->getType() << std::endl;
        }
    }
    
    auto found_struct = global_scope->getStructSymbol("Point");
    if (found_struct) {
        std::cout << "找到结构体: " << found_struct->getIdentifier() << ", 类型: " << found_struct->getType() << std::endl;
    }
    
    std::cout << "总符号数量: " << global_scope->getTotalSymbolCount() << std::endl;
}

void testScopeHierarchy() {
    std::cout << "\n=== 测试作用域层次结构 ===" << std::endl;
    
    // 创建作用域层次结构
    auto global_scope = std::make_shared<Scope>(ScopeType::GLOBAL);
    auto func_scope = std::make_shared<Scope>(ScopeType::FUNCTION, global_scope);
    auto block_scope = std::make_shared<Scope>(ScopeType::BLOCK, func_scope);
    
    // 建立父子关系
    global_scope->addChild(func_scope);
    func_scope->addChild(block_scope);
    
    // 在不同作用域中添加符号
    global_scope->addConstSymbol("GLOBAL_CONST", std::make_shared<ConstSymbol>("GLOBAL_CONST", "i32"));
    global_scope->addFuncSymbol("global_func", std::make_shared<FuncSymbol>("global_func", "void"));
    
    func_scope->addConstSymbol("FUNC_CONST", std::make_shared<ConstSymbol>("FUNC_CONST", "i32"));
    func_scope->addFuncSymbol("inner_func", std::make_shared<FuncSymbol>("inner_func", "void"));
    
    block_scope->addConstSymbol("BLOCK_CONST", std::make_shared<ConstSymbol>("BLOCK_CONST", "i32"));
    
    // 测试作用域链查找
    std::cout << "从块作用域查找符号:" << std::endl;
    std::cout << "  BLOCK_CONST: " << (block_scope->constSymbolExists("BLOCK_CONST") ? "找到" : "未找到") << std::endl;
    std::cout << "  FUNC_CONST: " << (block_scope->constSymbolExists("FUNC_CONST") ? "找到" : "未找到") << std::endl;
    std::cout << "  GLOBAL_CONST: " << (block_scope->constSymbolExists("GLOBAL_CONST") ? "找到" : "未找到") << std::endl;
    std::cout << "  NONEXISTENT: " << (block_scope->constSymbolExists("NONEXISTENT") ? "找到" : "未找到") << std::endl;
    
    // 测试仅当前作用域查找
    std::cout << "仅当前作用域查找:" << std::endl;
    std::cout << "  BLOCK_CONST: " << (block_scope->hasSymbolInCurrentScope("BLOCK_CONST") ? "找到" : "未找到") << std::endl;
    std::cout << "  FUNC_CONST: " << (block_scope->hasSymbolInCurrentScope("FUNC_CONST") ? "找到" : "未找到") << std::endl;
    std::cout << "  GLOBAL_CONST: " << (block_scope->hasSymbolInCurrentScope("GLOBAL_CONST") ? "找到" : "未找到") << std::endl;
    
    // 测试不同类型符号的作用域链查找
    std::cout << "不同类型符号的作用域链查找:" << std::endl;
    std::cout << "  global_func: " << (block_scope->funcSymbolExists("global_func") ? "找到" : "未找到") << std::endl;
    std::cout << "  inner_func: " << (block_scope->funcSymbolExists("inner_func") ? "找到" : "未找到") << std::endl;
}

void testSymbolTypeSeparation() {
    std::cout << "\n=== 测试符号类型分离 ===" << std::endl;
    
    auto scope = std::make_shared<Scope>(ScopeType::GLOBAL);
    
    // 添加不同类型的符号，但使用相同的名称（如果允许的话）
    scope->addConstSymbol("name", std::make_shared<ConstSymbol>("name", "i32"));
    scope->addStructSymbol("name", std::make_shared<StructSymbol>("name", "struct"));
    scope->addEnumSymbol("name", std::make_shared<EnumSymbol>("name", "enum"));
    scope->addFuncSymbol("name", std::make_shared<FuncSymbol>("name", "void"));
    scope->addTraitSymbol("name", std::make_shared<TraitSymbol>("name"));
    
    // 测试每种类型的符号都可以独立访问
    std::cout << "常量符号 'name': " << (scope->hasConstSymbol("name") ? "存在" : "不存在") << std::endl;
    std::cout << "结构体符号 'name': " << (scope->hasStructSymbol("name") ? "存在" : "不存在") << std::endl;
    std::cout << "枚举符号 'name': " << (scope->hasEnumSymbol("name") ? "存在" : "不存在") << std::endl;
    std::cout << "函数符号 'name': " << (scope->hasFuncSymbol("name") ? "存在" : "不存在") << std::endl;
    std::cout << "特征符号 'name': " << (scope->hasTraitSymbol("name") ? "存在" : "不存在") << std::endl;
    
    // 测试获取不同类型的符号
    auto const_sym = scope->getConstSymbol("name");
    auto struct_sym = scope->getStructSymbol("name");
    auto enum_sym = scope->getEnumSymbol("name");
    auto func_sym = scope->getFuncSymbol("name");
    auto trait_sym = scope->getTraitSymbol("name");
    
    std::cout << "符号类型验证:" << std::endl;
    if (const_sym) std::cout << "  常量: " << const_sym->getType() << std::endl;
    if (struct_sym) std::cout << "  结构体: " << struct_sym->getType() << std::endl;
    if (enum_sym) std::cout << "  枚举: " << enum_sym->getType() << std::endl;
    if (func_sym) std::cout << "  函数: " << func_sym->getType() << " -> " << func_sym->getReturnType() << std::endl;
    if (trait_sym) std::cout << "  特征: " << trait_sym->getType() << std::endl;
}

void testScopePrinting() {
    std::cout << "\n=== 测试作用域打印 ===" << std::endl;
    
    auto global_scope = std::make_shared<Scope>(ScopeType::GLOBAL);
    auto func_scope = std::make_shared<Scope>(ScopeType::FUNCTION);
    auto block_scope = std::make_shared<Scope>(ScopeType::BLOCK);
    
    // 添加符号
    global_scope->addConstSymbol("VERSION", std::make_shared<ConstSymbol>("VERSION", "i32"));
    global_scope->addStructSymbol("User", std::make_shared<StructSymbol>("User", "struct"));
    global_scope->addFuncSymbol("main", std::make_shared<FuncSymbol>("main", "i32"));
    
    func_scope->addConstSymbol("PI", std::make_shared<ConstSymbol>("PI", "f64"));
    func_scope->addFuncSymbol("calculate", std::make_shared<FuncSymbol>("calculate", "f64"));
    
    block_scope->addConstSymbol("temp", std::make_shared<ConstSymbol>("temp", "i32"));
    
    // 建立层次结构
    global_scope->addChild(func_scope);
    func_scope->addChild(block_scope);
    
    // 打印作用域结构
    std::cout << "作用域层次结构:" << std::endl;
    global_scope->printScope();
    
    std::cout << "\n总符号数量: " << global_scope->getTotalSymbolCount() << std::endl;
}

int main() {
    testBasicScopeOperations();
    testScopeHierarchy();
    testSymbolTypeSeparation();
    testScopePrinting();
    return 0;
}