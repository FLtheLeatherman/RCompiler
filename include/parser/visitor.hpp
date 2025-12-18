#pragma once

#include "parser/utils.hpp"

class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;
    
    // 基础访问方法
    virtual void visit(ASTNode&) {}
    
    // 顶层节点
    virtual void visit(Crate&) {}
    virtual void visit(Item&) {}
    
    // 声明类节点 (Items)
    virtual void visit(Function&) {}
    virtual void visit(Struct&) {}
    virtual void visit(Enumeration&) {}
    virtual void visit(ConstantItem&) {}
    virtual void visit(Trait&) {}
    virtual void visit(Implementation&) {}
    
    // 函数相关节点
    virtual void visit(FunctionParameters&) {}
    virtual void visit(SelfParam&) {}
    virtual void visit(ShorthandSelf&) {}
    virtual void visit(TypedSelf&) {}
    virtual void visit(FunctionParam&) {}
    virtual void visit(FunctionReturnType&) {}
    
    // 结构体相关节点
    virtual void visit(StructStruct&) {}
    virtual void visit(StructFields&) {}
    virtual void visit(StructField&) {}
    
    // 枚举相关节点
    virtual void visit(EnumVariants&) {}
    virtual void visit(EnumVariant&) {}
    
    // 实现相关节点
    virtual void visit(AssociatedItem&) {}
    virtual void visit(InherentImpl&) {}
    virtual void visit(TraitImpl&) {}
    
    // 语句类节点
    virtual void visit(Statement&) {}
    virtual void visit(LetStatement&) {}
    virtual void visit(ExpressionStatement&) {}
    virtual void visit(Statements&) {}
    
    // 表达式类节点
    virtual void visit(Expression&) {}
    virtual void visit(ExpressionWithoutBlock&) {}
    virtual void visit(ExpressionWithBlock&) {}
    
    // 字面量表达式
    virtual void visit(CharLiteral&) {}
    virtual void visit(StringLiteral&) {}
    virtual void visit(RawStringLiteral&) {}
    virtual void visit(CStringLiteral&) {}
    virtual void visit(RawCStringLiteral&) {}
    virtual void visit(IntegerLiteral&) {}
    virtual void visit(BoolLiteral&) {}
    
    // 路径和访问表达式
    virtual void visit(PathExpression&) {}
    virtual void visit(FieldExpression&) {}
    
    // 运算符表达式
    virtual void visit(UnaryExpression&) {}
    virtual void visit(BorrowExpression&) {}
    virtual void visit(DereferenceExpression&) {}
    virtual void visit(BinaryExpression&) {}
    virtual void visit(AssignmentExpression&) {}
    virtual void visit(CompoundAssignmentExpression&) {}
    virtual void visit(TypeCastExpression&) {}
    
    // 调用和索引表达式
    virtual void visit(CallExpression&) {}
    virtual void visit(MethodCallExpression&) {}
    virtual void visit(IndexExpression&) {}
    
    // 结构体和数组表达式
    virtual void visit(StructExpression&) {}
    virtual void visit(ArrayExpression&) {}
    virtual void visit(GroupedExpression&) {}
    
    // 控制流表达式
    virtual void visit(BlockExpression&) {}
    virtual void visit(IfExpression&) {}
    virtual void visit(LoopExpression&) {}
    virtual void visit(InfiniteLoopExpression&) {}
    virtual void visit(PredicateLoopExpression&) {}
    virtual void visit(BreakExpression&) {}
    virtual void visit(ContinueExpression&) {}
    virtual void visit(ReturnExpression&) {}
    
    // 辅助表达式节点
    virtual void visit(Condition&) {}
    virtual void visit(ArrayElements&) {}
    virtual void visit(StructExprFields&) {}
    virtual void visit(StructExprField&) {}
    virtual void visit(CallParams&) {}
    
    // 模式类节点
    virtual void visit(PatternNoTopAlt&) {}
    virtual void visit(IdentifierPattern&) {}
    virtual void visit(ReferencePattern&) {}
    
    // 类型类节点
    virtual void visit(Type&) {}
    virtual void visit(ReferenceType&) {}
    virtual void visit(ArrayType&) {}
    virtual void visit(UnitType&) {}
    
    // 路径类节点
    virtual void visit(PathInExpression&) {}
    virtual void visit(PathIdentSegment&) {}
};