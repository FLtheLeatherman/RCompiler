#include "semantic/scope.hpp"
#include "semantic/symbol.hpp"
#include "semantic/const_value.hpp"
#include <iostream>
#include <iomanip>
#include <assert.h>

// 构造函数
Scope::Scope(ScopeType type, std::shared_ptr<Scope> parent)
    : type(type), parent_scope(parent) {
    pos = 0;
}

// 基本访问器
ScopeType Scope::getType() const {
    return type;
}

std::shared_ptr<Scope> Scope::getParent() const {
    return parent_scope;
}

const std::vector<std::shared_ptr<Scope>>& Scope::getChildren() const {
    return children;
}

std::shared_ptr<Scope> Scope::getChild() const {
    // std::cout << pos << ' ' << children.size() << std::endl;
    assert(pos < children.size());
    return children[pos];
}
void Scope::nextChild() {
    pos++;
}
void Scope::resetChild() {
    pos = 0;
}

void Scope::setSelfType(std::string self_type) {
    this->self_type = self_type;
}
std::string Scope::getSelfType() {
    return this->self_type;
}

// 作用域层次结构管理
void Scope::addChild(std::shared_ptr<Scope> child) {
    if (child) {
        children.push_back(child);
        child->setParent(shared_from_this());
    }
}

void Scope::setParent(std::shared_ptr<Scope> parent) {
    parent_scope = parent;
}

// 常量符号管理
void Scope::addConstSymbol(const std::string& name, std::shared_ptr<ConstSymbol> symbol) {
    const_symbols[name] = symbol;
}

std::shared_ptr<ConstSymbol> Scope::getConstSymbol(const std::string& name) const {
    auto it = const_symbols.find(name);
    if (it != const_symbols.end()) {
        return it->second;
    }
    return nullptr;
}

bool Scope::hasConstSymbol(const std::string& name) const {
    return const_symbols.find(name) != const_symbols.end();
}

const std::unordered_map<std::string, std::shared_ptr<ConstSymbol>>& Scope::getConstSymbols() const {
    return const_symbols;
}

// 结构体符号管理
void Scope::addStructSymbol(const std::string& name, std::shared_ptr<StructSymbol> symbol) {
    struct_symbols[name] = symbol;
}

std::shared_ptr<StructSymbol> Scope::getStructSymbol(const std::string& name) const {
    auto it = struct_symbols.find(name);
    if (it != struct_symbols.end()) {
        return it->second;
    }
    return nullptr;
}

bool Scope::hasStructSymbol(const std::string& name) const {
    return struct_symbols.find(name) != struct_symbols.end();
}

const std::unordered_map<std::string, std::shared_ptr<StructSymbol>>& Scope::getStructSymbols() const {
    return struct_symbols;
}

// 枚举符号管理
void Scope::addEnumSymbol(const std::string& name, std::shared_ptr<EnumSymbol> symbol) {
    enum_symbols[name] = symbol;
}

std::shared_ptr<EnumSymbol> Scope::getEnumSymbol(const std::string& name) const {
    auto it = enum_symbols.find(name);
    if (it != enum_symbols.end()) {
        return it->second;
    }
    return nullptr;
}

bool Scope::hasEnumSymbol(const std::string& name) const {
    return enum_symbols.find(name) != enum_symbols.end();
}

const std::unordered_map<std::string, std::shared_ptr<EnumSymbol>>& Scope::getEnumSymbols() const {
    return enum_symbols;
}

// 函数符号管理
void Scope::addFuncSymbol(const std::string& name, std::shared_ptr<FuncSymbol> symbol) {
    func_symbols[name] = symbol;
}

std::shared_ptr<FuncSymbol> Scope::getFuncSymbol(const std::string& name) const {
    auto it = func_symbols.find(name);
    if (it != func_symbols.end()) {
        return it->second;
    }
    return nullptr;
}

bool Scope::hasFuncSymbol(const std::string& name) const {
    return func_symbols.find(name) != func_symbols.end();
}

const std::unordered_map<std::string, std::shared_ptr<FuncSymbol>>& Scope::getFuncSymbols() const {
    return func_symbols;
}

// 特征符号管理
void Scope::addTraitSymbol(const std::string& name, std::shared_ptr<TraitSymbol> symbol) {
    trait_symbols[name] = symbol;
}

std::shared_ptr<TraitSymbol> Scope::getTraitSymbol(const std::string& name) const {
    auto it = trait_symbols.find(name);
    if (it != trait_symbols.end()) {
        return it->second;
    }
    return nullptr;
}

bool Scope::hasTraitSymbol(const std::string& name) const {
    return trait_symbols.find(name) != trait_symbols.end();
}

const std::unordered_map<std::string, std::shared_ptr<TraitSymbol>>& Scope::getTraitSymbols() const {
    return trait_symbols;
}

// 通用符号查找（在作用域链中查找）
std::shared_ptr<Symbol> Scope::findSymbol(const std::string& name) const {
    // 首先在当前作用域查找常量符号
    auto const_sym = getConstSymbol(name);
    if (const_sym) return const_sym;
    
    // 向上查找
    if (parent_scope) {
        return parent_scope->findSymbol(name);
    }
    
    return nullptr;
}

std::shared_ptr<StructSymbol> Scope::findStructSymbol(const std::string& name) const {
    auto struct_sym = getStructSymbol(name);
    if (struct_sym) return struct_sym;
    
    if (parent_scope) {
        return parent_scope->findStructSymbol(name);
    }
    
    return nullptr;
}

std::shared_ptr<EnumSymbol> Scope::findEnumSymbol(const std::string& name) const {
    auto enum_sym = getEnumSymbol(name);
    if (enum_sym) return enum_sym;
    
    if (parent_scope) {
        return parent_scope->findEnumSymbol(name);
    }
    
    return nullptr;
}

std::shared_ptr<FuncSymbol> Scope::findFuncSymbol(const std::string& name) const {
    auto func_sym = getFuncSymbol(name);
    if (func_sym) return func_sym;
    
    if (parent_scope) {
        return parent_scope->findFuncSymbol(name);
    }
    
    return nullptr;
}

std::shared_ptr<TraitSymbol> Scope::findTraitSymbol(const std::string& name) const {
    auto trait_sym = getTraitSymbol(name);
    if (trait_sym) return trait_sym;
    
    if (parent_scope) {
        return parent_scope->findTraitSymbol(name);
    }
    
    return nullptr;
}

// 检查符号是否存在于作用域链中
bool Scope::symbolExists(const std::string& name) const {
    return findSymbol(name) != nullptr;
}

bool Scope::constSymbolExists(const std::string& name) const {
    return findSymbol(name) != nullptr;
}

bool Scope::structSymbolExists(const std::string& name) const {
    return findStructSymbol(name) != nullptr;
}

bool Scope::enumSymbolExists(const std::string& name) const {
    return findEnumSymbol(name) != nullptr;
}

bool Scope::funcSymbolExists(const std::string& name) const {
    return findFuncSymbol(name) != nullptr;
}

bool Scope::traitSymbolExists(const std::string& name) const {
    return findTraitSymbol(name) != nullptr;
}

// 仅在当前作用域中查找
std::shared_ptr<Symbol> Scope::getSymbolInCurrentScope(const std::string& name) const {
    return getConstSymbol(name);
}

bool Scope::hasSymbolInCurrentScope(const std::string& name) const {
    return hasConstSymbol(name);
}

// 调试和输出
void Scope::printScope(int indent) const {
    std::string indent_str(indent, ' ');
    
    // 打印作用域类型
    std::cout << indent_str << "Scope Type: ";
    switch (type) {
        case ScopeType::GLOBAL: std::cout << "GLOBAL"; break;
        case ScopeType::BLOCK: std::cout << "BLOCK"; break;
        case ScopeType::FUNCTION: std::cout << "FUNCTION"; break;
        case ScopeType::TRAIT: std::cout << "TRAIT"; break;
        case ScopeType::IMPL: std::cout << "IMPL " << self_type; break;
        case ScopeType::LOOP: std::cout << "LOOP"; break;
    }
    std::cout << std::endl;

    // 以下符号打印可能需要更详细。
    
    // 打印常量符号
    if (!const_symbols.empty()) {
        std::cout << indent_str << "Constants:" << std::endl;
        for (const auto& [name, symbol] : const_symbols) {
            std::cout << indent_str << "  " << name << ": " << symbol->getType() << std::endl;
        }
    }
    
    // 打印结构体符号
    if (!struct_symbols.empty()) {
        std::cout << indent_str << "Structs:" << std::endl;
        for (const auto& [name, symbol] : struct_symbols) {
            std::cout << indent_str << "  " << name << ": " << symbol->getType() << std::endl;
            auto struct_fields = symbol->getFields();
            for (const auto& symbol_in_struct: struct_fields) {
                std::cout << indent_str << "    " << symbol_in_struct->getIdentifier() << ": " << symbol_in_struct->getType() << std::endl;
            }
        }
    }
    
    // 打印枚举符号
    if (!enum_symbols.empty()) {
        std::cout << indent_str << "Enums:" << std::endl;
        for (const auto& [name, symbol] : enum_symbols) {
            std::cout << indent_str << "  " << name << ": " << symbol->getType() << std::endl;
        }
    }
    
    // 打印函数符号
    if (!func_symbols.empty()) {
        std::cout << indent_str << "Functions:" << std::endl;
        for (const auto& [name, symbol] : func_symbols) {
            std::cout << indent_str << "  " << name << ": " << symbol->getType()
                      << " -> " << symbol->getReturnType() << std::endl;
            auto func_params = symbol->getParameters();
            for (const auto& sb: func_params) {
                std::cout << indent_str << "    Param: " << sb->getIdentifier() 
                          << " Type: " << sb->getType() << std::endl;
            }
        }
    }
    
    // 打印特征符号
    if (!trait_symbols.empty()) {
        std::cout << indent_str << "Traits:" << std::endl;
        for (const auto& [name, symbol] : trait_symbols) {
            std::cout << indent_str << "  " << name << ": " << symbol->getType() << ' ' << symbol->getAllAssociatedFunctions().size() << std::endl;
        }
    }
    
    // 递归打印子作用域
    for (const auto& child : children) {
        std::cout << indent_str << "Child scope:" << std::endl;
        child->printScope(indent + 2);
    }
}

size_t Scope::getFuncSymbolCount() const {
    size_t count = func_symbols.size();
    return count;
}

size_t Scope::getTotalSymbolCount() const {
    size_t count = const_symbols.size() + struct_symbols.size() +
                  enum_symbols.size() + func_symbols.size() + trait_symbols.size();
    
    for (const auto& child : children) {
        count += child->getTotalSymbolCount();
    }
    
    return count;
}

void Scope::clearPos() {
    pos = 0;
    for (const auto& child : children) {
        child->clearPos();
    }
}
