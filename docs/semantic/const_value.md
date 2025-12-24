### ConstValue 集成支持

ConstValue 系统，用于常量求值。

#### 功能特点
- **表达式到 ConstValue 转换**: 使用 `createConstValueFromExpression()` 将 AST 表达式转换为对应的 ConstValue 对象
- **类型安全的常量处理**: 支持整型、布尔、字符、字符串、结构体和枚举类型的常量值
- **完整的常量表达式支持**: 支持字面量、路径表达式、算术运算、位运算和括号表达式
- **错误处理**: 对不支持的运算和类型错误提供清晰的错误信息

#### 支持的表达式类型

根据常量环境规则，支持以下表达式：

1. **字面量**
   - 整型字面量 (`IntegerLiteral`)
   - 布尔字面量 (`BoolLiteral`)
   - 字符字面量 (`CharLiteral`)
   - 字符串字面量 (`StringLiteral`)

2. **路径表达式**
   - 常量引用 (`const A`)
   - 结构体关联常量 (`Struct::CONST`)

3. **算术表达式**
   - 一元表达式：负号 (`-expr`)
   - 二元表达式：`+`, `-`, `*`, `/`, `%`

4. **位运算表达式**
   - 按位与 (`&`)
   - 按位或 (`|`)
   - 按位异或 (`^`)
   - 左移 (`<<`)
   - 右移 (`>>`)

5. **括号表达式**
   - `(expr)` 用于改变运算优先级

#### 不支持的表达式类型

以下表达式不会出现在常量环境中，会抛出错误：
- 块表达式和所有语句
- 结构体表达式和字段访问表达式
- 所有 `ExpressionWithBlock`（包括 `If`, `While`, `Loop`）
- 借用表达式、解引用表达式
- 类型转换表达式
- 逻辑表达式 (`&&`, `||`)
- 比较表达式 (`==`, `!=`, `<`, `>`, `<=`, `>=`)
- 函数调用、方法调用

#### 实现细节

```cpp
// ConstEvaluator 类中的核心函数
std::shared_ptr<ConstValue> ConstEvaluator::createConstValueFromExpression(std::shared_ptr<ASTNode> expression) {
    // 处理 Expression 包装器
    if (auto expr_wrapper = std::dynamic_pointer_cast<Expression>(expression)) {
        if (expr_wrapper->child) {
            return createConstValueFromExpression(expr_wrapper->child);
        }
    }
    
    // 处理字面量
    if (auto int_literal = std::dynamic_pointer_cast<IntegerLiteral>(expression)) {
        int value = std::stoi(int_literal->value);
        return std::make_shared<ConstValueInt>(value, expression);
    }
    // ... 其他字面量类型
    
    // 处理路径表达式
    if (auto path_expr = std::dynamic_pointer_cast<PathExpression>(expression)) {
        return createConstValueFromExpression(path_expr->path_in_expression);
    }
    
    // 处理一元表达式（负号）
    if (auto unary_expr = std::dynamic_pointer_cast<UnaryExpression>(expression)) {
        if (unary_expr->type == UnaryExpression::MINUS) {
            auto operand = createConstValueFromExpression(unary_expr->expression);
            auto int_value = std::dynamic_pointer_cast<ConstValueInt>(operand);
            return std::make_shared<ConstValueInt>(-int_value->getValue(), expression);
        }
    }
    
    // 处理二元表达式
    if (auto binary_expr = std::dynamic_pointer_cast<BinaryExpression>(expression)) {
        auto left = createConstValueFromExpression(binary_expr->lhs);
        auto right = createConstValueFromExpression(binary_expr->rhs);
        auto left_int = std::dynamic_pointer_cast<ConstValueInt>(left);
        auto right_int = std::dynamic_pointer_cast<ConstValueInt>(right);
        
        int result;
        switch (binary_expr->type) {
            case BinaryExpression::PLUS: result = left_int->getValue() + right_int->getValue(); break;
            case BinaryExpression::MINUS: result = left_int->getValue() - right_int->getValue(); break;
            // ... 其他运算符
        }
        return std::make_shared<ConstValueInt>(result, expression);
    }
    
    // 处理括号表达式
    if (auto grouped_expr = std::dynamic_pointer_cast<GroupedExpression>(expression)) {
        return createConstValueFromExpression(grouped_expr->expression);
    }
}
```

#### 使用示例

```rust
const A: i32 = 42;
const B: i32 = -10;
const C: i32 = A + B;           // 32
const D: i32 = (A + B) * 2;      // 64
const E: i32 = A & B;            // 34 (位运算)
const F: i32 = A | B;            // -2 (位运算)
const G: i32 = A ^ B;            // -36 (位运算)

struct Point {
    const ORIGIN_X: i32 = 0,
    const ORIGIN_Y: i32 = 0,
}

const H: i32 = Point::ORIGIN_X + 5;  // 5
```

### ArraySymbol 支持增强

对于 ArraySymbol 单独做处理。类型由两部分组成：基础类型，长度。

到 类型检查 再说吧。
