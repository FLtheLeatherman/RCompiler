# RCompiler 当前进度总结

## 项目概述
RCompiler 是一个 Rust 语言子集的 C++ 编译器实现。当前目标是完成词法分析器（lexer）和语法分析器（parser），将词法分析好的 token 转换为抽象语法树（AST）。

## 已完成内容

### 1. 词法分析器（Lexer）
根据 README.md 描述，lexer 已经完成，能够将源代码转换为 token 流。

### 2. AST 节点定义（include/astnode.hpp）
已定义完整的 AST 节点类层次结构，采用访问者模式设计：

#### 顶层节点
- `ASTNode`: 所有 AST 节点的基类，包含纯虚函数 `accept(ASTVisitor*)`
- `Crate`: 表示整个 Rust crate，包含多个 `Item`

#### 声明类型节点
- `Item`: 通用项目容器，可包含 Function、Struct、Enumeration、ConstantItem、Trait、Implementation
- `Function`: 函数定义，支持 const 函数，包含参数、返回类型和函数体
- `Struct`: 结构体定义
- `Enumeration`: 枚举定义
- `ConstantItem`: 常量项定义
- `Trait`: trait 定义
- `Implementation`: 实现，分为 InherentImpl 和 TraitImpl

#### 函数相关节点
- `FunctionParameters`: 函数参数列表
- `SelfParam`: self 参数，支持 ShorthandSelf 和 TypedSelf
- `ShorthandSelf`: 简写 self 参数（如 `&self`, `&mut self`, `self`, `mut self`）
- `TypedSelf`: 类型化 self 参数（如 `self: Type`）
- `FunctionParam`: 普通函数参数
- `FunctionReturnType`: 函数返回类型

#### 结构体相关节点
- `StructStruct`: 结构体具体定义
- `StructFields`: 结构体字段列表
- `StructField`: 单个结构体字段

#### 枚举相关节点
- `EnumVariants`: 枚举变体列表
- `EnumVariant`: 单个枚举变体

#### Trait 和实现相关节点
- `AssociatedItem`: 关联项，可包含 ConstantItem 或 Function
- `InherentImpl`: 固有实现
- `TraitImpl`: trait 实现

#### 语句和表达式节点
- `Statement`: 语句，可包含 Item、LetStatement、ExpressionStatement 或为空
- `LetStatement`: let 语句
- `ExpressionStatement`: 表达式语句
- `Statements`: 语句列表，包含 Statement+ 或 Statement+ ExpressionWithoutBlock 或 ExpressionWithoutBlock
- `Expression`: 表达式，分为 ExpressionWithoutBlock 和 ExpressionWithBlock
- `ExpressionWithoutBlock`: 无块表达式
- `ExpressionWithBlock`: 有块表达式

#### 字面量表达式节点
- `CharLiteral`: 字符字面量，包含字符串值（继承 Expression）
- `StringLiteral`: 字符串字面量，包含字符串值（继承 Expression）
- `RawStringLiteral`: 原始字符串字面量，包含字符串值（继承 Expression）
- `CStringLiteral`: C 字符串字面量，包含字符串值（继承 Expression）
- `RawCStringLiteral`: 原始 C 字符串字面量，包含字符串值（继承 Expression）
- `IntegerLiteral`: 整数字面量，包含字符串值（继承 Expression）
- `BoolLiteral`: 布尔字面量，包含布尔值（继承 Expression）

#### 路径和字段访问节点
- `PathExpression`: 路径表达式（已实现）
- `OperatorExpression`: 操作符表达式（已实现）
- `GroupedExpression`: 分组表达式（已实现，包含 Expression 成员变量）
- `ArrayExpression`: 数组表达式（已实现，包含 ArrayElements 成员变量）
- `ArrayElements`: 数组元素列表，包含表达式向量和分隔类型标志
- `IndexExpression`: 索引表达式（已实现）
- `StructExpression`: 结构体表达式（已实现）
- `StructExprFields`: 结构体字段列表，包含 StructExprField 向量
- `StructExprField`: 结构体字段，包含标识符和表达式
- `CallExpression`: 函数调用表达式（已实现）
- `CallParams`: 函数调用参数列表，包含表达式向量
- `MethodCallExpression`: 方法调用表达式（已实现）
- `FieldExpression`: 字段访问表达式（已实现）

#### 控制流表达式节点
- `ContinueExpression`: continue 表达式（已实现）
- `BreakExpression`: break 表达式（已实现，包含可选的表达式子节点）
- `ReturnExpression`: return 表达式（已实现，包含可选的表达式子节点）
- `BlockExpression`: 块表达式（已实现，包含可选的 Statements）
- `LoopExpression`: 循环表达式（已实现，包含 InfiniteLoopExpression 或 PredicateLoopExpression 子节点）
- `InfiniteLoopExpression`: 无限循环表达式（已实现，包含 BlockExpression）
- `PredicateLoopExpression`: 条件循环表达式（已实现，包含条件和 BlockExpression）
- `Condition`: 条件表达式（已实现，包含表达式（不能是 StructExpression））
- `IfExpression`: if 表达式（已实现，包含条件、then 块和可选的 else 分支）

#### 一元和类型转换表达式节点
- `UnaryExpression`: 一元表达式（已实现）
  - 包含 UnaryType 枚举（MINUS, NOT, TRY）
- `BorrowExpression`: 借用表达式（已实现）
  - 支持单借用（&）和双借用（&&），以及可变性（mut）
- `DereferenceExpression`: 解引用表达式（已实现）

#### 中缀表达式节点
- `AssignmentExpression`: 赋值表达式（已实现）
  - 包含 lhs 和 rhs 成员变量，支持 `lhs = rhs` 语法
- `CompoundAssignmentExpression`: 复合赋值表达式（已实现）
  - 包含 CompoundAssignmentType 枚举，支持 `+=`, `-=`, `*=`, `/=`, `%=`, `^=`, `&=`, `|=`, `<<=`, `>>=` 运算符
- `BinaryExpression`: 二元表达式（已实现）
  - 包含 BinaryType 枚举，支持算术、位运算、比较和逻辑运算符
- `TypeCastExpression`: 类型转换表达式（已实现）
  - 包含 expression 和 type 成员变量，支持 `expression as type` 语法

#### 模式和类型节点
- `PatternNoTopAlt`: 模式（已实现，支持 IdentifierPattern 和 ReferencePattern）
- `IdentifierPattern`: 标识符模式（已实现）
  - 支持引用（ref）和可变性（mut）标志
- `ReferencePattern`: 引用模式（已实现）
  - 支持单引用（&）、双引用（&&）和可变性（mut）
- `Type`: 类型，可包含 TypePath、ReferenceType、ArrayType、UnitType（已实现）
- `ReferenceType`: 引用类型（已实现）
  - 支持可变性标志（mut）和目标类型
- `ArrayType`: 数组类型（已实现）
  - 包含元素类型和大小表达式
- `UnitType`: 单元类型（已实现）

#### 路径相关节点
- `PathInExpression`: 表达式中的路径（已实现）
- `PathIdentSegment`: 路径标识符段，支持标识符、Self 和 self（已实现）

### 3. 语法分析器接口定义（include/parser.hpp）
Parser 类已定义完整的解析函数接口：

#### 基础工具函数
- `peek()`: 查看当前 token
- `get_string()`: 获取当前 token 的字符串值
- `consume()`: 消费当前 token
- `match(Token)`: 匹配指定 token

#### Pratt parsing 支持
- `BindingPower` 枚举：定义了从 PATH_ACCESS (200) 到 FLOW_CONTROL (50) 的完整绑定力优先级
- `getTokenLeftBP()`: 获取当前运算符的左绑定力
- `getTokenRightBP()`: 获取当前运算符的右绑定力（对于左结合运算符返回左绑定力+1）
- `getTokenUnaryBP()`: 获取一元运算符的绑定力
- `parsePrattExpression(int current_bp)`: Pratt parsing 主函数（已实现）
- `parsePrattPrefix()`: Pratt parsing 前缀解析函数（已实现）

#### 主要解析函数（已实现）
- `parseCrate()`: 解析整个 crate
- `parseItem()`: 解析项目
- `parseFunction()`: 解析函数，支持 const 函数、函数名、参数列表、返回类型和函数体
- `parseStruct()`: 解析结构体
- `parseEnumeration()`: 解析枚举
- `parseConstantItem()`: 解析常量项
- `parseTrait()`: 解析 trait
- `parseImplementation()`: 解析实现
- `parseFunctionParameters()`: 解析函数参数
- `parseSelfParam()`: 解析 self 参数
- `parseShorthandSelf()`: 解析简写 self
- `parseTypedSelf()`: 解析类型化 self
- `parseFunctionParam()`: 解析普通函数参数
- `parseFunctionReturnType()`: 解析函数返回类型
- `parseStructStruct()`: 解析结构体定义
- `parseStructFields()`: 解析结构体字段
- `parseStructField()`: 解析单个结构体字段
- `parseEnumVariants()`: 解析枚举变体列表
- `parseEnumVariant()`: 解析单个枚举变体
- `parseAssociatedItem()`: 解析关联项
- `parseInherentImpl()`: 解析固有实现
- `parseTraitImpl()`: 解析 trait 实现

#### 语句和表达式解析函数（已实现）
- `parseStatement()`: 解析语句
- `parseLetStatement()`: 解析 let 语句
- `parseExpressionStatement()`: 解析表达式语句
- `parseStatements()`: 解析语句列表
- `parseExpression()`: 解析表达式
- `parseExpressionWithoutBlock()`: 解析无块表达式（已实现，检查不能包含 ExpressionWithBlock）
- `parseExpressionWithBlock()`: 解析有块表达式
- `parseBlockExpression()`: 解析块表达式，支持 `{ Statements? }` 语法

#### 字面量解析函数（已实现）
- `parseCharLiteral()`: 解析字符字面量
- `parseStringLiteral()`: 解析字符串字面量
- `parseRawStringLiteral()`: 解析原始字符串字面量
- `parseCStringLiteral()`: 解析 C 字符串字面量
- `parseRawCStringLiteral()`: 解析原始 C 字符串字面量
- `parseIntegerLiteral()`: 解析整数字面量
- `parseBoolLiteral()`: 解析布尔字面量

#### 控制流表达式解析函数（已实现）
- `parseReturnExpression()`: 解析 return 表达式
- `parseIfExpression()`: 解析 if 表达式
- `parseCondition()`: 解析条件表达式（检查不能是 StructExpression）
- `parseLoopExpression()`: 解析循环表达式
- `parseInfiniteLoopExpression()`: 解析无限循环表达式
- `parsePredicateLoopExpression()`: 解析条件循环表达式
- `parseBreakExpression()`: 解析 break 表达式
- `parseContinueExpression()`: 解析 continue 表达式

#### 复合表达式解析函数（已实现）
- `parseGroupedExpression()`: 解析分组表达式，支持 `( Expression )` 语法
- `parseArrayExpression()`: 解析数组表达式，支持 `[ ArrayElements? ]` 语法
- `parseArrayElements()`: 解析数组元素，支持两种语法：
  - `Expression ; Expression`（分号分隔）
  - `Expression ( , Expression )* ,?`（逗号分隔）
- `parseIndexExpression()`: 解析索引表达式，支持 `Expression [ Expression ]` 语法
- `parseStructExpression()`: 解析结构体表达式，支持 `PathInExpression { StructExprFields? }` 语法
- `parseStructExprFields()`: 解析结构体字段列表，支持 `StructExprField ( , StructExprField )* ,?` 语法
- `parseStructExprField()`: 解析结构体字段，支持 `IDENTIFIER : Expression` 语法
- `parseCallExpression()`: 解析函数调用表达式，支持 `Expression ( CallParams? )` 语法
- `parseCallParams()`: 解析函数调用参数，支持 `Expression ( , Expression )* ,?` 语法
- `parseMethodCallExpression()`: 解析方法调用表达式，支持 `Expression . PathIdentSegment ( CallParams? )` 语法
- `parseFieldExpression()`: 解析字段访问表达式，支持 `Expression . IDENTIFIER` 语法
- `parsePathExpression()`: 解析路径表达式
- `parsePathInExpression()`: 解析表达式中的路径
- `parsePathIdentSegment()`: 解析路径标识符段

#### 一元表达式解析函数（已实现）
- `parseUnaryExpression()`: 解析一元表达式，支持 `-`, `!`, `?` 运算符
- `parseBorrowExpression()`: 解析借用表达式，支持 `&`, `&&`, `mut` 关键字
- `parseDereferenceExpression()`: 解析解引用表达式，支持 `*` 运算符

#### 中缀表达式解析函数（已实现）
- `parseAssignmentExpression()`: 解析赋值表达式，支持传入 lhs 和 rhs
- `parseCompoundAssignmentExpression()`: 解析复合赋值表达式，支持类型和 lhs、rhs 参数
- `parseBinaryExpression()`: 解析二元表达式，支持类型和 lhs、rhs 参数
- `parseTypeCastExpression()`: 解析类型转换表达式，支持表达式和类型参数
- **新增的中缀表达式解析函数**：
  - `parseCallExpressionFromInfix()`: 解析函数调用的中缀版本（传入 lhs）
  - `parseMethodCallExpressionFromInfix()`: 解析方法调用的中缀版本（传入 lhs）
  - `parseFieldExpressionFromInfix()`: 解析字段访问的中缀版本（传入 lhs）
  - `parseIndexExpressionFromInfix()`: 解析索引访问的中缀版本（传入 lhs）

#### 模式和类型解析函数
- `parsePatternNoTopAlt()`: 解析模式（已实现，支持 IdentifierPattern 和 ReferencePattern）
- `parseIdentifierPattern()`: 解析标识符模式（已实现，支持 `ref`? `mut`? IDENTIFIER 语法）
- `parseReferencePattern()`: 解析引用模式（已实现，支持 ( `&` | `&&` ) `mut`? PatternNoTopAlt 语法）
- `parseType()`: 解析类型（已实现，按顺序尝试解析 PathIdentSegment、ReferenceType、ArrayType、UnitType）
- `parseReferenceType()`: 解析引用类型（已实现，支持 `&` `mut`? Type 语法）
- `parseArrayType()`: 解析数组类型（已实现，支持 `[` Type `;` Expression `]` 语法）
- `parseUnitType()`: 解析单元类型（已实现，支持 `(` `)` 语法）

### 4. 语法分析器实现（src/parser.cpp）
已实现大部分解析功能：

#### 已完整实现的功能
1. **基础工具函数**：peek、get_string、consume、match
2. **Pratt parsing 系统**：
   - 完整的绑定力系统，支持所有 Rust 运算符的优先级
   - `parsePrattPrefix()` 函数，支持所有前缀表达式类型
   - `parsePrattExpression()` 函数，完整集成了所有中缀表达式处理：
     - 函数调用：`lhs(...)`
     - 索引访问：`lhs[index]`
     - 方法调用和字段访问：`lhs.method(...)` 和 `lhs.field`
     - 类型转换：`lhs as type`
     - 赋值运算：`lhs = rhs`
     - 复合赋值：`lhs op= rhs`（支持所有复合赋值运算符）
     - 二元运算：`lhs op rhs`（支持所有算术、位运算、比较和逻辑运算符）
   - 修正了 token 消耗逻辑，确保不同类型的中缀表达式使用正确的解析策略

3. **顶层解析**：完整的 crate 和项目解析
4. **函数解析**：支持 const 函数、完整参数列表、返回类型和函数体
5. **结构体和枚举解析**：支持完整的定义语法
6. **语句解析**：支持 let 语句、表达式语句和语句列表
7. **表达式解析**：支持所有字面量、控制流、复合表达式
8. **路径解析**：支持完整的路径语法
9. **一元表达式解析**：支持所有一元运算符
10. **错误处理**：使用异常机制，提供清晰的错误信息

#### 实现特点
- 使用递归下降解析方法
- 支持错误处理，遇到解析错误会抛出异常
- 使用智能指针管理内存
- 支持可选语法元素的解析
- 模块化设计，分离头文件和实现文件
- 支持 C++17 标准

### 5. 访问者模式基础（include/visitor.hpp）
已定义访问者模式基础结构：
- `ASTVisitor` 基类，包含纯虚函数 `visit(ASTNode&)`
- 所有 AST 节点都继承自 `ASTNode` 并实现 `accept` 方法

### 6. 工具文件（include/utils.hpp）
已定义所有 AST 节点类的前向声明，确保编译通过

### 7. 主程序（src/main.cpp）
简单的主程序，用于测试 lexer 和 parser 的集成

## 项目架构
- 采用访问者模式设计 AST
- 使用智能指针（shared_ptr）管理节点生命周期
- 模块化设计，分离头文件和实现文件
- 支持 C++17 标准
- 完整的 Rust 语法子集支持

## 当前限制
1. **表达式解析已完成**：ExpressionWithoutBlock 和 ExpressionWithBlock 的解析已完全实现，包含类型检查
2. **类型解析已完成**：parseType 函数已实现，支持 PathIdentSegment、ReferenceType、ArrayType、UnitType
3. **模式解析已完成**：parsePatternNoTopAlt 函数已实现，支持 IdentifierPattern 和 ReferencePattern
4. **访问者模式未实现**：需要实现具体的访问者类（如 ASTPrinter、TypeChecker、CodeGenerator）

## 下一步工作
1. **完善类型系统**：
   - [x] 实现 TypePath 解析（简单路径和泛型参数）
   - [x] 实现 ReferenceType 解析（引用类型 &T, &mut T）
   - [x] 实现 ArrayType 解析（数组类型 [T; N]）
   - [x] 完善 UnitType 解析（单元类型 ()）

2. **完善模式系统**：
   - 实现字面量模式（字面量）
   - 实现标识符模式（变量绑定）
   - 实现通配符模式（_）
   - 实现结构体模式
   - 实现枚举模式
   - 实现或模式（|）
   - 实现范围模式（..）

3. **完善名称系统**：
   - 实现 namespace 解析（use 语句）
   - 实现 scope 解析（作用域和可见性）
   - 实现 preludes 解析（外部 crate 导入）
   - 完善路径解析，支持绝对路径、相对路径、泛型参数等

4. **实现访问者模式**：
   - 设计并实现 ASTPrinter：用于打印 AST 结构
   - 设计并实现 TypeChecker：用于类型检查和推断
   - 设计并实现 CodeGenerator：用于生成中间表示或目标代码

5. **错误处理改进**：
   - 实现错误恢复机制
   - 添加错误位置信息
   - 实现警告系统
   - 提供更友好的错误消息

6. **测试和验证**：
   - 编写单元测试
   - 编写集成测试
   - 添加基准测试
   - 创建测试用例集合

## 技术亮点
- 完整的 Rust 语法子集支持
- 清晰的 AST 节点层次结构
- 高效的 Pratt parsing 表达式解析系统
- 模块化的解析器设计
- 良好的内存管理策略
- 完整的运算符优先级处理
- 支持复杂的嵌套表达式解析

这个项目为 Rust 编译器的前端开发奠定了坚实的基础，已经具备了处理复杂 Rust 代码的能力。