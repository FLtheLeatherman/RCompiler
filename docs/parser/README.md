# Parser 实现文档

本项目实现了一个 Rust 语言子集的语法分析器（Parser），用于将 lexer 生成的 token 流转换为抽象语法树（AST）。

## 实现方法概述

### 1. 递归下降解析 + Pratt 解析
- **递归下降解析**：用于处理大部分语法结构，如函数定义、结构体、枚举等
- **Pratt 解析**：专门用于处理表达式，通过绑定权力（Binding Power）处理运算符优先级和结合性

### 2. 访问者模式
- 所有 AST 节点都继承自 [`ASTNode`](include/parser/astnode.hpp:8) 基类
- 实现了 [`accept(ASTVisitor*)`](include/parser/astnode.hpp:12) 方法支持访问者模式
- 便于后续的 AST 遍历和语义分析

### 3. 智能指针管理
- 使用 `std::shared_ptr` 管理 AST 节点的生命周期
- 避免内存泄漏，确保节点间的正确引用关系

## 已实现的 AST 节点类型

### 顶层节点
- **[`Crate`](include/parser/astnode.hpp:15)**：表示整个编译单元，包含多个 Item
- **[`Item`](include/parser/astnode.hpp:26)**：顶级项目，可以是函数、结构体、枚举等

### 声明类节点 (Items)
- **[`Function`](include/parser/astnode.hpp:37)**：函数定义，包含 const 修饰符、函数名、参数、返回类型和函数体
- **[`Struct`](include/parser/astnode.hpp:60)**：结构体定义
- **[`Enumeration`](include/parser/astnode.hpp:71)**：枚举定义
- **[`ConstantItem`](include/parser/astnode.hpp:83)**：常量定义
- **[`Trait`](include/parser/astnode.hpp:96)**：特征定义
- **[`Implementation`](include/parser/astnode.hpp:108)**：实现块，包括固有实现和特征实现

### 函数相关节点
- **[`FunctionParameters`](include/parser/astnode.hpp:118)**：函数参数列表
- **[`SelfParam`](include/parser/astnode.hpp:130)**：self 参数
- **[`ShorthandSelf`](include/parser/astnode.hpp:141)**：简写 self 参数（如 `self`, `&self`, `&mut self`）
- **[`TypedSelf`](include/parser/astnode.hpp:153)**：带类型的 self 参数
- **[`FunctionParam`](include/parser/astnode.hpp:165)**：普通函数参数
- **[`FunctionReturnType`](include/parser/astnode.hpp:177)**：函数返回类型

### 结构体相关节点
- **[`StructStruct`](include/parser/astnode.hpp:188)**：结构体主体
- **[`StructFields`](include/parser/astnode.hpp:200)**：结构体字段列表
- **[`StructField`](include/parser/astnode.hpp:211)**：单个结构体字段

### 枚举相关节点
- **[`EnumVariants`](include/parser/astnode.hpp:223)**：枚举变体列表
- **[`EnumVariant`](include/parser/astnode.hpp:234)**：单个枚举变体

### 实现相关节点
- **[`AssociatedItem`](include/parser/astnode.hpp:245)**：关联项（特征中的方法或常量）
- **[`InherentImpl`](include/parser/astnode.hpp:256)**：固有实现
- **[`TraitImpl`](include/parser/astnode.hpp:267)**：特征实现

### 语句类节点
- **[`Statement`](include/parser/astnode.hpp:280)**：语句，可以是 let 语句、表达式语句或 Item
- **[`LetStatement`](include/parser/astnode.hpp:291)**：let 绑定语句
- **[`ExpressionStatement`](include/parser/astnode.hpp:308)**：表达式语句
- **[`Statements`](include/parser/astnode.hpp:320)**：语句序列

### 表达式类节点
#### 基础表达式
- **[`Expression`](include/parser/astnode.hpp:331)**：表达式基类
- **[`ExpressionWithoutBlock`](include/parser/astnode.hpp:343)**：不包含块的表达式
- **[`ExpressionWithBlock`](include/parser/astnode.hpp:356)**：包含块的表达式

#### 字面量表达式
- **[`CharLiteral`](include/parser/astnode.hpp:368)**：字符字面量
- **[`StringLiteral`](include/parser/astnode.hpp:378)**：字符串字面量
- **[`RawStringLiteral`](include/parser/astnode.hpp:388)**：原始字符串字面量
- **[`CStringLiteral`](include/parser/astnode.hpp:398)**：C 风格字符串字面量
- **[`RawCStringLiteral`](include/parser/astnode.hpp:408)**：C 风格原始字符串字面量
- **[`IntegerLiteral`](include/parser/astnode.hpp:418)**：整数字面量
- **[`BoolLiteral`](include/parser/astnode.hpp:428)**：布尔字面量

#### 路径和访问表达式
- **[`PathExpression`](include/parser/astnode.hpp:438)**：路径表达式
- **[`FieldExpression`](include/parser/astnode.hpp:577)**：字段访问表达式

#### 运算符表达式
- **[`UnaryExpression`](include/parser/astnode.hpp:457)**：一元表达式（`-`, `!`, `?`）
- **[`BorrowExpression`](include/parser/astnode.hpp:478)**：借用表达式（`&`, `&&`）
- **[`DereferenceExpression`](include/parser/astnode.hpp:493)**：解引用表达式（`*`）
- **[`BinaryExpression`](include/parser/astnode.hpp:880)**：二元表达式
- **[`AssignmentExpression`](include/parser/astnode.hpp:839)**：赋值表达式
- **[`CompoundAssignmentExpression`](include/parser/astnode.hpp:851)**：复合赋值表达式
- **[`TypeCastExpression`](include/parser/astnode.hpp:917)**：类型转换表达式（`as`）

#### 调用和索引表达式
- **[`CallExpression`](include/parser/astnode.hpp:552)**：函数调用表达式
- **[`MethodCallExpression`](include/parser/astnode.hpp:564)**：方法调用表达式
- **[`IndexExpression`](include/parser/astnode.hpp:528)**：索引表达式

#### 结构体和数组表达式
- **[`StructExpression`](include/parser/astnode.hpp:540)**：结构体实例化表达式
- **[`ArrayExpression`](include/parser/astnode.hpp:517)**：数组表达式
- **[`GroupedExpression`](include/parser/astnode.hpp:506)**：分组表达式（括号表达式）

#### 控制流表达式
- **[`BlockExpression`](include/parser/astnode.hpp:619)**：块表达式
- **[`IfExpression`](include/parser/astnode.hpp:675)**：if 表达式
- **[`LoopExpression`](include/parser/astnode.hpp:630)**：循环表达式
- **[`InfiniteLoopExpression`](include/parser/astnode.hpp:641)**：无限循环表达式（`loop`）
- **[`PredicateLoopExpression`](include/parser/astnode.hpp:652)**：谓词循环表达式（`while`）
- **[`BreakExpression`](include/parser/astnode.hpp:597)**：break 表达式
- **[`ContinueExpression`](include/parser/astnode.hpp:589)**：continue 表达式
- **[`ReturnExpression`](include/parser/astnode.hpp:608)**：return 表达式

#### 辅助表达式节点
- **[`Condition`](include/parser/astnode.hpp:664)**：条件表达式
- **[`ArrayElements`](include/parser/astnode.hpp:780)**：数组元素列表
- **[`StructExprFields`](include/parser/astnode.hpp:792)**：结构体表达式字段列表
- **[`StructExprField`](include/parser/astnode.hpp:803)**：结构体表达式字段
- **[`CallParams`](include/parser/astnode.hpp:815)**：调用参数列表

### 模式类节点
- **[`PatternNoTopAlt`](include/parser/astnode.hpp:692)**：模式基类
- **[`IdentifierPattern`](include/parser/astnode.hpp:700)**：标识符模式
- **[`ReferencePattern`](include/parser/astnode.hpp:713)**：引用模式

### 类型类节点
- **[`Type`](include/parser/astnode.hpp:726)**：类型基类
- **[`ReferenceType`](include/parser/astnode.hpp:737)**：引用类型
- **[`ArrayType`](include/parser/astnode.hpp:749)**：数组类型
- **[`UnitType`](include/parser/astnode.hpp:761)**：单元类型

### 路径类节点
- **[`PathInExpression`](include/parser/astnode.hpp:769)**：表达式中的路径
- **[`PathIdentSegment`](include/parser/astnode.hpp:827)**：路径标识符段

## Pratt 解析实现

### 绑定权力层次
```cpp
PATH_ACCESS = 200,     // ., ::
CALL_INDEX = 190,      // (), []
STRUCT_EXPR = 180,     // {}
UNARY = 170,           // -, *, &, !, ?
TYPE_CAST = 160,       // as
MUL_DIV_MOD = 150,     // *, /, %
ADD_SUB = 140,         // +, -
SHIFT = 130,           // <<, >>
BIT_AND = 120,         // &
BIT_XOR = 110,         // ^
BIT_OR = 100,          // |
COMPARISON = 90,       // ==, !=, <, <=, >, >=
LOGIC_AND = 80,        // &&
LOGIC_OR = 70,         // ||
ASSIGNMENT = 60,       // =, +=, -=, *=, /=, %=, ^=, &=, |=, <<=, >>=, .., ..., ..=
FLOW_CONTROL = 50       // return, break, continue
```

### 解析流程
1. **前缀解析**：[`parsePrattPrefix()`](src/parser/parser.cpp:137) 处理前缀表达式
2. **中缀解析**：在 [`parsePrattExpression()`](src/parser/parser.cpp:228) 中处理中缀运算符
3. **递归上升**：根据绑定权力决定是否继续解析更高优先级的运算符

## 错误处理

- 使用异常机制报告解析错误
- [`match()`](src/parser/parser.cpp:14) 函数在 token 不匹配时抛出 `std::runtime_error`
- 解析失败时会回退到合适的位置并重新尝试其他解析路径

## 使用方法

```cpp
#include "parser/parser.hpp"

// 假设已经有 lexer 生成的 token 流
std::vector<std::pair<Token, std::string>> tokens = /* lexer 输出 */;

Parser parser(std::move(tokens));
auto ast = parser.parseCrate();  // 解析整个编译单元
```

## 特性支持

### 已支持的 Rust 语法特性
- 函数定义（包括 const 函数）
- 结构体定义和实例化
- 枚举定义
- 常量定义
- 特征定义和实现
- 固有实现和特征实现
- 各种表达式（字面量、运算符、调用、索引等）
- 控制流（if、loop、while、break、continue、return）
- 模式匹配（标识符模式、引用模式）
- 类型系统（引用类型、数组类型、单元类型）
- 路径表达式

### 解析器特点
1. **模块化设计**：每个语法结构都有对应的解析函数
2. **类型安全**：强类型的 AST 节点系统
3. **可扩展性**：易于添加新的语法特性
4. **错误恢复**：基本的错误处理和恢复机制
5. **访问者友好**：支持访问者模式进行 AST 遍历

这个 parser 为后续的语义分析和代码生成阶段提供了完整的语法树表示。