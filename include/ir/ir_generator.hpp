#pragma once

#include "parser/astnode.hpp"
#include "parser/visitor.hpp"
#include "semantic/scope.hpp"
#include "ir_builder.hpp"
#include "type.hpp"
#include "value.hpp"

#include <memory>
#include <unordered_map>
#include <iostream>

class IRGenerator : public ASTVisitor {
private:
    std::ostream &os;
    Scope *current_scope;
    Scope *root_scope;
    myllvm::IRBuilder *builder;
    std::unordered_map<std::string, myllvm::Function*> functions;
    std::unordered_map<std::string, myllvm::GlobalVariable*> global_variables;
    std::vector<std::unordered_map<std::string, myllvm::LocalVariable*>> local_variables_stack; // 用于处理局部变量的作用域
    std::unordered_map<std::string, uint32_t> var_counter; // 用于生成唯一的局部变量名
    std::unordered_map<std::string, myllvm::Type*> context; // 类型缓存，避免重复创建相同的类型
    std::string current_basic_block; // 当前所在的基本块标签
    bool current_function_has_return; // 当前函数是否已经有 return 语句

    myllvm::Type* getLLVMType(std::string);
    myllvm::Value* getVarValue(std::string);
public:
    IRGenerator(std::ostream &os, Scope *root_scope, myllvm::IRBuilder *builder);
    ~IRGenerator() = default;

    // 顶层节点
    void visit(Crate& node) override;
    void visit(Item& node) override;

    // 声明类节点 (Items)
    void visit(Function& node) override;
    void visit(Struct& node) override;
    void visit(Enumeration& node) override;
    void visit(ConstantItem& node) override;
    void visit(Trait& node) override;
    void visit(Implementation& node) override;

    // 函数相关节点
    void visit(FunctionParameters& node) override;
    void visit(FunctionParam& node) override;
    void visit(FunctionReturnType& node) override;

    // 结构体相关节点
    void visit(StructStruct& node) override;
    void visit(StructFields& node) override;
    void visit(StructField& node) override;

    // 枚举相关节点
    void visit(EnumVariants& node) override;
    void visit(EnumVariant& node) override;

    // 实现相关节点
    void visit(AssociatedItem& node) override;
    void visit(InherentImpl& node) override;
    void visit(TraitImpl& node) override;

    // 语句类节点
    void visit(Statement& node) override;
    void visit(LetStatement& node) override;
    void visit(ExpressionStatement& node) override;
    void visit(Statements& node) override;

    // 表达式类节点
    void visit(Expression& node) override;
    void visit(ExpressionWithoutBlock& node) override;
    void visit(ExpressionWithBlock& node) override;

    // 控制流表达式
    void visit(BlockExpression& node) override;
    void visit(IfExpression& node) override;
    void visit(LoopExpression& node) override;
    void visit(InfiniteLoopExpression& node) override;
    void visit(PredicateLoopExpression& node) override;

    // 模式类节点
    void visit(PatternNoTopAlt& node) override;
    void visit(IdentifierPattern& node) override;

    // 类型类节点
    void visit(Type& node) override;
    void visit(PathInExpression& node) override;
    void visit(PathIdentSegment& node) override;

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
    void visit(ReferenceType& node) override;
    void visit(ArrayType& node) override;
    void visit(UnitType& node) override;
    void visit(ReferencePattern& node) override;

    // 字面量表达式
    void visit(CharLiteral& node) override;
    void visit(StringLiteral& node) override;
    void visit(RawStringLiteral& node) override;
    void visit(CStringLiteral& node) override;
    void visit(RawCStringLiteral& node) override;
    void visit(IntegerLiteral& node) override;
    void visit(BoolLiteral& node) override;

    // 控制流
    void visit(BreakExpression& node) override;
    void visit(ContinueExpression& node) override;
    void visit(ReturnExpression& node) override;

    // 其他
    void visit(SelfParam& node) override;
    void visit(ShorthandSelf& node) override;
    void visit(TypedSelf& node) override;
};
