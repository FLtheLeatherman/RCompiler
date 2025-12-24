#pragma once

#include <string>
#include <cstring>
#include <vector>
#include <memory>
#include <unordered_map>

// 前向声明
class ConstValue;
class FuncSymbol;

using SymbolType = std::string;

class Symbol {
private:
    SymbolType type;
public:
    Symbol(const SymbolType& type);
    virtual ~Symbol() = default;
    void setType(SymbolType);
    SymbolType getType();
};

class ConstSymbol : public Symbol {
private:
    std::string identifier;
    std::shared_ptr<ConstValue> value;
public:
    ConstSymbol(const std::string& identifier, const SymbolType& type);
    ConstSymbol(const std::string& identifier, const SymbolType& type, std::shared_ptr<ConstValue> value);
    std::string getIdentifier() const;
    std::shared_ptr<ConstValue> getValue() const;
    void setValue(std::shared_ptr<ConstValue> value);
    bool hasValue() const;
};

class VariableSymbol : public Symbol {
private:
    std::string identifier;
    bool is_ref;
    bool is_mut;
public:
    VariableSymbol(const std::string identifier, const SymbolType type, bool is_ref = false, bool is_mut = false);
    std::string getIdentifier() const;
    bool isRef() const;
    bool isMut() const;
};

class StructSymbol : public Symbol {
private:
    std::string identifier;
    std::unordered_map<std::string, std::shared_ptr<VariableSymbol>> vars;
    std::unordered_map<std::string, std::shared_ptr<ConstSymbol>> associated_consts;
    std::unordered_map<std::string, std::shared_ptr<FuncSymbol>> methods;      // 带 self 参数的方法
    std::unordered_map<std::string, std::shared_ptr<FuncSymbol>> functions;    // 不带 self 参数的关联函数
public:
    StructSymbol(const std::string& identifier, const SymbolType& type);
    std::string getIdentifier() const;
    
    // 字段管理
    void addField(std::shared_ptr<VariableSymbol> field);
    void eraseField(std::string str);
    bool hasField(const std::string& name) const;
    std::shared_ptr<VariableSymbol> getField(const std::string& name) const;
    std::vector<std::shared_ptr<VariableSymbol>> getFields() const;
    
    // 关联常量管理
    void addAssociatedConst(std::shared_ptr<ConstSymbol> const_symbol);
    bool hasAssociatedConst(const std::string& name) const;
    std::shared_ptr<ConstSymbol> getAssociatedConst(const std::string& name) const;
    std::vector<std::shared_ptr<ConstSymbol>> getAssociatedConsts() const;
    
    // 方法管理（带 self 参数）
    void addMethod(std::shared_ptr<FuncSymbol> method);
    bool hasMethod(const std::string& name) const;
    std::shared_ptr<FuncSymbol> getMethod(const std::string& name) const;
    std::vector<std::shared_ptr<FuncSymbol>> getMethods() const;
    
    // 关联函数管理（不带 self 参数）
    void addAssociatedFunction(std::shared_ptr<FuncSymbol> function);
    bool hasAssociatedFunction(const std::string& name) const;
    std::shared_ptr<FuncSymbol> getAssociatedFunction(const std::string& name) const;
    std::vector<std::shared_ptr<FuncSymbol>> getAssociatedFunctions() const;
    
    // 获取所有关联项（方法 + 关联函数）
    std::vector<std::shared_ptr<FuncSymbol>> getAllAssociatedFunctions() const;
};

class EnumVar {
private:
    std::string identifier;
public:
    EnumVar(const std::string& identifier);
    std::string getIdentifier() const;
};

class EnumSymbol : public Symbol {
private:
    std::string identifier;
    std::vector<std::shared_ptr<EnumVar>> vars;
public:
    EnumSymbol(const std::string& identifier, const SymbolType& type);
    std::string getIdentifier() const;
    void addVariant(std::shared_ptr<EnumVar> variant);
    const std::vector<std::shared_ptr<EnumVar>>& getVariants() const;
};

// 方法类型枚举
enum class MethodType {
    NOT_METHOD,      // 不是方法（普通函数）
    SELF_VALUE,      // self (按值获取)
    SELF_REF,        // &self (不可变引用)
    SELF_MUT_REF,    // &mut self (可变引用)
    SELF_MUT_VALUE   // mut self (按值获取，但可变)
};

class FuncSymbol : public Symbol {
private:
    bool is_const;
    MethodType method_type;
    std::string identifier;
    std::vector<std::shared_ptr<VariableSymbol>> func_params;
    SymbolType return_type;
public:
    FuncSymbol(const std::string& identifier, const SymbolType& return_type, bool is_const = false, MethodType method_type = MethodType::NOT_METHOD);
    std::string getIdentifier() const;
    bool isConst() const;
    bool isMethod() const;
    MethodType getMethodType() const;
    std::string getMethodTypeString() const;
    void addParameter(std::shared_ptr<VariableSymbol> param);
    const std::vector<std::shared_ptr<VariableSymbol>>& getParameters() const;
    SymbolType getReturnType() const;
    void setReturnType(std::string);
};

class TraitSymbol : public Symbol {
private:
    std::string identifier;
    std::unordered_map<std::string, std::shared_ptr<ConstSymbol>> const_symbols;
    std::unordered_map<std::string, std::shared_ptr<FuncSymbol>> methods;      // 带 self 参数的方法
    std::unordered_map<std::string, std::shared_ptr<FuncSymbol>> functions;    // 不带 self 参数的关联函数
public:
    TraitSymbol(const std::string& identifier);
    std::string getIdentifier() const;
    
    // 关联常量管理
    void addConstSymbol(std::shared_ptr<ConstSymbol> const_symbol);
    bool hasConstSymbol(const std::string& name) const;
    std::shared_ptr<ConstSymbol> getConstSymbol(const std::string& name) const;
    std::vector<std::shared_ptr<ConstSymbol>> getConstSymbols() const;
    
    // 方法管理（带 self 参数）
    void addMethod(std::shared_ptr<FuncSymbol> method);
    bool hasMethod(const std::string& name) const;
    std::shared_ptr<FuncSymbol> getMethod(const std::string& name) const;
    std::vector<std::shared_ptr<FuncSymbol>> getMethods() const;
    
    // 关联函数管理（不带 self 参数）
    void addAssociatedFunction(std::shared_ptr<FuncSymbol> function);
    bool hasAssociatedFunction(const std::string& name) const;
    std::shared_ptr<FuncSymbol> getAssociatedFunction(const std::string& name) const;
    std::vector<std::shared_ptr<FuncSymbol>> getAssociatedFunctions() const;
    
    // 获取所有关联项（方法 + 关联函数）
    std::vector<std::shared_ptr<FuncSymbol>> getAllAssociatedFunctions() const;
};

class ArraySymbol : public Symbol {
private:
    std::string identifier;
    std::string element_type;
    std::shared_ptr<ConstValue> length;
public:
    ArraySymbol(const std::string& identifier, const std::string& element_type);
    ArraySymbol(const std::string& identifier, const std::string& element_type, std::shared_ptr<ConstValue> length);
    std::string getIdentifier() const;
    std::string getElementType() const;
    void setElementType(const std::string& element_type);
    std::shared_ptr<ConstValue> getLength() const;
    void setLength(std::shared_ptr<ConstValue> length);
    bool hasLength() const;
};
