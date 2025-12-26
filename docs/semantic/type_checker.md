# TypeChecker 实现文档

## 概述

TypeChecker 是编译器语义分析阶段的核心类型检查组件，负责遍历整个 AST 并进行类型推断和类型兼容性检查。它确保所有表达式都有正确的类型，并且类型操作符合 Rust 语言的类型系统规则。

## 主要功能

### 1. 类型推断
- 为所有表达式推断类型信息
- 处理 `integer` 类型的自动类型提升
- 支持复合类型的类型推断（数组、结构体等）

### 2. 类型兼容性检查
- 检查赋值操作的类型兼容性
- 验证函数调用的参数类型匹配
- 确保二元表达式的操作数类型兼容

### 3. 变量可变性检查
- 跟踪变量的可变性状态
- 验证可变变量的使用规则
- 支持借用和所有权检查的基础功能

## 实现的核心方法

### 辅助方法

#### `canAssign(SymbolType var_type, SymbolType expr_type)`
- **位置**: [`src/semantic/type_checker.cpp:3`](src/semantic/type_checker.cpp:3)
- **功能**: 检查表达式类型是否可以赋值给变量类型
- **规则**: 
  - 相同类型可以直接赋值
  - 整型变量（i32, u32, isize, usize）可以接受 `integer` 类型的表达式

#### `autoDereference(SymbolType type)`
- **位置**: [`src/semantic/type_checker.cpp:9`](src/semantic/type_checker.cpp:9)
- **功能**: 自动解引用类型，去除引用标记 `&`
- **用途**: 处理借用表达式的类型推断

### 关键的 visit 方法实现

#### 二元表达式类型检查 (`visit(BinaryExpression& node)`)
- **位置**: [`src/semantic/type_checker.cpp:353`](src/semantic/type_checker.cpp:353)
- **功能**: 完整的二元表达式类型推断和检查

**类型匹配规则**:
1. 如果左右操作数类型相同，直接使用该类型
2. 如果一个操作数是 `integer`，另一个是具体整型（i32, u32, isize, usize），则使用具体整型类型
3. 其他情况抛出类型不匹配错误

**运算符类型推断和约束**:
- **算术运算符** (`+`, `-`, `*`, `/`, `%`):
  - 返回操作数类型
  - **约束**:
    - `+` 运算符：支持整数类型和字符串类型
      - 整数 + 整数 → 整数类型
      - 字符串 + 字符串 → 字符串类型
    - `-`, `*`, `/`, `%` 运算符：仅支持整数类型
    - 所有算术运算符：操作数不能为 `bool` 类型
- **位运算符** (`^`, `&`, `|`, `<<`, `>>`):
  - 返回操作数类型
  - **约束**: 操作数不能为 `bool` 类型，必须为整数类型
- **比较运算符** (`==`, `!=`, `>`, `<`, `>=`, `<=`):
  - 返回 `bool` 类型
  - **约束**: 操作数类型必须匹配（支持 integer 类型提升，也支持字符串比较）
- **逻辑运算符** (`&&`, `||`):
  - 返回 `bool` 类型
  - **约束**: 操作数必须为 `bool` 类型

#### 字面量类型推断
- **整数字面量**: 根据后缀推断类型（如 `42u32` → `u32`），否则为 `integer`
- **布尔字面量**: 类型为 `bool`
- **字符字面量**: 类型为 `char`
- **字符串字面量**: 类型为 `str`

#### 函数调用类型检查
- **普通函数调用**: 检查参数数量和类型匹配
- **方法调用**: 处理 self 参数和方法类型验证
- **可变性检查**: 验证可变方法的调用条件

#### 数组类型处理
- **索引表达式**: 验证索引类型为 `integer` 或 `usize`
- **数组创建**: 推断数组元素类型和长度
- **数组访问**: 返回元素类型

#### 结构体和字段访问
- **字段访问**: 验证结构体类型和字段存在性
- **结构体表达式**: 检查字段类型匹配
- **关联函数和方法**: 处理结构体的关联项调用

#### 类型转换表达式类型检查 (`visit(TypeCastExpression& node)`)
- **位置**: [`src/semantic/type_checker.cpp:594`](src/semantic/type_checker.cpp:594)
- **功能**: 类型转换表达式的类型推断和合法性检查

**支持的转换规则**:
1. **Numeric cast**: 整数类型之间的转换
   - 支持 `integer`, `i32`, `u32`, `isize`, `usize` 之间的任意转换
   - 使用 Rust 的二进制补码规则处理负数
2. **Primitive to integer cast**: 基础类型到整数的转换
   - `bool` → 整数: `false` → `0`, `true` → `1`
   - `char` → 整数: 转换为 Unicode 码点值

**错误处理**:
- 不支持的类型转换会抛出 `"Semantic: Invalid type cast from X to Y"` 错误
- 转换表达式的结果类型为目标类型

#### 返回表达式类型检查 (`visit(ReturnExpression& node)`)
- **位置**: [`src/semantic/type_checker.cpp:927`](src/semantic/type_checker.cpp:927)
- **功能**: 返回表达式的类型检查，验证返回值类型与函数返回类型匹配

**类型检查规则**:
1. **函数作用域查找**: 向上遍历作用域链，找到第一个 FUNCTION 类型的作用域
2. **函数符号获取**: 通过作用域的 `self_type` 或全局作用域查找对应的函数符号
3. **返回类型匹配**: 验证返回表达式类型是否与函数声明的返回类型匹配

**匹配规则**:
- 有返回值时：返回表达式类型必须等于函数返回类型
- 无返回值时：函数返回类型必须是 `()`
- 不匹配时：抛出 `"Semantic: Return type mismatch: expected X, got Y"` 错误

**错误处理**:
- 函数符号未找到：抛出相应错误
- 返回类型不匹配：提供清晰的错误信息，指出期望类型和实际类型
- ReturnExpression 的类型始终为 `"!"` (never type)

#### Break 表达式类型检查 (`visit(BreakExpression& node)`)
- **位置**: [`src/semantic/type_checker.cpp:891`](src/semantic/type_checker.cpp:891)
- **功能**: break 表达式的类型检查和类型记录到循环作用域

**类型检查规则**:
1. **循环作用域查找**: 向上遍历作用域链，找到第一个 LOOP 类型的作用域
2. **break 表达式类型**:
   - 有表达式时：使用表达式的类型
   - 无表达式时：使用 `()` 类型
3. **类型记录**: 将 break 表达式类型记录到 LOOP 作用域的 `break_type` 字段
4. **类型唯一性检查**: 确保同一循环中所有 break 表达式的类型兼容

**类型兼容性规则**:
- 相同类型直接兼容
- `integer` 类型与具体整型（i32, u32, isize, usize）兼容
- 当 `integer` 与具体整型同时存在时，优先使用具体整型作为 `break_type`

**错误处理**:
- 不在循环中：抛出 `"Control Flow: Break not in loop"` 错误
- 类型不匹配：抛出 `"Semantic: Break expression type mismatch: expected X, got Y"` 错误
- BreakExpression 的类型始终为 `"!"` (never type)

#### InfiniteLoop 表达式类型检查 (`visit(InfiniteLoopExpression& node)`)
- **位置**: [`src/semantic/type_checker.cpp:860`](src/semantic/type_checker.cpp:860)
- **功能**: 无限循环表达式的类型推断

**类型推断规则**:
1. **作用域遍历**: 进入循环作用域，遍历循环体中的所有语句
2. **break 类型获取**: 从 LOOP 作用域获取 `break_type`
3. **循环类型确定**:
   - 有 break 语句：使用 `break_type` 作为循环类型
   - 无 break 语句：使用 `()` 类型

**类型推断示例**:
```rust
// 类型为 ()
loop { }

// 类型为 i32
loop { break 42; }

// 类型为 String
loop { break "hello".to_string(); }
```

**错误处理**:
- 循环体类型错误：由内部语句的类型检查处理
- break 类型冲突：在 BreakExpression 中处理

#### If 表达式类型检查 (`visit(IfExpression& node)`)
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

**类型推断示例**:
```rust
// 类型为 i32
if condition { 42 } else { 100i32 }

// 类型为 String
if condition { "hello" } else { "world" }

// 类型为 i32（then 分支为 never）
if condition { break; } else { 42 }

// 类型为 !（两个分支都是 never）
if condition { break; } else { return; }
```

**错误处理**:
- 条件不是 bool 类型：抛出 `"Semantic: If condition must be bool type"` 错误
- 分支类型不兼容：抛出 `"Semantic: If expression branches have incompatible types: then branch is X, else branch is Y"` 错误

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
根据 [`include/semantic/utils.hpp:233`](include/semantic/utils.hpp:233) 中定义的 `builtin_types`：
- 基本类型：`bool`, `i32`, `isize`, `u32`, `usize`, `char`, `str`
- 特殊类型：`()`, `self`, `Self`

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
9. **类型转换表达式**: 基础的类型转换检查（整数间转换、bool/char 到整数转换）
10. **循环表达式**: 完整的循环类型推断，包括 break 表达式类型记录和 InfiniteLoopExpression 类型推断
11. **条件表达式**: 完整的 if 表达式类型推断，包括分支类型兼容性检查和 never 类型处理
12. **BlockExpression 尾表达式**: 完整的块表达式尾表达式检测和类型推断，包括 never 类型处理
13. **UnaryExpression**: 完整的一元表达式类型检查，包括算术和逻辑运算符

#### UnaryExpression 类型检查 (`visit(UnaryExpression& node)`)
- **位置**: [`src/semantic/type_checker.cpp:347`](src/semantic/type_checker.cpp:347)
- **功能**: 一元表达式的类型检查和类型推断

**类型检查规则**:
1. **负号运算符 (`-`)**:
   - 可以作用到 integer、i32、u32、isize、usize 类型上
   - 不能作用到 bool、char、str 等其他类型
   - 类型检查错误: `"Semantic: Unary minus operator can only be applied to integer types"`

2. **逻辑非运算符 (`!`)**:
   - 可以作用到 integer、i32、u32、isize、usize 类型上
   - 可以作用到 bool 类型上
   - 不能作用到 char、str 等其他类型
   - 类型检查错误: `"Semantic: Unary logical not operator can only be applied to integer or bool types"`

3. **Try 运算符 (`?`)**:
   - 不支持，抛出错误: `"Semantic: Try operator (?) is not supported"`

**类型推断规则**:
- UnaryExpression 的类型与其成员表达式的类型相同
- 保持原始类型的所有信息（如 integer 保持为 integer，i32 保持为 i32）

**实现细节**:
- 使用 `isIntegerType()` 辅助函数检查整数类型
- 使用 `static_cast<ASTNode&>(node).type` 设置类型（避免与 UnaryExpression::type 枚举冲突）
- 支持所有整数类型的自动类型提升

**类型推断示例**:
```rust
// 类型为 i32（- 作用于 i32）
fn test() -> i32 {
    let x = 5i32;
    -x  // 结果为 i32
}

// 类型为 integer（- 作用于 integer）
fn test() -> integer {
    let x = 5;
    -x  // 结果为 integer
}

// 类型为 bool（! 作用于 bool）
fn test() -> bool {
    let x = true;
    !x  // 结果为 bool
}

// 类型为 i32（! 作用于 i32）
fn test() -> i32 {
    let x = 5i32;
    !x  // 结果为 i32
}
```

#### BlockExpression 尾表达式类型检查 (`visit(BlockExpression& node)`)
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

**实现细节**:
- 进入新的作用域进行类型检查
- 遍历所有语句进行类型推断
- 支持嵌套的 IfExpression 类型检查
- 正确处理 ExpressionStatement 和直接 IfExpression 的情况

**类型推断示例**:
```rust
// 类型为 i32（有尾表达式）
{
    let x = 1;
    x + 1  // 尾表达式
}

// 类型为 ()（无尾表达式）
{
    let x = 1;
    x + 1;  // 有分号，不是尾表达式
}

// 类型为 !（最后一句是 return）
{
    let x = 1;
    return;
}

// 类型为 !（IfExpression 所有分支都是 !）
{
    if condition {
        return;
    } else {
        break;
    }
}
```

### 🔄 部分实现的功能
1. **借用检查**: 基础的引用类型处理，完整的借用检查待实现
2. **所有权系统**: 基础的可变性检查，完整的所有权规则待实现
3. **运算符类型约束**: 基础的运算符类型检查，更完整的约束规则待实现
4. **赋值表达式检查**: 基础的赋值操作类型检查，包括引用解引用和可变性验证

### ❌ 待实现的功能
1. **生命周期检查**: 完整的生命周期分析
2. **trait 系统**: trait 约束和实现检查
3. **泛型支持**: 泛型类型参数和约束
4. **模式匹配**: 模式的类型检查
5. **宏展开**: 宏的类型检查
6. **更完整的运算符重载**: 支持用户定义类型的运算符重载
7. **更完整的类型转换**: 更多类型转换规则的支持

## 使用示例

```cpp
// 创建 TypeChecker 实例
TypeChecker checker(root_scope);

// 遍历 AST 进行类型检查
checker.visit(crate_node);
```

## 设计原则

1. **类型安全**: 严格的类型检查，避免运行时类型错误
2. **Rust 兼容**: 遵循 Rust 语言的类型系统规则
3. **错误友好**: 提供清晰的类型错误信息
4. **性能优化**: 使用高效的类型表示和查找算法
5. **扩展性**: 易于添加新的类型和检查规则

## 相关文档

- [符号系统文档](symbol.md)：符号表和作用域管理
- [常量求值文档](const_value.md)：常量表达式求值
- [结构体检查器文档](struct_checker.md)：结构体相关的类型检查

## 文件结构

```
include/semantic/
├── type_checker.hpp    # TypeChecker 接口定义
├── utils.hpp          # 类型相关的辅助函数
└── ...               # 其他语义分析组件

src/semantic/
├── type_checker.cpp   # TypeChecker 实现
└── ...               # 其他语义分析实现

docs/semantic/
└── type_checker.md   # TypeChecker 文档（本文件）