#include "semantic/struct_checker.hpp"
#include "semantic/const_value.hpp"
#include <iostream>
#include <stdexcept>

StructChecker::StructChecker(std::shared_ptr<Scope> root_scope) {
    this->current_scope = this->root_scope = root_scope;
}

bool StructChecker::checkTypeExists(SymbolType type) {
    if (type[0] == '[') type = type.substr(1, type.length() - 2);
    if (type[0] == '&') type = type.substr(1);
    if (type.substr(0, 3) == "mut") type = type.substr(3);
    // builtin types
    for (auto builtin_type: builtin_types) {
        if (type == builtin_type) {
            return true;
        }
    }
    if (current_scope->structSymbolExists(type)) return true;
    if (current_scope->enumSymbolExists(type)) return true;
    return false;
}

void StructChecker::handleInherentImpl() {
    std::shared_ptr<StructSymbol> struct_symbol = current_scope->findStructSymbol(current_scope->getSelfType());
    if (struct_symbol == nullptr) {
        throw std::runtime_error("Undefined Name");
    }
    for (const auto& [name, const_symbol]: current_scope->getConstSymbols()) {
        struct_symbol->addAssociatedConst(const_symbol);
    }
    for (const auto& [name, func_symbol]: current_scope->getFuncSymbols()) {
        if (func_symbol->isMethod()) {
            struct_symbol->addMethod(func_symbol);
        } else {
            struct_symbol->addAssociatedFunction(func_symbol);
        }
    }
}

void StructChecker::handleTraitImpl(std::string identifier) {
    std::shared_ptr<StructSymbol> struct_symbol = current_scope->findStructSymbol(current_scope->getSelfType());
    std::shared_ptr<TraitSymbol> trait_symbol = current_scope->findTraitSymbol(identifier);
    if (struct_symbol == nullptr || trait_symbol == nullptr) {
        throw std::runtime_error("Undefined Name");
    }

    auto const_symbols = trait_symbol->getConstSymbols();
    if (const_symbols.size() != trait_symbol->getConstSymbols().size()) {
        throw std::runtime_error("Undefined Name");
    }
    for (const auto& const_symbol: const_symbols) {
        if (!current_scope->constSymbolExists(const_symbol->getIdentifier())) {
            throw std::runtime_error("Undefined Name");
        }
    }
    auto func_symbols = trait_symbol->getAssociatedFunctions();
    if (func_symbols.size() != current_scope->getFuncSymbolCount()) {
        throw std::runtime_error("Undefined Name");
    }
    for (const auto& func_symbol: func_symbols) {
        auto impl_func = current_scope->getFuncSymbol(func_symbol->getIdentifier());
        if (impl_func->getMethodType() != func_symbol->getMethodType() || impl_func->isConst() != func_symbol->isConst()) {
            throw std::runtime_error("Undefined Name");
        }
        if (func_symbol->getReturnType() != impl_func->getReturnType()) {
            throw std::runtime_error("Undefined Name");
        }
        auto trait_func_param = func_symbol->getParameters();
        auto impl_func_param = impl_func->getParameters();
        if (trait_func_param.size() != impl_func_param.size()) {
            throw std::runtime_error("Undefined Name");
        }
        for (size_t _ = 0; _ < impl_func_param.size(); ++_) {
            if (impl_func_param[_]->getType() != trait_func_param[_]->getType()) {
                throw std::runtime_error("Undefined Name");
            }
        }
    }

    for (const auto& [name, const_symbol]: current_scope->getConstSymbols()) {
        struct_symbol->addAssociatedConst(const_symbol);
    }
    for (const auto& [name, func_symbol]: current_scope->getFuncSymbols()) {
        if (func_symbol->isMethod()) {
            struct_symbol->addMethod(func_symbol);
        } else {
            struct_symbol->addAssociatedFunction(func_symbol);
        }
    }
}

void StructChecker::visit(Crate& node) {
    for (auto item: node.items) {
        item->accept(this);
    }
}

void StructChecker::visit(Item& node) {
    if (node.item) {
        node.item->accept(this);
    }
}

void StructChecker::visit(Function& node) {
    std::cout << "visit function: " << node.identifier << std::endl;

    // std::cout << "GOOD" << std::endl;

    auto function_symbol = current_scope->getFuncSymbol(node.identifier);

    // std::cout << "GOOD" << std::endl;

    if (!checkTypeExists(function_symbol->getReturnType())) {
        throw std::runtime_error("Undefined Name");
    }
    auto function_params = function_symbol->getParameters();
    for (auto function_param: function_params) {
        if (!checkTypeExists(function_param->getType())) {
            throw std::runtime_error("Undefined Name");
        }
    }

    // std::cout << "GOOD" << std::endl;
    
    auto prev_scope = current_scope;
    current_scope = current_scope->getChild();

    // std::cout << "GOOD" << std::endl;

    if (node.block_expression && current_scope) {
        node.block_expression->accept(this);
    }
    current_scope = prev_scope;
    current_scope->nextChild();
}

void StructChecker::visit(Struct& node) {
    std::cout << "visit struct: " << std::endl;
    // struct 不会创建新的 scope，直接访问其内容
    if (node.struct_struct) {
        node.struct_struct->accept(this);
    }
}

void StructChecker::visit(Enumeration& node) {
    std::cout << "visit enum: " << std::endl;
    // 检查枚举符号是否存在
    auto enum_symbol = current_scope->getEnumSymbol(node.identifier);
    if (!enum_symbol) {
        throw std::runtime_error("Undefined Name");
    }
    
    // 枚举不会创建新的 scope，直接访问其内容
    if (node.enum_variants) {
        node.enum_variants->accept(this);
    }
}

void StructChecker::visit(ConstantItem& node) {
    std::cout << "visit constant item: " << node.identifier << std::endl;
    // 从当前作用域获取常量符号
    auto const_symbol = current_scope->getConstSymbol(node.identifier);
    if (!const_symbol) {
        throw std::runtime_error("Undefined Name");
    }
    
    // 检查常量类型是否存在
    if (!checkTypeExists(const_symbol->getType())) {
        throw std::runtime_error("Undefined Name");
    }
}

void StructChecker::visit(Trait& node) {
    std::cout << "visit trait: " << node.identifier << std::endl;
    // 检查 trait 符号是否存在
    auto trait_symbol = current_scope->getTraitSymbol(node.identifier);
    if (!trait_symbol) {
        throw std::runtime_error("Undefined Name");
    }
    
    // trait 会创建新的 scope，需要进入
    auto prev_scope = current_scope;
    current_scope = current_scope->getChild();
    
    // 处理关联项
    for (auto& item : node.associated_item) {
        if (item) {
            item->accept(this);
        }
    }
    
    current_scope = prev_scope;
    current_scope->nextChild();
}

void StructChecker::visit(Implementation& node) {
    std::cout << "visit impl: " << std::endl;
    if (node.impl) {
        node.impl->accept(this);
    }
}

void StructChecker::visit(InherentImpl& node) {
    std::cout << "visit inherent impl: " << std::endl;
    // 进入 impl 作用域
    auto prev_scope = current_scope;
    current_scope = current_scope->getChild();
    
    // 处理关联项
    for (auto& item : node.associated_item) {
        if (item) {
            item->accept(this);
        }
    }
    
    // 调用辅助函数处理 inherent impl
    handleInherentImpl();
    
    current_scope = prev_scope;
    current_scope->nextChild();
}

void StructChecker::visit(TraitImpl& node) {
    std::cout << "visit trait impl: " << node.identifier << std::endl;
    // 进入 impl 作用域
    auto prev_scope = current_scope;
    current_scope = current_scope->getChild();
    
    // 处理关联项
    for (auto& item : node.associated_item) {
        if (item) {
            item->accept(this);
        }
    }
    
    // 调用辅助函数处理 trait impl
    handleTraitImpl(node.identifier);
    
    current_scope = prev_scope;
    current_scope->nextChild();
}

void StructChecker::visit(AssociatedItem& node) {
    std::cout << "visit associated item: " << std::endl;
    if (node.child) {
        node.child->accept(this);
    }
}
