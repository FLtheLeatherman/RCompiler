#pragma once

#include "parser/visitor.hpp"
#include "parser/astnode.hpp"
#include "scope.hpp"
#include "symbol.hpp"
#include <memory>
#include <unordered_map>
#include <stdexcept>

/**
 * StructChecker 类负责进行 struct 相关字段、方法和函数的检查
 * 
 * 主要功能：
 * 1. 在每个 scope 中构建 struct 的字段、方法和函数信息
 * 2. 检查类型是否存在/可见
 * 3. 处理 impl 块，将关联项添加到 struct 中
 * 4. 检查 trait 实现的完整性
 * 5. 对于未定义的名称抛出 "Undefined Name" 异常
 */
class StructChecker : public ASTVisitor {
private:
    std::shared_ptr<Scope> current_scope;
    std::shared_ptr<Scope> root_scope;
    
    // 辅助方法
    std::string typeToString(std::shared_ptr<Type> type);
    bool checkTypeExists(const std::string& type_name);
    std::shared_ptr<StructSymbol> findStructSymbol(const std::string& struct_name);
    std::shared_ptr<TraitSymbol> findTraitSymbol(const std::string& trait_name);
    bool hasSelfParameter(std::shared_ptr<Function> func);
    void checkTraitImplementation(std::shared_ptr<TraitSymbol> trait_symbol, 
                                 std::shared_ptr<StructSymbol> struct_symbol,
                                 const std::vector<std::shared_ptr<AssociatedItem>>& impl_items);
    
public:
    StructChecker(std::shared_ptr<Scope> root_scope);
    ~StructChecker() = default;
    
    // 顶层节点
    void visit(Crate& node) override;
    void visit(Item& node) override;
    
    // 声明类节点 (Items)
    void visit(Struct& node) override;
    void visit(StructStruct& node) override;
    void visit(StructFields& node) override;
    void visit(StructField& node) override;
    
    // 实现相关节点
    void visit(Implementation& node) override;
    void visit(InherentImpl& node) override;
    void visit(TraitImpl& node) override;
    void visit(AssociatedItem& node) override;
    
    // 函数相关节点
    void visit(Function& node) override;
    void visit(FunctionParameters& node) override;
    void visit(SelfParam& node) override;
    void visit(ShorthandSelf& node) override;
    void visit(TypedSelf& node) override;
    void visit(FunctionParam& node) override;
    void visit(FunctionReturnType& node) override;
    
    // 常量项
    void visit(ConstantItem& node) override;
    
    // 特征相关
    void visit(Trait& node) override;
    
    // 类型相关节点
    void visit(Type& node) override;
    void visit(PathInExpression& node) override;
    void visit(PathIdentSegment& node) override;
    void visit(ReferenceType& node) override;
    void visit(ArrayType& node) override;
    void visit(UnitType& node) override;
    
    // 需要遍历的其他节点（保持 AST 遍历的完整性）
    void visit(Enumeration& node) override;
    void visit(EnumVariants& node) override;
    void visit(EnumVariant& node) override;
    void visit(Statement& node) override;
    void visit(LetStatement& node) override;
    void visit(ExpressionStatement& node) override;
    void visit(Statements& node) override;
    void visit(Expression& node) override;
    void visit(ExpressionWithoutBlock& node) override;
    void visit(ExpressionWithBlock& node) override;
    void visit(BlockExpression& node) override;
    void visit(IfExpression& node) override;
    void visit(LoopExpression& node) override;
    void visit(InfiniteLoopExpression& node) override;
    void visit(PredicateLoopExpression& node) override;
    void visit(PatternNoTopAlt& node) override;
    void visit(IdentifierPattern& node) override;
    void visit(CallExpression& node) override;
    void visit(MethodCallExpression& node) override;
    void visit(FieldExpression& node) override;
    void visit(IndexExpression& node) override;
    void visit(StructExpression& node) override;
    void visit(ArrayExpression& node) override;
    void visit(GroupedExpression& node) override;
    void visit(UnaryExpression& node) override;
    void visit(BorrowExpression& node) override;
    void visit(DereferenceExpression& node) override;
    void visit(BinaryExpression& node) override;
    void visit(AssignmentExpression& node) override;
    void visit(CompoundAssignmentExpression& node) override;
    void visit(TypeCastExpression& node) override;
    void visit(PathExpression& node) override;
    void visit(Condition& node) override;
    void visit(ArrayElements& node) override;
    void visit(StructExprFields& node) override;
    void visit(StructExprField& node) override;
    void visit(CallParams& node) override;
    void visit(ReferencePattern& node) override;
    void visit(CharLiteral& node) override;
    void visit(StringLiteral& node) override;
    void visit(RawStringLiteral& node) override;
    void visit(CStringLiteral& node) override;
    void visit(RawCStringLiteral& node) override;
    void visit(IntegerLiteral& node) override;
    void visit(BoolLiteral& node) override;
    void visit(BreakExpression& node) override;
    void visit(ContinueExpression& node) override;
    void visit(ReturnExpression& node) override;
};