#include "semantic/const_evaluator.hpp"

// 辅助函数实现
std::shared_ptr<ConstValue> ConstEvaluator::createConstValueFromExpression(std::shared_ptr<ASTNode> expression) {
    if (!expression) {
        return nullptr;
    }
    
    // 处理 Expression 包装器
    if (auto expr_wrapper = std::dynamic_pointer_cast<Expression>(expression)) {
        if (expr_wrapper->child) {
            return createConstValueFromExpression(expr_wrapper->child);
        }
    }
    
    // 尝试转换为不同的字面量类型
    if (auto int_literal = std::dynamic_pointer_cast<IntegerLiteral>(expression)) {
        try {
            int value = std::stoi(int_literal->value);
            auto result = std::make_shared<ConstValueInt>(value, expression);
            return result;
        } catch (const std::exception& e) {
            // 转换失败，返回 null
            return nullptr;
        }
    }
    if (auto bool_literal = std::dynamic_pointer_cast<BoolLiteral>(expression)) {
        return std::make_shared<ConstValueBool>(bool_literal->value, expression);
    }
    if (auto char_literal = std::dynamic_pointer_cast<CharLiteral>(expression)) {
        if (!char_literal->value.empty()) {
            return std::make_shared<ConstValueChar>(char_literal->value[0], expression);
        }
    }
    if (auto string_literal = std::dynamic_pointer_cast<StringLiteral>(expression)) {
        return std::make_shared<ConstValueString>(string_literal->value, expression);
    }

    if (auto path_expr = std::dynamic_pointer_cast<PathExpression>(expression)) {
        if (path_expr->path_in_expression) {
            return createConstValueFromExpression(std::dynamic_pointer_cast<ASTNode>(path_expr->path_in_expression));
        }
    }
    
    if (auto path_in_expr = std::dynamic_pointer_cast<PathInExpression>(expression)) {
        if (path_in_expr->segment2) {
            auto struct_identifier = path_in_expr->segment1->identifier;
            auto identifier = path_in_expr->segment2->identifier;
            auto struct_symbol = current_scope->findStructSymbol(struct_identifier);
            if (!struct_symbol) throw std::runtime_error("Const Evaluation Error");
            auto const_symbol = struct_symbol->getAssociatedConst(identifier);
            if (!const_symbol) throw std::runtime_error("Const Evaluation Error");
            return const_symbol->getValue();
        } else {
            auto identifier = path_in_expr->segment1->identifier;
            auto const_symbol = current_scope->getConstSymbol(identifier);
            if (!const_symbol) throw std::runtime_error("Const Evaluation Error");
            return const_symbol->getValue();
        }
    }

    // 处理括号表达式
    if (auto grouped_expr = std::dynamic_pointer_cast<GroupedExpression>(expression)) {
        return createConstValueFromExpression(std::dynamic_pointer_cast<ASTNode>(grouped_expr->expression));
    }

    // 处理一元表达式（负号）
    if (auto unary_expr = std::dynamic_pointer_cast<UnaryExpression>(expression)) {
        if (unary_expr->type == UnaryExpression::MINUS) {
            auto operand_value = createConstValueFromExpression(std::dynamic_pointer_cast<ASTNode>(unary_expr->expression));
            if (!operand_value || !operand_value->isInt()) {
                throw std::runtime_error("Const Evaluation Error: Unary minus can only be applied to integer constants");
            }
            auto int_value = std::dynamic_pointer_cast<ConstValueInt>(operand_value);
            return std::make_shared<ConstValueInt>(-int_value->getValue(), expression);
        } else {
            throw std::runtime_error("Const Evaluation Error: Only unary minus is supported in constant expressions");
        }
    }

    // 处理二元表达式（算术运算和位运算）
    if (auto binary_expr = std::dynamic_pointer_cast<BinaryExpression>(expression)) {
        auto left_value = createConstValueFromExpression(std::dynamic_pointer_cast<ASTNode>(binary_expr->lhs));
        auto right_value = createConstValueFromExpression(std::dynamic_pointer_cast<ASTNode>(binary_expr->rhs));
        
        if (!left_value || !right_value) {
            throw std::runtime_error("Const Evaluation Error: Invalid operands in binary expression");
        }
        
        // 只支持整型运算
        if (!left_value->isInt() || !right_value->isInt()) {
            throw std::runtime_error("Const Evaluation Error: Binary operations only support integer constants");
        }
        
        auto left_int = std::dynamic_pointer_cast<ConstValueInt>(left_value);
        auto right_int = std::dynamic_pointer_cast<ConstValueInt>(right_value);
        
        int result;
        switch (binary_expr->type) {
            case BinaryExpression::PLUS:
                result = left_int->getValue() + right_int->getValue();
                break;
            case BinaryExpression::MINUS:
                result = left_int->getValue() - right_int->getValue();
                break;
            case BinaryExpression::STAR:
                result = left_int->getValue() * right_int->getValue();
                break;
            case BinaryExpression::SLASH:
                if (right_int->getValue() == 0) {
                    throw std::runtime_error("Const Evaluation Error: Division by zero");
                }
                result = left_int->getValue() / right_int->getValue();
                break;
            case BinaryExpression::PERCENT:
                if (right_int->getValue() == 0) {
                    throw std::runtime_error("Const Evaluation Error: Modulo by zero");
                }
                result = left_int->getValue() % right_int->getValue();
                break;
            // 位运算
            case BinaryExpression::CARET:  // ^
                result = left_int->getValue() ^ right_int->getValue();
                break;
            case BinaryExpression::AND:    // &
                result = left_int->getValue() & right_int->getValue();
                break;
            case BinaryExpression::OR:     // |
                result = left_int->getValue() | right_int->getValue();
                break;
            case BinaryExpression::SHL:   // <<
                result = left_int->getValue() << right_int->getValue();
                break;
            case BinaryExpression::SHR:   // >>
                result = left_int->getValue() >> right_int->getValue();
                break;
            // 不支持的操作符
            case BinaryExpression::EQ_EQ:   // ==
            case BinaryExpression::NE:      // !=
            case BinaryExpression::GT:      // >
            case BinaryExpression::LT:      // <
            case BinaryExpression::GE:      // >=
            case BinaryExpression::LE:      // <=
            case BinaryExpression::AND_AND: // &&
            case BinaryExpression::OR_OR:   // ||
                throw std::runtime_error("Const Evaluation Error: Comparison and logical operators are not supported in constant expressions");
            default:
                throw std::runtime_error("Const Evaluation Error: Unsupported binary operator");
        }
        
        return std::make_shared<ConstValueInt>(result, expression);
    }

    throw std::runtime_error("Const Evaluation Error: Unsupported expression type in constant context");
}

ConstEvaluator::ConstEvaluator(std::shared_ptr<Scope> root_scope) {
    this->root_scope = root_scope;
    this->current_scope = root_scope;
}

void ConstEvaluator::visit(Crate& node) {
    for (auto item: node.items) {
        item->accept(this);
    }
}

void ConstEvaluator::visit(ConstantItem& node) {
    std::string identifier = node.identifier;
    auto const_symbol = current_scope->getConstSymbol(identifier);
    const_symbol->setValue(createConstValueFromExpression(node.expression));
}

void ConstEvaluator::visit(ArrayType& node) {
    
}