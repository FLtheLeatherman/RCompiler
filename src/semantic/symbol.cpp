#include "semantic/symbol.hpp"
#include "semantic/const_value.hpp"

// Symbol 基类实现
Symbol::Symbol(const SymbolType& type) : type(type) {}

void Symbol::setType(SymbolType type) {
    this->type = type;
}

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
VariableSymbol::VariableSymbol(const std::string identifier, const SymbolType type, bool is_ref, int is_mut)
    : Symbol(type), identifier(identifier), is_ref(is_ref), is_mut(is_mut) {}

std::string VariableSymbol::getIdentifier() const {
    return identifier;
}

bool VariableSymbol::isRef() const {
    return is_ref;
}

int VariableSymbol::getMut() const {
    return is_mut;
}

// StructSymbol 类实现
StructSymbol::StructSymbol(const std::string& identifier, const SymbolType& type)
    : Symbol(type), identifier(identifier) {}

std::string StructSymbol::getIdentifier() const {
    return identifier;
}

// 字段管理
void StructSymbol::addField(std::shared_ptr<VariableSymbol> field) {
    // std::cout << "add field " << field->getIdentifier() << std::endl;
    vars[field->getIdentifier()] = field;
}

void StructSymbol::eraseField(std::string str) {
    vars.erase(vars.find(str));
}

bool StructSymbol::hasField(const std::string& name) const {
    return vars.find(name) != vars.end();
}

std::shared_ptr<VariableSymbol> StructSymbol::getField(const std::string& name) const {
    auto it = vars.find(name);
    return (it != vars.end()) ? it->second : nullptr;
}

std::vector<std::shared_ptr<VariableSymbol>> StructSymbol::getFields() const {
    std::vector<std::shared_ptr<VariableSymbol>> result;
    result.reserve(vars.size());
    for (const auto& pair : vars) {
        result.push_back(pair.second);
    }
    return result;
}

int StructSymbol::getFieldSize() const {
    return vars.size();
}

// 关联常量管理
void StructSymbol::addAssociatedConst(std::shared_ptr<ConstSymbol> const_symbol) {
    associated_consts[const_symbol->getIdentifier()] = const_symbol;
}

bool StructSymbol::hasAssociatedConst(const std::string& name) const {
    return associated_consts.find(name) != associated_consts.end();
}

std::shared_ptr<ConstSymbol> StructSymbol::getAssociatedConst(const std::string& name) const {
    auto it = associated_consts.find(name);
    return (it != associated_consts.end()) ? it->second : nullptr;
}

std::vector<std::shared_ptr<ConstSymbol>> StructSymbol::getAssociatedConsts() const {
    std::vector<std::shared_ptr<ConstSymbol>> result;
    result.reserve(associated_consts.size());
    for (const auto& pair : associated_consts) {
        result.push_back(pair.second);
    }
    return result;
}

// 方法管理（带 self 参数）
void StructSymbol::addMethod(std::shared_ptr<FuncSymbol> method) {
    methods[method->getIdentifier()] = method;
}

bool StructSymbol::hasMethod(const std::string& name) const {
    return methods.find(name) != methods.end();
}

std::shared_ptr<FuncSymbol> StructSymbol::getMethod(const std::string& name) const {
    auto it = methods.find(name);
    return (it != methods.end()) ? it->second : nullptr;
}

std::vector<std::shared_ptr<FuncSymbol>> StructSymbol::getMethods() const {
    std::vector<std::shared_ptr<FuncSymbol>> result;
    result.reserve(methods.size());
    for (const auto& pair : methods) {
        result.push_back(pair.second);
    }
    return result;
}

// 关联函数管理（不带 self 参数）
void StructSymbol::addAssociatedFunction(std::shared_ptr<FuncSymbol> function) {
    functions[function->getIdentifier()] = function;
}

bool StructSymbol::hasAssociatedFunction(const std::string& name) const {
    return functions.find(name) != functions.end();
}

std::shared_ptr<FuncSymbol> StructSymbol::getAssociatedFunction(const std::string& name) const {
    auto it = functions.find(name);
    return (it != functions.end()) ? it->second : nullptr;
}

std::vector<std::shared_ptr<FuncSymbol>> StructSymbol::getAssociatedFunctions() const {
    std::vector<std::shared_ptr<FuncSymbol>> result;
    result.reserve(functions.size());
    for (const auto& pair : functions) {
        result.push_back(pair.second);
    }
    return result;
}

// 获取所有关联项（方法 + 关联函数）
std::vector<std::shared_ptr<FuncSymbol>> StructSymbol::getAllAssociatedFunctions() const {
    std::vector<std::shared_ptr<FuncSymbol>> all_funcs;
    all_funcs.reserve(methods.size() + functions.size());
    for (const auto& pair : methods) {
        all_funcs.push_back(pair.second);
    }
    for (const auto& pair : functions) {
        all_funcs.push_back(pair.second);
    }
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
FuncSymbol::FuncSymbol(const std::string& identifier, const SymbolType& return_type, bool is_const, MethodType method_type)
    : Symbol("function"), identifier(identifier), return_type(return_type), is_const(is_const), method_type(method_type) {}

std::string FuncSymbol::getIdentifier() const {
    return identifier;
}

bool FuncSymbol::isConst() const {
    return is_const;
}

bool FuncSymbol::isMethod() const {
    return method_type != MethodType::NOT_METHOD;
}

MethodType FuncSymbol::getMethodType() const {
    return method_type;
}

std::string FuncSymbol::getMethodTypeString() const {
    switch (method_type) {
        case MethodType::NOT_METHOD:
            return "function";
        case MethodType::SELF_VALUE:
            return "self";
        case MethodType::SELF_REF:
            return "&self";
        case MethodType::SELF_MUT_REF:
            return "&mut self";
        case MethodType::SELF_MUT_VALUE:
            return "mut self";
        default:
            return "unknown";
    }
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

void FuncSymbol::setReturnType(std::string return_type) {
    this->return_type = return_type;
}

// TraitSymbol 类实现
TraitSymbol::TraitSymbol(const std::string& identifier)
    : Symbol("trait"), identifier(identifier) {}

std::string TraitSymbol::getIdentifier() const {
    return identifier;
}

// 关联常量管理
void TraitSymbol::addConstSymbol(std::shared_ptr<ConstSymbol> const_symbol) {
    const_symbols[const_symbol->getIdentifier()] = const_symbol;
}

bool TraitSymbol::hasConstSymbol(const std::string& name) const {
    return const_symbols.find(name) != const_symbols.end();
}

std::shared_ptr<ConstSymbol> TraitSymbol::getConstSymbol(const std::string& name) const {
    auto it = const_symbols.find(name);
    return (it != const_symbols.end()) ? it->second : nullptr;
}

std::vector<std::shared_ptr<ConstSymbol>> TraitSymbol::getConstSymbols() const {
    std::vector<std::shared_ptr<ConstSymbol>> result;
    result.reserve(const_symbols.size());
    for (const auto& pair : const_symbols) {
        result.push_back(pair.second);
    }
    return result;
}

// 方法管理（带 self 参数）
void TraitSymbol::addMethod(std::shared_ptr<FuncSymbol> method) {
    methods[method->getIdentifier()] = method;
}

bool TraitSymbol::hasMethod(const std::string& name) const {
    return methods.find(name) != methods.end();
}

std::shared_ptr<FuncSymbol> TraitSymbol::getMethod(const std::string& name) const {
    auto it = methods.find(name);
    return (it != methods.end()) ? it->second : nullptr;
}

std::vector<std::shared_ptr<FuncSymbol>> TraitSymbol::getMethods() const {
    std::vector<std::shared_ptr<FuncSymbol>> result;
    result.reserve(methods.size());
    for (const auto& pair : methods) {
        result.push_back(pair.second);
    }
    return result;
}

// 关联函数管理（不带 self 参数）
void TraitSymbol::addAssociatedFunction(std::shared_ptr<FuncSymbol> function) {
    functions[function->getIdentifier()] = function;
}

bool TraitSymbol::hasAssociatedFunction(const std::string& name) const {
    return functions.find(name) != functions.end();
}

std::shared_ptr<FuncSymbol> TraitSymbol::getAssociatedFunction(const std::string& name) const {
    auto it = functions.find(name);
    return (it != functions.end()) ? it->second : nullptr;
}

std::vector<std::shared_ptr<FuncSymbol>> TraitSymbol::getAssociatedFunctions() const {
    std::vector<std::shared_ptr<FuncSymbol>> result;
    result.reserve(functions.size());
    for (const auto& pair : functions) {
        result.push_back(pair.second);
    }
    return result;
}

// 获取所有关联项（方法 + 关联函数）
std::vector<std::shared_ptr<FuncSymbol>> TraitSymbol::getAllAssociatedFunctions() const {
    std::vector<std::shared_ptr<FuncSymbol>> all_funcs;
    all_funcs.reserve(methods.size() + functions.size());
    for (const auto& pair : methods) {
        all_funcs.push_back(pair.second);
    }
    for (const auto& pair : functions) {
        all_funcs.push_back(pair.second);
    }
    return all_funcs;
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