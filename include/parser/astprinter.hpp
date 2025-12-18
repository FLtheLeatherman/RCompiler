#pragma once

#include "parser/visitor.hpp"
#include <string>
#include <ostream>

class ASTPrinter : public ASTVisitor {
private:
    std::ostream& output;
    int indent_level;
    bool use_colors;
    
    // 辅助方法
    void indent();
    void print_with_indent(const std::string& text);
    std::string get_color_code(const std::string& color);
    std::string reset_color();
    
public:
    explicit ASTPrinter(std::ostream& output = std::cout, bool use_colors = true);
    virtual ~ASTPrinter() = default;
    
    // 设置选项
    void set_use_colors(bool enabled) { use_colors = enabled; }
    void set_indent_level(int level) { indent_level = level; }
    
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
    void visit(SelfParam& node) override;
    void visit(ShorthandSelf& node) override;
    void visit(TypedSelf& node) override;
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
    
    // 字面量表达式
    void visit(CharLiteral& node) override;
    void visit(StringLiteral& node) override;
    void visit(RawStringLiteral& node) override;
    void visit(CStringLiteral& node) override;
    void visit(RawCStringLiteral& node) override;
    void visit(IntegerLiteral& node) override;
    void visit(BoolLiteral& node) override;
    
    // 路径和访问表达式
    void visit(PathExpression& node) override;
    void visit(FieldExpression& node) override;
    
    // 运算符表达式
    void visit(UnaryExpression& node) override;
    void visit(BorrowExpression& node) override;
    void visit(DereferenceExpression& node) override;
    void visit(BinaryExpression& node) override;
    void visit(AssignmentExpression& node) override;
    void visit(CompoundAssignmentExpression& node) override;
    void visit(TypeCastExpression& node) override;
    
    // 调用和索引表达式
    void visit(CallExpression& node) override;
    void visit(MethodCallExpression& node) override;
    void visit(IndexExpression& node) override;
    
    // 结构体和数组表达式
    void visit(StructExpression& node) override;
    void visit(ArrayExpression& node) override;
    void visit(GroupedExpression& node) override;
    
    // 控制流表达式
    void visit(BlockExpression& node) override;
    void visit(IfExpression& node) override;
    void visit(LoopExpression& node) override;
    void visit(InfiniteLoopExpression& node) override;
    void visit(PredicateLoopExpression& node) override;
    void visit(BreakExpression& node) override;
    void visit(ContinueExpression& node) override;
    void visit(ReturnExpression& node) override;
    
    // 辅助表达式节点
    void visit(Condition& node) override;
    void visit(ArrayElements& node) override;
    void visit(StructExprFields& node) override;
    void visit(StructExprField& node) override;
    void visit(CallParams& node) override;
    
    // 模式类节点
    void visit(PatternNoTopAlt& node) override;
    void visit(IdentifierPattern& node) override;
    void visit(ReferencePattern& node) override;
    
    // 类型类节点
    void visit(Type& node) override;
    void visit(ReferenceType& node) override;
    void visit(ArrayType& node) override;
    void visit(UnitType& node) override;
    
    // 路径类节点
    void visit(PathInExpression& node) override;
    void visit(PathIdentSegment& node) override;
};