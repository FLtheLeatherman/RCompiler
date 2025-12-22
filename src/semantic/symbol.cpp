#include "semantic/symbol.hpp"
#include "semantic/const_value.hpp"

// Symbol 基类实现
Symbol::Symbol(const SymbolType& type) : type(type) {}

SymbolType Symbol::getType() {
    return type;
}

// ConstSymbol 类实现
ConstSymbol::ConstSymbol(const std::string& identifier, const SymbolType& type)
    : Symbol(type), identifier(identifier), value(nullptr) {}

ConstSymbol::ConstSymbol(const std::string& identifier, const SymbolType& type, std::shared_ptr<ConstValue> value)
    : Symbol(type), identifier(identifier), value(value) {}

std::string ConstSymbol::getIdentifier() const {
    return identifier;
}

std::shared_ptr<ConstValue> ConstSymbol::getValue() const {
    return value;
}

void ConstSymbol::setValue(std::shared_ptr<ConstValue> value) {
    this->value = value;
}

bool ConstSymbol::hasValue() const {
    return value != nullptr;
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

// 关联常量管理
void StructSymbol::addAssociatedConst(std::shared_ptr<ConstSymbol> const_symbol) {
    associated_consts.push_back(const_symbol);
}

const std::vector<std::shared_ptr<ConstSymbol>>& StructSymbol::getAssociatedConsts() const {
    return associated_consts;
}

// 方法管理（带 self 参数）
void StructSymbol::addMethod(std::shared_ptr<FuncSymbol> method) {
    methods.push_back(method);
}

const std::vector<std::shared_ptr<FuncSymbol>>& StructSymbol::getMethods() const {
    return methods;
}

// 关联函数管理（不带 self 参数）
void StructSymbol::addAssociatedFunction(std::shared_ptr<FuncSymbol> function) {
    functions.push_back(function);
}

const std::vector<std::shared_ptr<FuncSymbol>>& StructSymbol::getAssociatedFunctions() const {
    return functions;
}

// 获取所有关联项（方法 + 关联函数）
std::vector<std::shared_ptr<FuncSymbol>> StructSymbol::getAllAssociatedFunctions() const {
    std::vector<std::shared_ptr<FuncSymbol>> all_funcs;
    all_funcs.insert(all_funcs.end(), methods.begin(), methods.end());
    all_funcs.insert(all_funcs.end(), functions.begin(), functions.end());
    return all_funcs;
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

// ArraySymbol 类实现
ArraySymbol::ArraySymbol(const std::string& identifier, const std::string& element_type)
    : Symbol("array"), identifier(identifier), element_type(element_type), length(nullptr) {}

ArraySymbol::ArraySymbol(const std::string& identifier, const std::string& element_type, std::shared_ptr<ConstValue> length)
    : Symbol("array"), identifier(identifier), element_type(element_type), length(length) {}

std::string ArraySymbol::getIdentifier() const {
    return identifier;
}

std::string ArraySymbol::getElementType() const {
    return element_type;
}

void ArraySymbol::setElementType(const std::string& element_type) {
    this->element_type = element_type;
}

std::shared_ptr<ConstValue> ArraySymbol::getLength() const {
    return length;
}

void ArraySymbol::setLength(std::shared_ptr<ConstValue> length) {
    this->length = length;
}

bool ArraySymbol::hasLength() const {
    return length != nullptr;
}