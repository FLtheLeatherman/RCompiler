#pragma once

#include <string>
#include <cstring>
#include "parser/visitor.hpp"
#include "parser/astnode.hpp"
#include "semantic/const_value.hpp"
#include "semantic/scope.hpp"
#include "utils.hpp"

class ConstEvaluator : ASTVisitor {
private:
    std::shared_ptr<Scope> current_scope;
    std::shared_ptr<Scope> root_scope;
public:
    ConstEvaluator(std::shared_ptr<Scope> root_scope);
    ~ConstEvaluator() = default;
    
    void visit(Crate&) override;
    void visit(Item&) override;

    void visit(ConstantItem&) override;
    void visit(Function&) override;
    void visit(Struct&) override;
    void visit(Enumeration&) override;
    void visit(Trait&) override;
    void visit(Implementation&) override;

    void visit(StructStruct&) override;
    
    // 函数相关节点
    void visit(FunctionParameters&) override;
    void visit(SelfParam&) override;
    void visit(ShorthandSelf&) override;
    void visit(TypedSelf&) override;
    void visit(FunctionParam&) override;
    void visit(FunctionReturnType&) override;
    
    // 结构体相关节点
    void visit(StructFields&) override;
    void visit(StructField&) override;
    
    // 枚举相关节点
    void visit(EnumVariants&) override;
    void visit(EnumVariant&) override;
    
    // 实现相关节点
    void visit(AssociatedItem&) override;
    void visit(InherentImpl&) override;
    void visit(TraitImpl&) override;
    
    // 语句类节点
    void visit(Statement&) override;
    void visit(LetStatement&) override;
    void visit(ExpressionStatement&) override;
    void visit(Statements&) override;
    
    // 表达式类节点
    void visit(Expression&) override;
    void visit(ExpressionWithoutBlock&) override;
    void visit(ExpressionWithBlock&) override;
    
    // 字面量表达式
    void visit(CharLiteral&) override;
    void visit(StringLiteral&) override;
    void visit(RawStringLiteral&) override;
    void visit(CStringLiteral&) override;
    void visit(RawCStringLiteral&) override;
    void visit(IntegerLiteral&) override;
    void visit(BoolLiteral&) override;
    
    // 路径和访问表达式
    void visit(PathExpression&) override;
    void visit(FieldExpression&) override;
    
    // 运算符表达式
    void visit(UnaryExpression&) override;
    void visit(BorrowExpression&) override;
    void visit(DereferenceExpression&) override;
    void visit(BinaryExpression&) override;
    void visit(AssignmentExpression&) override;
    void visit(CompoundAssignmentExpression&) override;
    void visit(TypeCastExpression&) override;
    
    // 调用和索引表达式
    void visit(CallExpression&) override;
    void visit(MethodCallExpression&) override;
    void visit(IndexExpression&) override;
    
    // 结构体和数组表达式
    void visit(StructExpression&) override;
    void visit(ArrayExpression&) override;
    void visit(GroupedExpression&) override;
    
    // 控制流表达式
    void visit(BlockExpression&) override;
    void visit(IfExpression&) override;
    void visit(LoopExpression&) override;
    void visit(InfiniteLoopExpression&) override;
    void visit(PredicateLoopExpression&) override;
    void visit(BreakExpression&) override;
    void visit(ContinueExpression&) override;
    void visit(ReturnExpression&) override;
    
    // 辅助表达式节点
    void visit(Condition&) override;
    void visit(ArrayElements&) override;
    void visit(StructExprFields&) override;
    void visit(StructExprField&) override;
    void visit(CallParams&) override;
    
    // 模式类节点
    void visit(PatternNoTopAlt&) override;
    void visit(IdentifierPattern&) override;
    void visit(ReferencePattern&) override;
    
    // 类型类节点
    void visit(Type&) override;
    void visit(ReferenceType&) override;
    void visit(ArrayType&) override;
    void visit(UnitType&) override;
    
    // 路径类节点
    void visit(PathInExpression&) override;
    void visit(PathIdentSegment&) override;
};