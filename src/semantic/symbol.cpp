#include "semantic/symbol.hpp"

// Symbol 基类实现
Symbol::Symbol(const SymbolType& type) : type(type) {}

SymbolType Symbol::getType() {
    return type;
}

// ConstSymbol 类实现
ConstSymbol::ConstSymbol(const std::string& identifier, const SymbolType& type)
    : Symbol(type), identifier(identifier) {}

std::string ConstSymbol::getIdentifier() const {
    return identifier;
}

// VariableSymbol 类实现
VariableSymbol::VariableSymbol(const std::string& identifier, const SymbolType& type, bool is_ref, bool is_mut)
    : Symbol(type), identifier(identifier), is_ref(is_ref), is_mut(is_mut) {}

std::string VariableSymbol::getIdentifier() const {
    return identifier;
}

bool VariableSymbol::isRef() const {
    return is_ref;
}

bool VariableSymbol::isMut() const {
    return is_mut;
}

// StructSymbol 类实现
StructSymbol::StructSymbol(const std::string& identifier, const SymbolType& type)
    : Symbol(type), identifier(identifier) {}

std::string StructSymbol::getIdentifier() const {
    return identifier;
}

void StructSymbol::addField(std::shared_ptr<VariableSymbol> field) {
    vars.push_back(field);
}

const std::vector<std::shared_ptr<VariableSymbol>>& StructSymbol::getFields() const {
    return vars;
}

// EnumVar 类实现
EnumVar::EnumVar(const std::string& identifier) : identifier(identifier) {}

std::string EnumVar::getIdentifier() const {
    return identifier;
}

// EnumSymbol 类实现
EnumSymbol::EnumSymbol(const std::string& identifier, const SymbolType& type)
    : Symbol(type), identifier(identifier) {}

std::string EnumSymbol::getIdentifier() const {
    return identifier;
}

void EnumSymbol::addVariant(std::shared_ptr<EnumVar> variant) {
    vars.push_back(variant);
}

const std::vector<std::shared_ptr<EnumVar>>& EnumSymbol::getVariants() const {
    return vars;
}

// FuncSymbol 类实现
FuncSymbol::FuncSymbol(const std::string& identifier, const SymbolType& return_type, bool is_const)
    : Symbol("function"), identifier(identifier), return_type(return_type), is_const(is_const) {}

std::string FuncSymbol::getIdentifier() const {
    return identifier;
}

bool FuncSymbol::isConst() const {
    return is_const;
}

void FuncSymbol::addParameter(std::shared_ptr<VariableSymbol> param) {
    func_params.push_back(param);
}

const std::vector<std::shared_ptr<VariableSymbol>>& FuncSymbol::getParameters() const {
    return func_params;
}

SymbolType FuncSymbol::getReturnType() const {
    return return_type;
}

// TraitSymbol 类实现
TraitSymbol::TraitSymbol(const std::string& identifier)
    : Symbol("trait"), identifier(identifier) {}

std::string TraitSymbol::getIdentifier() const {
    return identifier;
}

void TraitSymbol::addConstSymbol(std::shared_ptr<ConstSymbol> const_symbol) {
    const_symbols.push_back(const_symbol);
}

void TraitSymbol::addAssociatedFunction(std::shared_ptr<FuncSymbol> func) {
    associated_funcs.push_back(func);
}

const std::vector<std::shared_ptr<ConstSymbol>>& TraitSymbol::getConstSymbols() const {
    return const_symbols;
}

const std::vector<std::shared_ptr<FuncSymbol>>& TraitSymbol::getAssociatedFunctions() const {
    return associated_funcs;
}