#include "parser/astprinter.hpp"
#include "parser/astnode.hpp"
#include <iostream>

ASTPrinter::ASTPrinter(std::ostream& output, bool use_colors)
    : output(output), indent_level(0), use_colors(use_colors) {}

void ASTPrinter::indent() {
    for (int i = 0; i < indent_level; ++i) {
        output << "  ";
    }
}

void ASTPrinter::print_with_indent(const std::string& text) {
    indent();
    output << text;
}

std::string ASTPrinter::get_color_code(const std::string& color) {
    if (!use_colors) return "";
    
    if (color == "red") return "\033[31m";
    if (color == "green") return "\033[32m";
    if (color == "yellow") return "\033[33m";
    if (color == "blue") return "\033[34m";
    if (color == "magenta") return "\033[35m";
    if (color == "cyan") return "\033[36m";
    if (color == "white") return "\033[37m";
    if (color == "bold") return "\033[1m";
    
    return "";
}

std::string ASTPrinter::reset_color() {
    if (!use_colors) return "";
    return "\033[0m";
}

// 顶层节点
void ASTPrinter::visit(Crate& node) {
    print_with_indent(get_color_code("bold") + "Crate" + reset_color() + "\n");
    indent_level++;
    for (const auto& item : node.items) {
        if (item) {
            item->accept(this);
        }
    }
    indent_level--;
}

void ASTPrinter::visit(Item& node) {
    print_with_indent(get_color_code("cyan") + "Item" + reset_color() + "\n");
    indent_level++;
    if (node.item) {
        node.item->accept(this);
    }
    indent_level--;
}

// 声明类节点 (Items)
void ASTPrinter::visit(Function& node) {
    print_with_indent(get_color_code("green") + "Function" + reset_color());
    if (node.is_const) {
        output << " " << get_color_code("yellow") << "const" << reset_color();
    }
    output << " " << get_color_code("white") << node.identifier << reset_color() << "\n";
    indent_level++;
    if (node.function_parameters) {
        node.function_parameters->accept(this);
    }
    if (node.function_return_type) {
        node.function_return_type->accept(this);
    }
    if (node.block_expression) {
        node.block_expression->accept(this);
    }
    indent_level--;
}

void ASTPrinter::visit(Struct& node) {
    print_with_indent(get_color_code("green") + "Struct" + reset_color() + "\n");
    indent_level++;
    if (node.struct_struct) {
        node.struct_struct->accept(this);
    }
    indent_level--;
}

void ASTPrinter::visit(Enumeration& node) {
    print_with_indent(get_color_code("green") + "Enumeration" + reset_color());
    output << " " << get_color_code("white") << node.identifier << reset_color() << "\n";
    indent_level++;
    if (node.enum_variants) {
        node.enum_variants->accept(this);
    }
    indent_level--;
}

void ASTPrinter::visit(ConstantItem& node) {
    print_with_indent(get_color_code("green") + "ConstantItem" + reset_color());
    output << " " << get_color_code("white") << node.identifier << reset_color() << "\n";
    indent_level++;
    if (node.type) {
        node.type->accept(this);
    }
    if (node.expression) {
        node.expression->accept(this);
    }
    indent_level--;
}

void ASTPrinter::visit(Trait& node) {
    print_with_indent(get_color_code("green") + "Trait" + reset_color());
    output << " " << get_color_code("white") << node.identifier << reset_color() << "\n";
    indent_level++;
    for (const auto& item : node.associated_item) {
        if (item) {
            item->accept(this);
        }
    }
    indent_level--;
}

void ASTPrinter::visit(Implementation& node) {
    print_with_indent(get_color_code("green") + "Implementation" + reset_color() + "\n");
    indent_level++;
    if (node.impl) {
        node.impl->accept(this);
    }
    indent_level--;
}

// 函数相关节点
void ASTPrinter::visit(FunctionParameters& node) {
    print_with_indent(get_color_code("magenta") + "FunctionParameters" + reset_color() + "\n");
    indent_level++;
    if (node.self_param) {
        node.self_param->accept(this);
    }
    for (const auto& param : node.function_param) {
        if (param) {
            param->accept(this);
        }
    }
    indent_level--;
}

void ASTPrinter::visit(SelfParam& node) {
    print_with_indent(get_color_code("magenta") + "SelfParam" + reset_color() + "\n");
    indent_level++;
    if (node.child) {
        node.child->accept(this);
    }
    indent_level--;
}

void ASTPrinter::visit(ShorthandSelf& node) {
    print_with_indent(get_color_code("magenta") + "ShorthandSelf" + reset_color());
    if (node.is_reference) {
        output << " " << get_color_code("yellow") << "&" << reset_color();
    }
    if (node.is_mutable) {
        output << " " << get_color_code("yellow") << "mut" << reset_color();
    }
    output << " " << get_color_code("white") << "self" << reset_color() << "\n";
}

void ASTPrinter::visit(TypedSelf& node) {
    print_with_indent(get_color_code("magenta") + "TypedSelf" + reset_color());
    if (node.is_mutable) {
        output << " " << get_color_code("yellow") << "mut" << reset_color();
    }
    output << " " << get_color_code("white") << "self" << reset_color() << "\n";
    indent_level++;
    if (node.type) {
        node.type->accept(this);
    }
    indent_level--;
}

void ASTPrinter::visit(FunctionParam& node) {
    print_with_indent(get_color_code("magenta") + "FunctionParam" + reset_color() + "\n");
    indent_level++;
    if (node.pattern_no_top_alt) {
        node.pattern_no_top_alt->accept(this);
    }
    if (node.type) {
        node.type->accept(this);
    }
    indent_level--;
}

void ASTPrinter::visit(FunctionReturnType& node) {
    print_with_indent(get_color_code("magenta") + "FunctionReturnType" + reset_color() + "\n");
    indent_level++;
    if (node.type) {
        node.type->accept(this);
    }
    indent_level--;
}

// 结构体相关节点
void ASTPrinter::visit(StructStruct& node) {
    print_with_indent(get_color_code("blue") + "StructStruct" + reset_color());
    output << " " << get_color_code("white") << node.identifier << reset_color() << "\n";
    indent_level++;
    if (node.struct_fields) {
        node.struct_fields->accept(this);
    }
    indent_level--;
}

void ASTPrinter::visit(StructFields& node) {
    print_with_indent(get_color_code("blue") + "StructFields" + reset_color() + "\n");
    indent_level++;
    for (const auto& field : node.struct_fields) {
        if (field) {
            field->accept(this);
        }
    }
    indent_level--;
}

void ASTPrinter::visit(StructField& node) {
    print_with_indent(get_color_code("blue") + "StructField" + reset_color());
    output << " " << get_color_code("white") << node.identifier << reset_color() << "\n";
    indent_level++;
    if (node.type) {
        node.type->accept(this);
    }
    indent_level--;
}

// 枚举相关节点
void ASTPrinter::visit(EnumVariants& node) {
    print_with_indent(get_color_code("blue") + "EnumVariants" + reset_color() + "\n");
    indent_level++;
    for (const auto& variant : node.enum_variant) {
        if (variant) {
            variant->accept(this);
        }
    }
    indent_level--;
}

void ASTPrinter::visit(EnumVariant& node) {
    print_with_indent(get_color_code("blue") + "EnumVariant" + reset_color());
    output << " " << get_color_code("white") << node.identifier << reset_color() << "\n";
}

// 实现相关节点
void ASTPrinter::visit(AssociatedItem& node) {
    print_with_indent(get_color_code("cyan") + "AssociatedItem" + reset_color() + "\n");
    indent_level++;
    if (node.child) {
        node.child->accept(this);
    }
    indent_level--;
}

void ASTPrinter::visit(InherentImpl& node) {
    print_with_indent(get_color_code("cyan") + "InherentImpl" + reset_color() + "\n");
    indent_level++;
    if (node.type) {
        node.type->accept(this);
    }
    for (const auto& item : node.associated_item) {
        if (item) {
            item->accept(this);
        }
    }
    indent_level--;
}

void ASTPrinter::visit(TraitImpl& node) {
    print_with_indent(get_color_code("cyan") + "TraitImpl" + reset_color());
    output << " " << get_color_code("white") << node.identifier << reset_color() << "\n";
    indent_level++;
    if (node.type) {
        node.type->accept(this);
    }
    for (const auto& item : node.associated_item) {
        if (item) {
            item->accept(this);
        }
    }
    indent_level--;
}

// 语句类节点
void ASTPrinter::visit(Statement& node) {
    print_with_indent(get_color_code("yellow") + "Statement" + reset_color() + "\n");
    indent_level++;
    if (node.child) {
        node.child->accept(this);
    }
    indent_level--;
}

void ASTPrinter::visit(LetStatement& node) {
    print_with_indent(get_color_code("yellow") + "LetStatement" + reset_color() + "\n");
    indent_level++;
    if (node.pattern_no_top_alt) {
        node.pattern_no_top_alt->accept(this);
    }
    if (node.type) {
        node.type->accept(this);
    }
    if (node.expression) {
        node.expression->accept(this);
    }
    indent_level--;
}

void ASTPrinter::visit(ExpressionStatement& node) {
    print_with_indent(get_color_code("yellow") + "ExpressionStatement" + reset_color());
    if (node.has_semi) {
        output << " " << get_color_code("red") << ";" << reset_color();
    }
    output << "\n";
    indent_level++;
    if (node.child) {
        node.child->accept(this);
    }
    indent_level--;
}

void ASTPrinter::visit(Statements& node) {
    print_with_indent(get_color_code("yellow") + "Statements" + reset_color() + "\n");
    indent_level++;
    for (const auto& stmt : node.statements) {
        if (stmt) {
            stmt->accept(this);
        }
    }
    indent_level--;
}

// 表达式类节点
void ASTPrinter::visit(Expression& node) {
    print_with_indent(get_color_code("red") + "Expression" + reset_color() + "\n");
    indent_level++;
    if (node.child) {
        node.child->accept(this);
    }
    indent_level--;
}

void ASTPrinter::visit(ExpressionWithoutBlock& node) {
    print_with_indent(get_color_code("red") + "ExpressionWithoutBlock" + reset_color() + "\n");
    indent_level++;
    if (node.child) {
        node.child->accept(this);
    }
    indent_level--;
}

void ASTPrinter::visit(ExpressionWithBlock& node) {
    print_with_indent(get_color_code("red") + "ExpressionWithBlock" + reset_color() + "\n");
    indent_level++;
    if (node.child) {
        node.child->accept(this);
    }
    indent_level--;
}

// 字面量表达式
void ASTPrinter::visit(CharLiteral& node) {
    print_with_indent(get_color_code("green") + "CharLiteral" + reset_color());
    output << " " << get_color_code("white") << "'" << node.value << "'" << reset_color() << "\n";
}

void ASTPrinter::visit(StringLiteral& node) {
    print_with_indent(get_color_code("green") + "StringLiteral" + reset_color());
    output << " " << get_color_code("white") << "\"" << node.value << "\"" << reset_color() << "\n";
}

void ASTPrinter::visit(RawStringLiteral& node) {
    print_with_indent(get_color_code("green") + "RawStringLiteral" + reset_color());
    output << " " << get_color_code("white") << "r\"" << node.value << "\"" << reset_color() << "\n";
}

void ASTPrinter::visit(CStringLiteral& node) {
    print_with_indent(get_color_code("green") + "CStringLiteral" + reset_color());
    output << " " << get_color_code("white") << "c\"" << node.value << "\"" << reset_color() << "\n";
}

void ASTPrinter::visit(RawCStringLiteral& node) {
    print_with_indent(get_color_code("green") + "RawCStringLiteral" + reset_color());
    output << " " << get_color_code("white") << "cr\"" << node.value << "\"" << reset_color() << "\n";
}

void ASTPrinter::visit(IntegerLiteral& node) {
    print_with_indent(get_color_code("green") + "IntegerLiteral" + reset_color());
    output << " " << get_color_code("white") << node.value << reset_color() << "\n";
}

void ASTPrinter::visit(BoolLiteral& node) {
    print_with_indent(get_color_code("green") + "BoolLiteral" + reset_color());
    output << " " << get_color_code("white") << (node.value ? "true" : "false") << reset_color() << "\n";
}

// 路径和访问表达式
void ASTPrinter::visit(PathExpression& node) {
    print_with_indent(get_color_code("cyan") + "PathExpression" + reset_color() + "\n");
    indent_level++;
    if (node.path_in_expression) {
        node.path_in_expression->accept(this);
    }
    indent_level--;
}

void ASTPrinter::visit(FieldExpression& node) {
    print_with_indent(get_color_code("cyan") + "FieldExpression" + reset_color());
    output << " ." << get_color_code("white") << node.identifier << reset_color() << "\n";
    indent_level++;
    if (node.expression) {
        node.expression->accept(this);
    }
    indent_level--;
}

// 运算符表达式
void ASTPrinter::visit(UnaryExpression& node) {
    print_with_indent(get_color_code("magenta") + "UnaryExpression" + reset_color());
    std::string op;
    switch (node.type) {
        case UnaryExpression::MINUS: op = "-"; break;
        case UnaryExpression::NOT: op = "!"; break;
        case UnaryExpression::TRY: op = "?"; break;
    }
    output << " " << get_color_code("yellow") << op << reset_color() << "\n";
    indent_level++;
    if (node.expression) {
        node.expression->accept(this);
    }
    indent_level--;
}

void ASTPrinter::visit(BorrowExpression& node) {
    print_with_indent(get_color_code("magenta") + "BorrowExpression" + reset_color());
    output << " " << get_color_code("yellow");
    if (node.is_double) output << "&";
    output << "&";
    if (node.is_mutable) output << " mut";
    output << reset_color() << "\n";
    indent_level++;
    if (node.expression) {
        node.expression->accept(this);
    }
    indent_level--;
}

void ASTPrinter::visit(DereferenceExpression& node) {
    print_with_indent(get_color_code("magenta") + "DereferenceExpression" + reset_color());
    output << " " << get_color_code("yellow") << "*" << reset_color() << "\n";
    indent_level++;
    if (node.expression) {
        node.expression->accept(this);
    }
    indent_level--;
}

void ASTPrinter::visit(BinaryExpression& node) {
    print_with_indent(get_color_code("magenta") + "BinaryExpression" + reset_color());
    std::string op;
    switch (node.type) {
        case BinaryExpression::PLUS: op = "+"; break;
        case BinaryExpression::MINUS: op = "-"; break;
        case BinaryExpression::STAR: op = "*"; break;
        case BinaryExpression::SLASH: op = "/"; break;
        case BinaryExpression::PERCENT: op = "%"; break;
        case BinaryExpression::CARET: op = "^"; break;
        case BinaryExpression::AND: op = "&"; break;
        case BinaryExpression::OR: op = "|"; break;
        case BinaryExpression::SHL: op = "<<"; break;
        case BinaryExpression::SHR: op = ">>"; break;
        case BinaryExpression::EQ_EQ: op = "=="; break;
        case BinaryExpression::NE: op = "!="; break;
        case BinaryExpression::GT: op = ">"; break;
        case BinaryExpression::LT: op = "<"; break;
        case BinaryExpression::GE: op = ">="; break;
        case BinaryExpression::LE: op = "<="; break;
        case BinaryExpression::AND_AND: op = "&&"; break;
        case BinaryExpression::OR_OR: op = "||"; break;
    }
    output << " " << get_color_code("yellow") << op << reset_color() << "\n";
    indent_level++;
    if (node.lhs) {
        node.lhs->accept(this);
    }
    if (node.rhs) {
        node.rhs->accept(this);
    }
    indent_level--;
}

void ASTPrinter::visit(AssignmentExpression& node) {
    print_with_indent(get_color_code("magenta") + "AssignmentExpression" + reset_color());
    output << " " << get_color_code("yellow") << "=" << reset_color() << "\n";
    indent_level++;
    if (node.lhs) {
        node.lhs->accept(this);
    }
    if (node.rhs) {
        node.rhs->accept(this);
    }
    indent_level--;
}

void ASTPrinter::visit(CompoundAssignmentExpression& node) {
    print_with_indent(get_color_code("magenta") + "CompoundAssignmentExpression" + reset_color());
    std::string op;
    switch (node.type) {
        case CompoundAssignmentExpression::PLUS_EQ: op = "+="; break;
        case CompoundAssignmentExpression::MINUS_EQ: op = "-="; break;
        case CompoundAssignmentExpression::STAR_EQ: op = "*="; break;
        case CompoundAssignmentExpression::SLASH_EQ: op = "/="; break;
        case CompoundAssignmentExpression::PERCENT_EQ: op = "%="; break;
        case CompoundAssignmentExpression::CARET_EQ: op = "^="; break;
        case CompoundAssignmentExpression::AND_EQ: op = "&="; break;
        case CompoundAssignmentExpression::OR_EQ: op = "|="; break;
        case CompoundAssignmentExpression::SHL_EQ: op = "<<="; break;
        case CompoundAssignmentExpression::SHR_EQ: op = ">>="; break;
    }
    output << " " << get_color_code("yellow") << op << reset_color() << "\n";
    indent_level++;
    if (node.lhs) {
        node.lhs->accept(this);
    }
    if (node.rhs) {
        node.rhs->accept(this);
    }
    indent_level--;
}

void ASTPrinter::visit(TypeCastExpression& node) {
    print_with_indent(get_color_code("magenta") + "TypeCastExpression" + reset_color());
    output << " " << get_color_code("yellow") << "as" << reset_color() << "\n";
    indent_level++;
    if (node.expression) {
        node.expression->accept(this);
    }
    if (node.type) {
        node.type->accept(this);
    }
    indent_level--;
}

// 调用和索引表达式
void ASTPrinter::visit(CallExpression& node) {
    print_with_indent(get_color_code("cyan") + "CallExpression" + reset_color() + "\n");
    indent_level++;
    if (node.expression) {
        node.expression->accept(this);
    }
    if (node.call_params) {
        node.call_params->accept(this);
    }
    indent_level--;
}

void ASTPrinter::visit(MethodCallExpression& node) {
    print_with_indent(get_color_code("cyan") + "MethodCallExpression" + reset_color() + "\n");
    indent_level++;
    if (node.expression) {
        node.expression->accept(this);
    }
    if (node.path_ident_segment) {
        node.path_ident_segment->accept(this);
    }
    if (node.call_params) {
        node.call_params->accept(this);
    }
    indent_level--;
}

void ASTPrinter::visit(IndexExpression& node) {
    print_with_indent(get_color_code("cyan") + "IndexExpression" + reset_color() + "\n");
    indent_level++;
    if (node.base_expression) {
        node.base_expression->accept(this);
    }
    if (node.index_expression) {
        node.index_expression->accept(this);
    }
    indent_level--;
}

// 结构体和数组表达式
void ASTPrinter::visit(StructExpression& node) {
    print_with_indent(get_color_code("cyan") + "StructExpression" + reset_color() + "\n");
    indent_level++;
    if (node.path_in_expression) {
        node.path_in_expression->accept(this);
    }
    if (node.struct_expr_fields) {
        node.struct_expr_fields->accept(this);
    }
    indent_level--;
}

void ASTPrinter::visit(ArrayExpression& node) {
    print_with_indent(get_color_code("cyan") + "ArrayExpression" + reset_color() + "\n");
    indent_level++;
    if (node.array_elements) {
        node.array_elements->accept(this);
    }
    indent_level--;
}

void ASTPrinter::visit(GroupedExpression& node) {
    print_with_indent(get_color_code("cyan") + "GroupedExpression" + reset_color() + "\n");
    indent_level++;
    if (node.expression) {
        node.expression->accept(this);
    }
    indent_level--;
}

// 控制流表达式
void ASTPrinter::visit(BlockExpression& node) {
    print_with_indent(get_color_code("yellow") + "BlockExpression" + reset_color() + "\n");
    indent_level++;
    if (node.statements) {
        node.statements->accept(this);
    }
    indent_level--;
}

void ASTPrinter::visit(IfExpression& node) {
    print_with_indent(get_color_code("yellow") + "IfExpression" + reset_color() + "\n");
    indent_level++;
    if (node.condition) {
        node.condition->accept(this);
    }
    if (node.then_block) {
        node.then_block->accept(this);
    }
    if (node.else_branch) {
        node.else_branch->accept(this);
    }
    indent_level--;
}

void ASTPrinter::visit(LoopExpression& node) {
    print_with_indent(get_color_code("yellow") + "LoopExpression" + reset_color() + "\n");
    indent_level++;
    if (node.child) {
        node.child->accept(this);
    }
    indent_level--;
}

void ASTPrinter::visit(InfiniteLoopExpression& node) {
    print_with_indent(get_color_code("yellow") + "InfiniteLoopExpression" + reset_color() + "\n");
    indent_level++;
    if (node.block_expression) {
        node.block_expression->accept(this);
    }
    indent_level--;
}

void ASTPrinter::visit(PredicateLoopExpression& node) {
    print_with_indent(get_color_code("yellow") + "PredicateLoopExpression" + reset_color() + "\n");
    indent_level++;
    if (node.condition) {
        node.condition->accept(this);
    }
    if (node.block_expression) {
        node.block_expression->accept(this);
    }
    indent_level--;
}

void ASTPrinter::visit(BreakExpression& node) {
    print_with_indent(get_color_code("yellow") + "BreakExpression" + reset_color() + "\n");
    indent_level++;
    if (node.expression) {
        node.expression->accept(this);
    }
    indent_level--;
}

void ASTPrinter::visit(ContinueExpression& node) {
    print_with_indent(get_color_code("yellow") + "ContinueExpression" + reset_color() + "\n");
}

void ASTPrinter::visit(ReturnExpression& node) {
    print_with_indent(get_color_code("yellow") + "ReturnExpression" + reset_color() + "\n");
    indent_level++;
    if (node.expression) {
        node.expression->accept(this);
    }
    indent_level--;
}

// 辅助表达式节点
void ASTPrinter::visit(Condition& node) {
    print_with_indent(get_color_code("cyan") + "Condition" + reset_color() + "\n");
    indent_level++;
    if (node.expression) {
        node.expression->accept(this);
    }
    indent_level--;
}

void ASTPrinter::visit(ArrayElements& node) {
    print_with_indent(get_color_code("cyan") + "ArrayElements" + reset_color());
    if (node.is_semicolon_separated) {
        output << " " << get_color_code("yellow") << "(semicolon separated)" << reset_color();
    }
    output << "\n";
    indent_level++;
    for (const auto& expr : node.expressions) {
        if (expr) {
            expr->accept(this);
        }
    }
    indent_level--;
}

void ASTPrinter::visit(StructExprFields& node) {
    print_with_indent(get_color_code("cyan") + "StructExprFields" + reset_color() + "\n");
    indent_level++;
    for (const auto& field : node.struct_expr_fields) {
        if (field) {
            field->accept(this);
        }
    }
    indent_level--;
}

void ASTPrinter::visit(StructExprField& node) {
    print_with_indent(get_color_code("cyan") + "StructExprField" + reset_color());
    output << " " << get_color_code("white") << node.identifier << reset_color() << "\n";
    indent_level++;
    if (node.expression) {
        node.expression->accept(this);
    }
    indent_level--;
}

void ASTPrinter::visit(CallParams& node) {
    print_with_indent(get_color_code("cyan") + "CallParams" + reset_color() + "\n");
    indent_level++;
    for (const auto& expr : node.expressions) {
        if (expr) {
            expr->accept(this);
        }
    }
    indent_level--;
}

// 模式类节点
void ASTPrinter::visit(PatternNoTopAlt& node) {
    print_with_indent(get_color_code("blue") + "PatternNoTopAlt" + reset_color() + "\n");
    indent_level++;
    if (node.child) {
        node.child->accept(this);
    }
    indent_level--;
}

void ASTPrinter::visit(IdentifierPattern& node) {
    print_with_indent(get_color_code("blue") + "IdentifierPattern" + reset_color());
    if (node.is_ref) {
        output << " " << get_color_code("yellow") << "ref" << reset_color();
    }
    if (node.is_mutable) {
        output << " " << get_color_code("yellow") << "mut" << reset_color();
    }
    output << " " << get_color_code("white") << node.identifier << reset_color() << "\n";
}

void ASTPrinter::visit(ReferencePattern& node) {
    print_with_indent(get_color_code("blue") + "ReferencePattern" + reset_color());
    output << " " << get_color_code("yellow");
    if (node.is_double) output << "&";
    output << "&";
    if (node.is_mutable) output << " mut";
    output << reset_color() << "\n";
    indent_level++;
    if (node.pattern) {
        node.pattern->accept(this);
    }
    indent_level--;
}

// 类型类节点
void ASTPrinter::visit(Type& node) {
    print_with_indent(get_color_code("magenta") + "Type" + reset_color() + "\n");
    indent_level++;
    if (node.child) {
        node.child->accept(this);
    }
    indent_level--;
}

void ASTPrinter::visit(ReferenceType& node) {
    print_with_indent(get_color_code("magenta") + "ReferenceType" + reset_color());
    output << " " << get_color_code("yellow") << "&";
    if (node.is_mutable) output << " mut";
    output << reset_color() << "\n";
    indent_level++;
    if (node.type) {
        node.type->accept(this);
    }
    indent_level--;
}

void ASTPrinter::visit(ArrayType& node) {
    print_with_indent(get_color_code("magenta") + "ArrayType" + reset_color() + "\n");
    indent_level++;
    if (node.type) {
        node.type->accept(this);
    }
    if (node.expression) {
        node.expression->accept(this);
    }
    indent_level--;
}

void ASTPrinter::visit(UnitType& node) {
    print_with_indent(get_color_code("magenta") + "UnitType" + reset_color() + "\n");
}

// 路径类节点
void ASTPrinter::visit(PathInExpression& node) {
    print_with_indent(get_color_code("cyan") + "PathInExpression" + reset_color() + "\n");
    indent_level++;
    if (node.segment1) {
        node.segment1->accept(this);
    }
    if (node.segment2) {
        node.segment2->accept(this);
    }
    indent_level--;
}

void ASTPrinter::visit(PathIdentSegment& node) {
    print_with_indent(get_color_code("cyan") + "PathIdentSegment" + reset_color());
    output << " " << get_color_code("white");
    switch (node.type) {
        case 0: output << node.identifier; break;
        case 1: output << "self"; break;
        case 2: output << "Self"; break;
    }
    output << reset_color() << "\n";
}