#pragma once

#include "parser/visitor.hpp"
#include "parser/astnode.hpp"
#include "scope.hpp"
#include "symbol.hpp"
#include "utils.hpp"

// 前向声明
class Type;
class ReferenceType;
class ArrayType;
class UnitType;
class PathIdentSegment;
#include <memory>
#include <unordered_map>
#include <stdexcept>

class StructChecker : public ASTVisitor {
private:
    std::shared_ptr<Scope> current_scope;
    std::shared_ptr<Scope> root_scope;
    const std::vector<std::string> builtin_types = {"bool", "i32", "u32", "usize", "char", "str", "()"};
    bool checkTypeExists(SymbolType);
    void handleInherentImpl();
    void handleTraitImpl(std::string);
public:
    StructChecker(std::shared_ptr<Scope> root_scope);
    ~StructChecker() = default;
    void visit(Crate& node) override;
    void visit(Item& node) override;
    void visit(Function& node) override;
    void visit(Struct& node) override;
    void visit(Enumeration& node) override;
    void visit(ConstantItem& node) override;
    void visit(Trait& node) override;
    void visit(Implementation& node) override;
    void visit(InherentImpl& node) override;
    void visit(TraitImpl& node) override;
    void visit(AssociatedItem& node) override;
};