#pragma once

#include <string>
#include <cstring>
#include <vector>
#include <memory>

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
    // ConstValue value;
    // not yet implemented
public:
    ConstSymbol(const std::string& identifier, const SymbolType& type);
    std::string getIdentifier() const;
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
public:
    StructSymbol(const std::string& identifier, const SymbolType& type);
    std::string getIdentifier() const;
    void addField(std::shared_ptr<VariableSymbol> field);
    const std::vector<std::shared_ptr<VariableSymbol>>& getFields() const;
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
