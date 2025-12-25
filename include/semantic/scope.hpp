#pragma once

#include <memory>
#include <vector>
#include <unordered_map>

#include "symbol.hpp"

// 变量信息结构体，包含类型和可变性标记
struct VariableInfo {
    std::string type;
    bool is_mutable;
    
    VariableInfo() : type(""), is_mutable(false) {}
    VariableInfo(const std::string& t, bool mut = false) : type(t), is_mutable(mut) {}
};

enum class ScopeType {
    GLOBAL,
    BLOCK,
    FUNCTION,
    TRAIT,
    IMPL,
    LOOP
};

class Scope : public std::enable_shared_from_this<Scope> {
private:
    ScopeType type;
    size_t pos; // 目前应该访问哪个 children?
    std::string self_type; // for impl scope
    std::shared_ptr<Scope> parent_scope;
    std::vector<std::shared_ptr<Scope>> children;
    std::unordered_map<std::string, std::shared_ptr<ConstSymbol>> const_symbols;
    std::unordered_map<std::string, std::shared_ptr<StructSymbol>> struct_symbols;
    std::unordered_map<std::string, std::shared_ptr<EnumSymbol>> enum_symbols;
    std::unordered_map<std::string, std::shared_ptr<FuncSymbol>> func_symbols;
    std::unordered_map<std::string, std::shared_ptr<TraitSymbol>> trait_symbols;
    std::unordered_map<std::string, VariableInfo> variable_table;

public:
    // 构造函数
    Scope(ScopeType type, std::shared_ptr<Scope> parent = nullptr);
    
    // 基本访问器
    ScopeType getType() const;
    std::shared_ptr<Scope> getParent() const;
    const std::vector<std::shared_ptr<Scope>>& getChildren() const;
    std::shared_ptr<Scope> getChild() const;
    void nextChild();
    void resetChild();
    void setSelfType(std::string);
    std::string getSelfType();
    
    // 作用域层次结构管理
    void addChild(std::shared_ptr<Scope> child);
    void setParent(std::shared_ptr<Scope> parent);
    
    // 常量符号管理
    void addConstSymbol(const std::string& name, std::shared_ptr<ConstSymbol> symbol);
    std::shared_ptr<ConstSymbol> getConstSymbol(const std::string& name) const;
    bool hasConstSymbol(const std::string& name) const;
    const std::unordered_map<std::string, std::shared_ptr<ConstSymbol>>& getConstSymbols() const;
    
    // 结构体符号管理
    void addStructSymbol(const std::string& name, std::shared_ptr<StructSymbol> symbol);
    std::shared_ptr<StructSymbol> getStructSymbol(const std::string& name) const;
    bool hasStructSymbol(const std::string& name) const;
    const std::unordered_map<std::string, std::shared_ptr<StructSymbol>>& getStructSymbols() const;
    
    // 枚举符号管理
    void addEnumSymbol(const std::string& name, std::shared_ptr<EnumSymbol> symbol);
    std::shared_ptr<EnumSymbol> getEnumSymbol(const std::string& name) const;
    bool hasEnumSymbol(const std::string& name) const;
    const std::unordered_map<std::string, std::shared_ptr<EnumSymbol>>& getEnumSymbols() const;
    
    // 函数符号管理
    void addFuncSymbol(const std::string& name, std::shared_ptr<FuncSymbol> symbol);
    std::shared_ptr<FuncSymbol> getFuncSymbol(const std::string& name) const;
    bool hasFuncSymbol(const std::string& name) const;
    const std::unordered_map<std::string, std::shared_ptr<FuncSymbol>>& getFuncSymbols() const;
    
    // 特征符号管理
    void addTraitSymbol(const std::string& name, std::shared_ptr<TraitSymbol> symbol);
    std::shared_ptr<TraitSymbol> getTraitSymbol(const std::string& name) const;
    bool hasTraitSymbol(const std::string& name) const;
    const std::unordered_map<std::string, std::shared_ptr<TraitSymbol>>& getTraitSymbols() const;
    
    // 变量表管理
    void addVariable(const std::string& name, const std::string& type, bool is_mutable = false);
    std::string getVariableType(const std::string& name) const;
    bool isVariableMutable(const std::string& name) const;
    bool hasVariable(const std::string& name) const;
    const std::unordered_map<std::string, VariableInfo>& getVariableTable() const;
    std::string findVariableType(const std::string& name) const; // 在作用域链中查找
    bool findVariableMutable(const std::string& name) const; // 在作用域链中查找变量可变性
    bool variableExists(const std::string& name) const; // 在作用域链中检查变量是否存在
    
    // 通用符号查找（在作用域链中查找）
    std::shared_ptr<Symbol> findSymbol(const std::string& name) const;
    std::shared_ptr<StructSymbol> findStructSymbol(const std::string& name) const;
    std::shared_ptr<EnumSymbol> findEnumSymbol(const std::string& name) const;
    std::shared_ptr<FuncSymbol> findFuncSymbol(const std::string& name) const;
    std::shared_ptr<TraitSymbol> findTraitSymbol(const std::string& name) const;
    
    // 检查符号是否存在于作用域链中
    bool symbolExists(const std::string& name) const;
    bool constSymbolExists(const std::string& name) const;
    bool structSymbolExists(const std::string& name) const;
    bool enumSymbolExists(const std::string& name) const;
    bool funcSymbolExists(const std::string& name) const;
    bool traitSymbolExists(const std::string& name) const;
    
    // 仅在当前作用域中查找
    std::shared_ptr<Symbol> getSymbolInCurrentScope(const std::string& name) const;
    bool hasSymbolInCurrentScope(const std::string& name) const;
    
    // 调试和输出
    void printScope(int indent = 0) const;
    size_t getFuncSymbolCount() const;
    size_t getTotalSymbolCount() const;
    void clearPos();
};