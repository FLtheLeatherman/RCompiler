# 类型检查器文档

## 概述

TypeChecker 是 Rust 子集编译器语义分析阶段的核心组件，负责在符号收集、常量求值和结构体检查完成后，对整个 AST 进行完整的类型推断和类型兼容性检查。该组件确保所有表达式都有正确的类型，类型操作符合 Rust 语言的类型系统规则，并为后续的代码生成阶段提供类型信息。

## 核心功能

### 1. 类型推断
- **表达式类型推断**: 为所有表达式推断类型信息
- **integer 类型提升**: 处理 `integer` 类型的自动类型提升
- **复合类型推断**: 支持数组、结构体等复合类型的类型推断
- **上下文推断**: 根据使用上下文推断表达式类型

### 2. 类型兼容性检查
- **赋值兼容性**: 检查赋值操作的类型兼容性
- **函数调用验证**: 验证函数调用的参数类型匹配
- **二元表达式检查**: 确保二元表达式的操作数类型兼容
- **类型转换验证**: 检查显式类型转换的合法性

### 3. 变量可变性检查
- **可变性跟踪**: 跟踪变量的可变性状态
- **借用规则验证**: 基础的借用和所有权检查
- **可变访问控制**: 验证可变变量的使用规则

### 4. 控制流类型推断
- **if 表达式**: 推断 if 表达式的类型，处理分支类型兼容性
- **循环表达式**: 处理 loop 表达式的类型推断，包括 break 表达式
- **返回表达式**: 验证返回值类型与函数声明的匹配
- **块表达式**: 处理块表达式的尾表达式类型推断

## TypeChecker 类

### 类定义
- **位置**: [`include/semantic/type_checker.hpp:13`](include/semantic/type_checker.hpp:13)
- **继承**: 继承自 `ASTVisitor`
- **功能**: 遍历 AST 并进行类型检查

### 核心成员
```cpp
class TypeChecker : ASTVisitor {
private:
    std::shared_ptr<Scope> current_scope;  // 当前作用域
    std::shared_ptr<Scope> root_scope;     // 根作用域
    
    // 辅助方法
    bool canAssign(SymbolType var_type, SymbolType expr_type);
    SymbolType autoDereference(SymbolType type);
    bool isIntegerType(const SymbolType& type);
    std::pair<std::string, std::string> getBaseType(const SymbolType& type);
    void checkIntegerOverflow(std::string, int);
    
    int exit_num;  // 用于错误处理
};
```

## 核心辅助方法

### canAssign
- **位置**: [`src/semantic/type_checker.cpp:3`](src/semantic/type_checker.cpp:3)
- **功能**: 检查表达式类型是否可以赋值给变量类型
- **规则**: 
  - 相同类型可以直接赋值
  - 整型变量（i32, u32, isize, usize）可以接受 `integer` 类型的表达式

```cpp
bool TypeChecker::canAssign(SymbolType var_type, SymbolType expr_type) {
    if (var_type == expr_type) {
        return true;
    }
    
    // 整型变量可以接受 integer 类型的表达式
    if ((var_type == "i32" || var_type == "u32" || var_type == "isize" || var_type == "usize") 
        && expr_type == "integer") {
        return true;
    }
    
    return false;
}
```

### autoDereference
- **位置**: [`src/semantic/type_checker.cpp:9`](src/semantic/type_checker.cpp:9)
- **功能**: 自动解引用类型，去除引用标记 `&`
- **用途**: 处理借用表达式的类型推断

```cpp
SymbolType TypeChecker::autoDereference(SymbolType type) {
    if (type.length() > 0 && type[0] == '&') {
        return type.substr(1);
    }
    return type;
}
```

### isIntegerType
- **功能**: 检查类型是否为整数类型
- **支持类型**: `integer`, `i32`, `u32`, `isize`, `usize`

## 关键类型的类型检查

### 二元表达式类型检查

#### visit(BinaryExpression& node)
- **位置**: [`src/semantic/type_checker.cpp:353`](src/semantic/type_checker.cpp:353)
- **功能**: 完整的二元表达式类型推断和检查

**类型匹配规则**:
1. 如果左右操作数类型相同，直接使用该类型
2. 如果一个操作数是 `integer`，另一个是具体整型（i32, u32, isize, usize），则使用具体整型类型
3. 其他情况抛出类型不匹配错误

**运算符类型推断和约束**:

**算术运算符** (`+`, `-`, `*`, `/`, `%`):
- **返回类型**: 操作数类型
- **约束**:
  - `+` 运算符：支持整数类型和字符串类型
    - 整数 + 整数 → 整数类型
    - 字符串 + 字符串 → 字符串类型
  - `-`, `*`, `/`, `%` 运算符：仅支持整数类型
  - 所有算术运算符：操作数不能为 `bool` 类型

**位运算符** (`^`, `&`, `|`, `<<`, `>>`):
- **返回类型**: 操作数类型
- **约束**: 操作数不能为 `bool` 类型，必须为整数类型

**比较运算符** (`==`, `!=`, `>`, `<`, `>=`, `<=`):
- **返回类型**: `bool` 类型
- **约束**: 操作数类型必须匹配（支持 integer 类型提升，也支持字符串比较）

**逻辑运算符** (`&&`, `||`):
- **返回类型**: `bool` 类型
- **约束**: 操作数必须为 `bool` 类型

### 一元表达式类型检查

#### visit(UnaryExpression& node)
- **位置**: [`src/semantic/type_checker.cpp:347`](src/semantic/type_checker.cpp:347)
- **功能**: 一元表达式的类型检查和类型推断

**类型检查规则**:

**负号运算符 (`-`)**:
- **支持类型**: integer、i32、u32、isize、usize
- **不支持类型**: bool、char、str 等
- **错误信息**: `"Semantic: Unary minus operator can only be applied to integer types"`

**逻辑非运算符 (`!`)**:
- **支持类型**: integer、i32、u32、isize、usize、bool
- **不支持类型**: char、str 等
- **错误信息**: `"Semantic: Unary logical not operator can only be applied to integer or bool types"`

**Try 运算符 (`?`)**:
- **支持**: 不支持
- **错误信息**: `"Semantic: Try operator (?) is not supported"`

**类型推断规则**:
- UnaryExpression 的类型与其成员表达式的类型相同
- 保持原始类型的所有信息（如 integer 保持为 integer，i32 保持为 i32）

### 字面量类型推断

- **整数字面量**: 根据后缀推断类型（如 `42u32` → `u32`），否则为 `integer`
- **布尔字面量**: 类型为 `bool`
- **字符字面量**: 类型为 `char`
- **字符串字面量**: 类型为 `str`

### 类型转换表达式类型检查

#### visit(TypeCastExpression& node)
- **位置**: [`src/semantic/type_checker.cpp:594`](src/semantic/type_checker.cpp:594)
- **功能**: 类型转换表达式的类型推断和合法性检查

**支持的转换规则**:

**Numeric cast**: 整数类型之间的转换
- **支持类型**: `integer`, `i32`, `u32`, `isize`, `usize` 之间的任意转换
- **实现**: 使用 Rust 的二进制补码规则处理负数

**Primitive to integer cast**: 基础类型到整数的转换
- `bool` → 整数: `false` → `0`, `true` → `1`
- `char` → 整数: 转换为 Unicode 码点值

**错误处理**:
- **不支持的转换**: `"Semantic: Invalid type cast from X to Y"`
- **结果类型**: 转换表达式的结果类型为目标类型

## 控制流类型检查

### If 表达式类型检查

#### visit(IfExpression& node)
- **位置**: [`src/semantic/type_checker.cpp:842`](src/semantic/type_checker.cpp:842)
- **功能**: if 表达式的类型推断和分支类型兼容性检查

**类型检查规则**:

1. **条件类型检查**: if 条件必须是 `bool` 类型
2. **分支类型推断**: 根据 then 分支和 else 分支的类型推断整个 if 表达式的类型
3. **never 类型处理**: 特殊处理 never 类型（`!`）的情况

**类型推断逻辑**:

1. **两个 never 分支**: 如果 then 和 else 分支都是 `!` 类型，则 if 表达式类型为 `!`
2. **一个 never 分支**:
   - then 分支是 `!`：if 表达式类型为 else 分支类型
   - else 分支是 `!`：if 表达式类型为 then 分支类型
3. **两个非 never 分支**: 需要类型兼容
   - 相同类型：直接使用该类型
   - `integer` 与具体整型：优先使用具体整型类型
   - 类型不兼容：抛出错误

**类型兼容性规则**:
- 相同类型直接兼容
- `integer` 类型与具体整型（i32, u32, isize, usize）兼容
- 当 `integer` 与具体整型同时存在时，优先使用具体整型作为结果类型

**错误处理**:
- **条件错误**: `"Semantic: If condition must be bool type"`
- **分支不兼容**: `"Semantic: If expression branches have incompatible types: then branch is X, else branch is Y"`

### 循环表达式类型检查

#### InfiniteLoop 表达式类型检查
- **位置**: [`src/semantic/type_checker.cpp:860`](src/semantic/type_checker.cpp:860)
- **功能**: 无限循环表达式的类型推断

**类型推断规则**:
1. **作用域遍历**: 进入循环作用域，遍历循环体中的所有语句
2. **break 类型获取**: 从 LOOP 作用域获取 `break_type`
3. **循环类型确定**:
   - 有 break 语句：使用 `break_type` 作为循环类型
   - 无 break 语句：使用 `()` 类型

#### Break 表达式类型检查
- **位置**: [`src/semantic/type_checker.cpp:891`](src/semantic/type_checker.cpp:891)
- **功能**: break 表达式的类型检查和类型记录到循环作用域

**类型检查规则**:
1. **循环作用域查找**: 向上遍历作用域链，找到第一个 LOOP 类型的作用域
2. **break 表达式类型**:
   - 有表达式时：使用表达式的类型
   - 无表达式时：使用 `()` 类型
3. **类型记录**: 将 break 表达式类型记录到 LOOP 作用域的 `break_type` 字段
4. **类型唯一性检查**: 确保同一循环中所有 break 表达式的类型兼容

**错误处理**:
- **不在循环中**: `"Control Flow: Break not in loop"`
- **类型不匹配**: `"Semantic: Break expression type mismatch: expected X, got Y"`

### 返回表达式类型检查

#### visit(ReturnExpression& node)
- **位置**: [`src/semantic/type_checker.cpp:927`](src/semantic/type_checker.cpp:927)
- **功能**: 返回表达式的类型检查，验证返回值类型与函数返回类型匹配

**类型检查规则**:
1. **函数作用域查找**: 向上遍历作用域链，找到第一个 FUNCTION 类型的作用域
2. **函数符号获取**: 通过作用域的 `self_type` 或全局作用域查找对应的函数符号
3. **返回类型匹配**: 验证返回表达式类型是否与函数声明的返回类型匹配

**匹配规则**:
- **有返回值时**: 返回表达式类型必须等于函数返回类型
- **无返回值时**: 函数返回类型必须是 `()`
- **不匹配时**: 抛出 `"Semantic: Return type mismatch: expected X, got Y"` 错误

### 块表达式类型检查

#### visit(BlockExpression& node)
- **位置**: [`src/semantic/type_checker.cpp:816`](src/semantic/type_checker.cpp:816)
- **功能**: 块表达式的尾表达式检测和类型推断

**尾表达式检测规则**:
1. **尾表达式定义**: Statements 末尾的 ExpressionWithoutBlock 和不带分号的 ExpressionWithBlock
2. **检测逻辑**: 从最后一个语句开始向前查找，找到第一个没有分号的 ExpressionStatement
3. **分号判断**: 通过 `ExpressionStatement::has_semi` 字段判断是否有分号

**类型推断规则**:
1. **有尾表达式**: 使用尾表达式的类型作为块表达式的类型
2. **无尾表达式**:
   - 如果最后一句是 break | continue | return，类型为 `!`
   - 如果包含【每个分支都是 ! 类型】的 IfExpression，类型为 `!`
   - 否则类型为 `()`

## 函数调用类型检查

### 普通函数调用
- **参数数量检查**: 验证调用参数数量与函数声明匹配
- **参数类型检查**: 验证每个参数的类型兼容性
- **返回类型**: 使用函数声明的返回类型

### 方法调用
- **self 参数验证**: 检查方法调用的接收者类型
- **可变性检查**: 验证可变方法的调用条件
- **方法类型识别**: 支持 `self`, `&self`, `&mut self`, `mut self` 等形式

## 数组类型处理

### 索引表达式
- **索引类型**: 验证索引类型为 `integer` 或 `usize`
- **数组类型**: 确保索引表达式的基类型是数组
- **结果类型**: 返回数组元素类型

### 数组创建
- **元素类型推断**: 推断数组元素类型
- **长度检查**: 验证数组长度为常量表达式
- **类型表示**: 生成完整的数组类型表示

## 结构体和字段访问

### 字段访问
- **结构体类型**: 验证字段访问表达式的基类型是结构体
- **字段存在性**: 检查字段是否在结构体中定义
- **字段类型**: 返回字段的声明类型

### 结构体表达式
- **字段匹配**: 检查结构体表达式中的字段与定义匹配
- **字段类型**: 验证字段值的类型与字段声明类型兼容
- **结果类型**: 返回结构体类型

### 关联函数和方法
- **关联函数调用**: 处理结构体的关联函数调用
- **方法调用**: 处理结构体的方法调用，包括 self 参数处理

## 作用域管理

TypeChecker 遵循与 SymbolCollector 相同的作用域遍历模式：

```cpp
// 进入新作用域
auto prev_scope = current_scope;
current_scope = current_scope->getChild();

// 进行类型检查...

// 返回原作用域
current_scope = prev_scope;
current_scope->nextChild();
```

### 作用域创建规则
- **BlockExpression**: 创建块作用域，处理局部变量
- **Function**: 创建函数作用域，处理参数和局部变量
- **Trait**: 创建特征作用域
- **Implementation**: 创建实现作用域
- **LoopExpression**: 创建循环作用域，处理 break/continue

## 类型系统特性

### 内置类型支持
根据 [`include/semantic/utils.hpp:235`](include/semantic/utils.hpp:235) 中定义的 `builtin_types`：
- **基本类型**: `bool`, `i32`, `isize`, `u32`, `usize`, `char`, `str`
- **特殊类型**: `()`, `self`, `Self`

### 复合类型支持
- **引用类型**: `&T` 和 `&mut T`
- **数组类型**: `[T; N]` 其中 N 为编译时常量
- **结构体类型**: 用户定义的结构体
- **枚举类型**: 用户定义的枚举

### 类型推断策略
1. **字面量推断**: 整数字面量默认为 `integer`，支持类型后缀
2. **上下文推断**: 根据使用上下文推断 `integer` 的具体类型
3. **自动类型提升**: `integer` 可以提升为任何具体整型
4. **类型兼容性**: 严格的类型检查，禁止隐式类型转换（除 integer 提升）

## 错误处理

TypeChecker 在遇到类型错误时会抛出 `std::runtime_error`，包含具体的错误信息：

### 常见错误类型
- `"Semantic: Type Error in LetStmt"`: let 语句类型不匹配
- `"Semantic: BinaryExpression type not match"`: 二元表达式操作数类型不匹配
- `"Semantic: CallExpr function param number not match"`: 函数调用参数数量不匹配
- `"Semantic: CallExpr function param type not match"`: 函数调用参数类型不匹配
- `"Semantic: FieldExpr struct not found"`: 字段访问时结构体未找到
- `"Semantic: FieldExpr field not found"`: 字段不存在
- `"Semantic: IndexExpr not array"`: 索引表达式的基类型不是数组
- `"Semantic: IndexExpr index not usize"`: 数组索引类型错误

## 当前实现状态

### ✅ 已完成的功能
1. **基础类型推断**: 所有基本字面量的类型推断
2. **二元表达式**: 完整的运算符类型推断和检查
3. **函数调用**: 参数类型和数量验证
4. **方法调用**: 包括可变性检查
5. **数组操作**: 索引和数组创建的类型检查
6. **结构体操作**: 字段访问和结构体表达式
7. **作用域管理**: 正确的作用域遍历和变量查找
8. **类型兼容性**: 赋值和参数传递的类型检查
9. **类型转换表达式**: 基础的类型转换检查
10. **循环表达式**: 完整的循环类型推断
11. **条件表达式**: 完整的 if 表达式类型推断
12. **块表达式**: 尾表达式检测和类型推断
13. **一元表达式**: 完整的一元表达式类型检查

### 🔄 部分实现的功能
1. **借用检查**: 基础的引用类型处理
2. **所有权系统**: 基础的可变性检查
3. **运算符类型约束**: 基础的运算符类型检查
4. **赋值表达式检查**: 基础的赋值操作类型检查

### ❌ 待实现的功能
1. **生命周期检查**: 完整的生命周期分析
2. **trait 系统**: trait 约束和实现检查
3. **泛型支持**: 泛型类型参数和约束
4. **模式匹配**: 模式的类型检查
5. **宏展开**: 宏的类型检查
6. **更完整的运算符重载**: 支持用户定义类型的运算符重载
7. **更完整的类型转换**: 更多类型转换规则的支持

## 使用示例

### 基本使用
```cpp
// 创建 TypeChecker 实例
TypeChecker checker(root_scope);

// 遍历 AST 进行类型检查
checker.visit(crate_node);
```

### 类型检查示例
```rust
// 基础类型推断
let x = 42;           // x: integer
let y: i32 = x;       // OK: integer → i32
let z: i32 = 3.14;    // Error: 类型不匹配

// 二元表达式
let a = 5 + 3;         // a: integer
let b: i32 = 5 + 3;    // b: i32
let c = "hello" + " world";  // c: str
let d = 5 + "hello";   // Error: 类型不匹配

// 函数调用
fn add(x: i32, y: i32) -> i32 { x + y }
let result = add(5, 3); // OK
let error = add("5", 3); // Error: 参数类型不匹配

// if 表达式
let x = if condition { 42 } else { 100 };      // x: integer
let y: i32 = if condition { 42 } else { 100i32 }; // y: i32
let z = if condition { "hello" } else { 42 };   // Error: 分支类型不兼容

// 循环表达式
let a = loop { break 42; };     // a: i32
let b = loop { break; };        // b: ()
let c = loop { };               // c: ()
```

## 性能考虑

### 优化策略
1. **类型缓存**: 缓存已推断的类型信息
2. **早期退出**: 在发现类型错误时尽早停止检查
3. **最小化遍历**: 只遍历需要类型检查的节点

### 内存管理
- **共享指针**: 使用 `std::shared_ptr` 管理作用域和符号
- **类型字符串**: 使用字符串表示类型，便于比较和调试
- **避免拷贝**: 通过引用传递大型对象

## 扩展指南

### 添加新的类型检查规则
1. 在相应的 visit 方法中添加检查逻辑
2. 实现类型推断和兼容性检查
3. 添加必要的错误处理和诊断信息
4. 编写测试用例验证新规则

### 扩展类型系统
1. 在 `builtin_types` 中添加新的内置类型
2. 扩展 `isIntegerType()` 等辅助函数
3. 更新类型推断逻辑以支持新类型
4. 确保与现有系统的兼容性

### 改进错误报告
1. 添加更详细的类型错误信息
2. 提供类型不匹配的具体位置
3. 添加类型修复建议
4. 支持错误恢复机制

## 相关文档

- [符号系统文档](symbol.md): 了解符号类型和作用域管理
- [常量求值文档](const_value.md): 了解常量表达式求值
- [结构体检查器文档](struct_checker.md): 了解类型存在性检查
- [工具函数文档](../utils.md): 了解辅助函数的实现

## 注意事项

1. **依赖关系**: 确保在调用 TypeChecker 之前，SymbolCollector、ConstEvaluator 和 StructChecker 都已完成
2. **作用域正确性**: TypeChecker 依赖于正确的作用域树结构
3. **类型完整性**: 所有类型检查都基于符号表中已定义的类型信息
4. **错误处理**: 确保类型错误信息清晰且有助于调试

---

TypeChecker 为 Rust 子集编译器提供了完整的类型检查功能，通过系统化的类型推断和兼容性检查，确保了代码的类型安全性。该组件与符号系统、作用域管理和常量求值系统紧密集成，为后续的代码生成阶段提供了坚实的类型基础。其模块化设计和清晰的接口使其易于扩展和维护，为支持更复杂的类型系统特性奠定了基础。