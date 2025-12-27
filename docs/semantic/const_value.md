# 常量求值系统文档

## 概述

常量求值系统是 Rust 子集编译器语义分析阶段的重要组成部分，负责在编译时计算常量表达式的值。该系统支持完整的常量表达式语法，包括字面量、算术运算、位运算、路径引用等，为编译器优化和类型检查提供基础支持。

## 系统架构

常量求值系统由以下核心组件构成：

### 1. ConstValue 层次结构
- **位置**: [`include/semantic/const_value.hpp`](include/semantic/const_value.hpp:1)
- **功能**: 表示不同类型的常量值
- **核心类**:
  - `ConstValue`: 常量值基类
  - `ConstValueInt`: 整型常量
  - `ConstValueBool`: 布尔常量
  - `ConstValueChar`: 字符常量
  - `ConstValueString`: 字符串常量
  - `ConstValueStruct`: 结构体常量
  - `ConstValueEnum`: 枚举常量

### 2. ConstEvaluator 求值器
- **位置**: [`include/semantic/const_evaluator.hpp`](include/semantic/const_evaluator.hpp:11)
- **实现**: [`src/semantic/const_evaluator.cpp`](src/semantic/const_evaluator.cpp:1)
- **功能**: 遍历 AST 并计算常量表达式

### 3. 工具函数
- **位置**: [`include/semantic/utils.hpp`](include/semantic/utils.hpp:58)
- **功能**: 提供常量值创建和操作的核心函数

## ConstValue 类型系统

### 基类 ConstValue
- **位置**: [`include/semantic/const_value.hpp:13`](include/semantic/const_value.hpp:13)
- **功能**: 所有常量值的抽象基类
- **核心属性**:
  - `expression_node`: 对应的 AST 节点指针，用于错误定位

#### 核心方法
- `std::shared_ptr<ASTNode> getExpressionNode()`: 获取对应的 AST 节点
- `virtual std::string getValueType() const = 0`: 获取值的类型（纯虚函数）
- `virtual std::string toString() const = 0`: 获取字符串表示（纯虚函数）

#### 类型检查方法
- `virtual bool isInt() const`: 检查是否为整型
- `virtual bool isBool() const`: 检查是否为布尔型
- `virtual bool isChar() const`: 检查是否为字符型
- `virtual bool isString() const`: 检查是否为字符串型
- `virtual bool isStruct() const`: 检查是否为结构体型
- `virtual bool isEnum() const`: 检查是否为枚举型

### 具体常量类型

#### ConstValueInt - 整型常量
- **位置**: [`include/semantic/const_value.hpp:40`](include/semantic/const_value.hpp:40)
- **功能**: 表示整型常量值
- **核心属性**:
  - `value`: 整数值
- **主要方法**:
  - `int getValue() const`: 获取整数值
  - `void setValue(int)`: 设置整数值
  - `std::string getValueType() const override`: 返回 "int"
  - `std::string toString() const override`: 返回值的字符串表示
  - `bool isInt() const override`: 返回 true

#### ConstValueBool - 布尔常量
- **位置**: [`include/semantic/const_value.hpp:57`](include/semantic/const_value.hpp:57)
- **功能**: 表示布尔常量值
- **核心属性**:
  - `value`: 布尔值
- **主要方法**:
  - `bool getValue() const`: 获取布尔值
  - `void setValue(bool)`: 设置布尔值
  - `std::string getValueType() const override`: 返回 "bool"
  - `std::string toString() const override`: 返回 "true" 或 "false"
  - `bool isBool() const override`: 返回 true

#### ConstValueChar - 字符常量
- **位置**: [`include/semantic/const_value.hpp:74`](include/semantic/const_value.hpp:74)
- **功能**: 表示字符常量值
- **核心属性**:
  - `value`: 字符值
- **主要方法**:
  - `char getValue() const`: 获取字符值
  - `void setValue(char)`: 设置字符值
  - `std::string getValueType() const override`: 返回 "char"
  - `std::string toString() const override`: 返回字符的字符串表示
  - `bool isChar() const override`: 返回 true

#### ConstValueString - 字符串常量
- **位置**: [`include/semantic/const_value.hpp:91`](include/semantic/const_value.hpp:91)
- **功能**: 表示字符串常量值
- **核心属性**:
  - `value`: 字符串值
- **主要方法**:
  - `const std::string& getValue() const`: 获取字符串值
  - `void setValue(const std::string&)`: 设置字符串值
  - `std::string getValueType() const override`: 返回 "str"
  - `std::string toString() const override`: 返回字符串值
  - `bool isString() const override`: 返回 true

#### ConstValueStruct - 结构体常量
- **位置**: [`include/semantic/const_value.hpp:108`](include/semantic/const_value.hpp:108)
- **功能**: 表示结构体常量值
- **核心属性**:
  - `struct_name`: 结构体名称
  - `fields`: 字段映射（`std::unordered_map<std::string, std::shared_ptr<ConstValue>>`）

**主要方法**:
- `const std::string& getStructName() const`: 获取结构体名称
- `void setStructName(const std::string&)`: 设置结构体名称
- `void setField(const std::string&, std::shared_ptr<ConstValue>)`: 设置字段值
- `std::shared_ptr<ConstValue> getField(const std::string&)`: 获取字段值
- `bool hasField(const std::string&)`: 检查字段是否存在
- `const std::unordered_map<std::string, std::shared_ptr<ConstValue>>& getFields()`: 获取所有字段
- `std::string getValueType() const override`: 返回结构体类型
- `std::string toString() const override`: 返回结构体的字符串表示
- `bool isStruct() const override`: 返回 true

#### ConstValueEnum - 枚举常量
- **位置**: [`include/semantic/const_value.hpp:132`](include/semantic/const_value.hpp:132)
- **功能**: 表示枚举常量值
- **核心属性**:
  - `enum_name`: 枚举名称
  - `variant_name`: 变体名称

**主要方法**:
- `const std::string& getEnumName() const`: 获取枚举名称
- `void setEnumName(const std::string&)`: 设置枚举名称
- `const std::string& getVariantName() const`: 获取变体名称
- `void setVariantName(const std::string&)`: 设置变体名称
- `std::string getValueType() const override`: 返回枚举类型
- `std::string toString() const override`: 返回枚举的字符串表示
- `bool isEnum() const override`: 返回 true

## 常量求值器

### ConstEvaluator 类
- **位置**: [`include/semantic/const_evaluator.hpp:11`](include/semantic/const_evaluator.hpp:11)
- **功能**: 继承自 `ASTVisitor`，负责遍历 AST 并计算常量表达式

#### 核心成员
- `current_scope`: 当前作用域指针，用于符号查找
- `root_scope`: 根作用域指针

#### 求值流程
1. **初始化**: 创建 `ConstEvaluator` 实例，传入根作用域
2. **遍历**: 调用 `visit()` 方法遍历 AST
3. **求值**: 在遍历过程中计算常量表达式
4. **赋值**: 将计算结果赋值给对应的常量符号

## 核心工具函数

### createConstValueFromExpression
- **位置**: [`include/semantic/utils.hpp:58`](include/semantic/utils.hpp:58)
- **功能**: 从 AST 表达式节点创建对应的 ConstValue 对象
- **签名**: 
```cpp
std::shared_ptr<ConstValue> createConstValueFromExpression(
    std::shared_ptr<Scope> current_scope, 
    std::shared_ptr<ASTNode> expression
)
```

#### 支持的表达式类型

**字面量表达式**:
- `IntegerLiteral`: 转换为 `ConstValueInt`
- `BoolLiteral`: 转换为 `ConstValueBool`
- `CharLiteral`: 转换为 `ConstValueChar`
- `StringLiteral`: 转换为 `ConstValueString`

**路径表达式**:
- `PathExpression`: 解析路径并查找对应的符号
- `PathInExpression`: 支持常量引用和结构体关联常量

**运算符表达式**:
- `UnaryExpression`: 支持负号运算符
- `BinaryExpression`: 支持算术运算和位运算
- `GroupedExpression`: 支持括号表达式

#### 实现细节

**字面量处理**:
```cpp
if (auto int_literal = std::dynamic_pointer_cast<IntegerLiteral>(expression)) {
    try {
        int value = std::stoi(int_literal->value);
        return std::make_shared<ConstValueInt>(value, expression);
    } catch (const std::exception& e) {
        return nullptr; // 转换失败
    }
}
```

**路径表达式处理**:
```cpp
if (auto path_in_expr = std::dynamic_pointer_cast<PathInExpression>(expression)) {
    if (path_in_expr->segment2) {
        // 结构体关联常量: Struct::CONST
        auto struct_identifier = path_in_expr->segment1->identifier;
        auto identifier = path_in_expr->segment2->identifier;
        auto struct_symbol = current_scope->findStructSymbol(struct_identifier);
        if (!struct_symbol) throw std::runtime_error("Const Evaluation Error1");
        auto const_symbol = struct_symbol->getAssociatedConst(identifier);
        if (!const_symbol) throw std::runtime_error("Const Evaluation Error2");
        return const_symbol->getValue();
    } else {
        // 普通常量: CONST
        auto identifier = path_in_expr->segment1->identifier;
        auto const_symbol = current_scope->findConstSymbol(identifier);
        if (!const_symbol) throw std::runtime_error("Const Evaluation Error3");
        return const_symbol->getValue();
    }
}
```

**一元表达式处理**:
```cpp
if (auto unary_expr = std::dynamic_pointer_cast<UnaryExpression>(expression)) {
    if (unary_expr->type == UnaryExpression::MINUS) {
        auto operand_value = createConstValueFromExpression(current_scope, unary_expr->expression);
        if (!operand_value || !operand_value->isInt()) {
            throw std::runtime_error("Const Evaluation Error: Unary minus can only be applied to integer constants");
        }
        auto int_value = std::dynamic_pointer_cast<ConstValueInt>(operand_value);
        return std::make_shared<ConstValueInt>(-int_value->getValue(), expression);
    }
}
```

**二元表达式处理**:
```cpp
if (auto binary_expr = std::dynamic_pointer_cast<BinaryExpression>(expression)) {
    auto left_value = createConstValueFromExpression(current_scope, binary_expr->lhs);
    auto right_value = createConstValueFromExpression(current_scope, binary_expr->rhs);
    
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
    switch (binary_expr->binary_type) {
        case BinaryExpression::PLUS: result = left_int->getValue() + right_int->getValue(); break;
        case BinaryExpression::MINUS: result = left_int->getValue() - right_int->getValue(); break;
        case BinaryExpression::STAR: result = left_int->getValue() * right_int->getValue(); break;
        case BinaryExpression::SLASH: 
            if (right_int->getValue() == 0) {
                throw std::runtime_error("Const Evaluation Error: Division by zero");
            }
            result = left_int->getValue() / right_int->getValue(); 
            break;
        // ... 其他运算符
    }
    return std::make_shared<ConstValueInt>(result, expression);
}
```

## 支持的常量表达式

### 1. 字面量
- **整型字面量**: `42`, `0x1F`, `0b1010`
- **布尔字面量**: `true`, `false`
- **字符字面量**: `'a'`, `'\\n'`
- **字符串字面量**: `"hello"`, `"world\\n"`

### 2. 路径表达式
- **常量引用**: `CONST_NAME`
- **结构体关联常量**: `Struct::CONST_NAME`

### 3. 算术表达式
- **一元运算**: `-expr`
- **二元运算**: `+`, `-`, `*`, `/`, `%`

### 4. 位运算表达式
- **按位运算**: `&`, `|`, `^`
- **位移运算**: `<<`, `>>`

### 5. 括号表达式
- **分组**: `(expr)` 用于改变运算优先级

## 不支持的表达式类型

以下表达式不会出现在常量环境中，会抛出错误：

- **块表达式和所有语句**
- **结构体表达式和字段访问表达式**
- **所有 `ExpressionWithBlock`**（包括 `If`, `While`, `Loop`）
- **借用表达式、解引用表达式**
- **类型转换表达式**
- **逻辑表达式** (`&&`, `||`)
- **比较表达式** (`==`, `!=`, `<`, `>`, `<=`, `>=`)
- **函数调用、方法调用**

## 数组类型处理

### handleArraySymbol 函数
- **位置**: [`include/semantic/utils.hpp:212`](include/semantic/utils.hpp:212)
- **功能**: 处理数组类型的常量求值
- **签名**: `std::string handleArraySymbol(std::shared_ptr<Scope> current_scope, std::shared_ptr<Type> node)`

#### 处理逻辑
1. **引用类型**: 递归处理引用类型的基础类型
2. **基本类型**: 直接返回类型标识符
3. **数组类型**: 计算数组长度并返回完整的数组类型表示

```cpp
if (auto array_type = std::dynamic_pointer_cast<ArrayType>(node->child)) {
    auto expression = array_type->expression;
    auto length = createConstValueFromExpression(current_scope, expression);
    if (!length->isInt()) {
        throw std::runtime_error("Const Evaluation Error: Array length not integer");
    }
    int len = std::dynamic_pointer_cast<ConstValueInt>(length)->getValue();
    return "[" + handleArraySymbol(current_scope, array_type->type) + "]" + std::to_string(len);
}
```

## 错误处理

### 常见错误类型
1. **类型错误**: 运算符应用于不支持的表达式类型
2. **除零错误**: 除法或取模运算的除数为零
3. **符号未找到**: 引用的常量符号不存在
4. **表达式不支持**: 使用了非常量环境支持的表达式

### 错误信息格式
- `"Const Evaluation Error: <详细描述>"`
- `"Const Evaluation Error<number>"`: 用于路径表达式中的特定错误

## 使用示例

### 基本常量定义
```rust
const A: i32 = 42;
const B: bool = true;
const C: char = 'a';
const D: str = "hello";
```

### 算术表达式
```rust
const E: i32 = A + 10;           // 52
const F: i32 = -A;              // -42
const G: i32 = (A + 10) * 2;    // 104
const H: i32 = A / 2;           // 21
const I: i32 = A % 5;           // 2
```

### 位运算表达式
```rust
const J: i32 = A & 0xF;         // 42 & 15 = 10
const K: i32 = A | 0xF;         // 42 | 15 = 47
const L: i32 = A ^ 0xF;         // 42 ^ 15 = 37
const M: i32 = A << 2;         // 42 << 2 = 168
const N: i32 = A >> 2;         // 42 >> 2 = 10
```

### 结构体关联常量
```rust
struct Point {
    const ORIGIN_X: i32 = 0,
    const ORIGIN_Y: i32 = 0,
}

const O: i32 = Point::ORIGIN_X + 5;  // 5
const P: i32 = Point::ORIGIN_Y * 2;  // 0
```

### 数组长度常量
```rust
const SIZE: usize = 10;
const ARRAY: [i32; SIZE] = [0; SIZE];
```

## 性能考虑

### 优化策略
1. **缓存机制**: 对重复的常量表达式进行缓存
2. **延迟求值**: 只在需要时计算常量值
3. **类型检查**: 在求值前进行类型检查，避免无效计算

### 内存管理
- 使用 `std::shared_ptr` 进行内存管理
- 避免不必要的值拷贝
- 及时释放不再使用的常量值

## 扩展指南

### 添加新的常量类型
1. 在 [`const_value.hpp`](include/semantic/const_value.hpp:1) 中定义新的 `ConstValue` 子类
2. 实现所有必需的虚函数
3. 在 `createConstValueFromExpression` 中添加创建逻辑
4. 更新相关的类型检查和求值逻辑

### 支持新的运算符
1. 在 `createConstValueFromExpression` 的二元表达式处理中添加新的 case
2. 实现相应的运算逻辑
3. 添加类型检查和错误处理
4. 编写测试用例验证新运算符

### 扩展表达式支持
1. 在 `createConstValueFromExpression` 中添加新的表达式类型处理
2. 确保新表达式类型符合常量环境的要求
3. 添加相应的错误处理
4. 更新文档说明支持的新表达式类型

## 相关文档

- [符号系统文档](symbol.md): 了解常量符号的定义和管理
- [类型检查器文档](type_checker.md): 了解常量在类型检查中的使用
- [工具函数文档](../utils.md): 了解其他辅助函数的实现

---

常量求值系统为 Rust 子集编译器提供了强大的编译时计算能力，通过精心设计的类型系统和求值算法，确保了常量表达式的正确性和性能。该系统的模块化设计使其易于扩展和维护，为支持更复杂的常量表达式奠定了基础。
