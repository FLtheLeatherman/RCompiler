#pragma once

#include <string>
#include <cstring>
#include <vector>
#include <memory>

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
    VariableSymbol(const std::string& identifier, const SymbolType& type, bool is_ref = false, bool is_mut = false);
    std::string getIdentifier() const;
    bool isRef() const;
    bool isMut() const;
};

class StructSymbol : public Symbol {
private:
    std::string identifier;
    std::vector<std::shared_ptr<VariableSymbol>> vars;
    std::vector<std::shared_ptr<ConstSymbol>> associated_consts;
    std::vector<std::shared_ptr<FuncSymbol>> methods;      // 带 self 参数的方法
    std::vector<std::shared_ptr<FuncSymbol>> functions;    // 不带 self 参数的关联函数
public:
    StructSymbol(const std::string& identifier, const SymbolType& type);
    std::string getIdentifier() const;
    void addField(std::shared_ptr<VariableSymbol> field);
    const std::vector<std::shared_ptr<VariableSymbol>>& getFields() const;
    
    // 关联常量管理
    void addAssociatedConst(std::shared_ptr<ConstSymbol> const_symbol);
    const std::vector<std::shared_ptr<ConstSymbol>>& getAssociatedConsts() const;
    
    // 方法管理（带 self 参数）
    void addMethod(std::shared_ptr<FuncSymbol> method);
    const std::vector<std::shared_ptr<FuncSymbol>>& getMethods() const;
    
    // 关联函数管理（不带 self 参数）
    void addAssociatedFunction(std::shared_ptr<FuncSymbol> function);
    const std::vector<std::shared_ptr<FuncSymbol>>& getAssociatedFunctions() const;
    
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

class FuncSymbol : public Symbol {
private:
    bool is_const;
    std::string identifier;
    std::vector<std::shared_ptr<VariableSymbol>> func_params;
    SymbolType return_type;
public:
    FuncSymbol(const std::string& identifier, const SymbolType& return_type, bool is_const = false);
    std::string getIdentifier() const;
    bool isConst() const;
    void addParameter(std::shared_ptr<VariableSymbol> param);
    const std::vector<std::shared_ptr<VariableSymbol>>& getParameters() const;
    SymbolType getReturnType() const;
};

class TraitSymbol : public Symbol {
private:
    std::string identifier;
    std::vector<std::shared_ptr<ConstSymbol>> const_symbols;
    std::vector<std::shared_ptr<FuncSymbol>> associated_funcs;
public:
    TraitSymbol(const std::string& identifier);
    std::string getIdentifier() const;
    void addConstSymbol(std::shared_ptr<ConstSymbol> const_symbol);
    void addAssociatedFunction(std::shared_ptr<FuncSymbol> func);
    const std::vector<std::shared_ptr<ConstSymbol>>& getConstSymbols() const;
    const std::vector<std::shared_ptr<FuncSymbol>>& getAssociatedFunctions() const;
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
