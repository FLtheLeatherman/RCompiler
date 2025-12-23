# StructChecker 文档

## 概述

StructChecker 是编译器语义分析阶段的完整类型检查组件，负责遍历整个 AST 并进行类型存在性检查。它检查所有用到类型的地方，确保类型存在且可见，同时处理 impl 块与 struct 的集成。

## 主要功能

### 1. 类型存在性检查
- 检查 struct 中的字段类型
- 检查 impl/trait 中的关联函数参数和返回值类型
- 检查关联常量的类型
- 检查任何用到类型的地方，确保类型存在且可见

### 2. Impl 块处理
- **InherentImpl**: 寻找对应的 struct 类型，在 struct 中添加关联项
- **TraitImpl**: 寻找对应的 struct 类型和 trait，检查 trait 中未定义的关联项是否全部实现，然后将关联项添加到 struct 中

### 3. 错误处理
- 如果在当前 scope 或其所有 parent scope 中找不到对应的 const/struct/trait，抛出 "Undefined Name" 错误

## 实现的 Visit 函数

### 顶层节点
- `visit(Crate& node)`: 遍历 crate 中的所有 items
- `visit(Item& node)`: 处理通用 item 节点

### 声明类节点 (Items)
- `visit(Function& node)`: 处理函数定义，检查参数和返回值类型
- `visit(Struct& node)`: 处理结构体定义
- `visit(Enumeration& node)`: 处理枚举定义，检查枚举符号是否存在
- `visit(ConstantItem& node)`: 处理常量项，检查常量符号和类型是否存在
- `visit(Trait& node)`: 处理特征定义，检查特征符号是否存在
- `visit(Implementation& node)`: 处理实现块

### 函数相关节点
- `visit(FunctionParameters& node)`: 处理函数参数列表
- `visit(SelfParam& node)`: 处理 self 参数
- `visit(ShorthandSelf& node)`: 处理简写形式的 self
- `visit(TypedSelf& node)`: 处理带类型注解的 self
- `visit(FunctionParam& node)`: 处理函数参数，检查参数类型
- `visit(FunctionReturnType& node)`: 处理函数返回类型

### 结构体相关节点
- `visit(StructStruct& node)`: 处理结构体定义
- `visit(StructFields& node)`: 处理结构体字段列表
- `visit(StructField& node)`: 处理结构体字段，检查字段类型

### 枚举相关节点
- `visit(EnumVariants& node)`: 处理枚举变体列表
- `visit(EnumVariant& node)`: 处理枚举变体

### 实现相关节点
- `visit(AssociatedItem& node)`: 处理关联项
- `visit(InherentImpl& node)`: 处理固有实现，进入 impl 作用域并调用 `handleInherentImpl()`
- `visit(TraitImpl& node)`: 处理特征实现，进入 impl 作用域并调用 `handleTraitImpl()`

### 语句类节点
- `visit(Statement& node)`: 处理通用语句节点
- `visit(LetStatement& node)`: 处理 let 语句，检查类型是否存在
- `visit(ExpressionStatement& node)`: 处理表达式语句
- `visit(Statements& node)`: 处理语句列表

### 表达式类节点
- `visit(Expression& node)`: 处理通用表达式节点
- `visit(ExpressionWithoutBlock& node)`: 处理无块表达式
- `visit(ExpressionWithBlock& node)`: 处理带块表达式

### 字面量表达式
- `visit(CharLiteral& node)`: 处理字符字面量
- `visit(StringLiteral& node)`: 处理字符串字面量
- `visit(RawStringLiteral& node)`: 处理原始字符串字面量
- `visit(CStringLiteral& node)`: 处理 C 字符串字面量
- `visit(RawCStringLiteral& node)`: 处理原始 C 字符串字面量
- `visit(IntegerLiteral& node)`: 处理整数字面量
- `visit(BoolLiteral& node)`: 处理布尔字面量

### 路径和访问表达式
- `visit(PathExpression& node)`: 处理路径表达式
- `visit(FieldExpression& node)`: 处理字段访问表达式

### 运算符表达式
- `visit(UnaryExpression& node)`: 处理一元表达式
- `visit(BorrowExpression& node)`: 处理借用表达式
- `visit(DereferenceExpression& node)`: 处理解引用表达式
- `visit(BinaryExpression& node)`: 处理二元表达式
- `visit(AssignmentExpression& node)`: 处理赋值表达式
- `visit(CompoundAssignmentExpression& node)`: 处理复合赋值表达式
- `visit(TypeCastExpression& node)`: 处理类型转换表达式

### 调用和索引表达式
- `visit(CallExpression& node)`: 处理函数调用表达式
- `visit(MethodCallExpression& node)`: 处理方法调用表达式
- `visit(IndexExpression& node)`: 处理索引表达式

### 结构体和数组表达式
- `visit(StructExpression& node)`: 处理结构体表达式
- `visit(ArrayExpression& node)`: 处理数组表达式
- `visit(GroupedExpression& node)`: 处理分组表达式

### 控制流表达式
- `visit(BlockExpression& node)`: 处理块表达式，进入新作用域
- `visit(IfExpression& node)`: 处理 if 表达式
- `visit(LoopExpression& node)`: 处理循环表达式
- `visit(InfiniteLoopExpression& node)`: 处理无限循环表达式，进入新作用域
- `visit(PredicateLoopExpression& node)`: 处理条件循环表达式，进入新作用域
- `visit(BreakExpression& node)`: 处理 break 表达式
- `visit(ContinueExpression& node)`: 处理 continue 表达式
- `visit(ReturnExpression& node)`: 处理 return 表达式

### 辅助表达式节点
- `visit(Condition& node)`: 处理条件表达式
- `visit(ArrayElements& node)`: 处理数组元素列表
- `visit(StructExprFields& node)`: 处理结构体表达式字段
- `visit(StructExprField& node)`: 处理结构体表达式字段
- `visit(CallParams& node)`: 处理调用参数列表

### 模式类节点
- `visit(PatternNoTopAlt& node)`: 处理模式节点
- `visit(IdentifierPattern& node)`: 处理标识符模式
- `visit(ReferencePattern& node)`: 处理引用模式

### 类型类节点
- `visit(Type& node)`: 处理通用类型节点
- `visit(ReferenceType& node)`: 处理引用类型
- `visit(ArrayType& node)`: 处理数组类型
- `visit(UnitType& node)`: 处理单元类型

### 路径类节点
- `visit(PathInExpression& node)`: 处理表达式中的路径
- `visit(PathIdentSegment& node)`: 处理路径标识符段

### 辅助函数
- `checkTypeExists(SymbolType type)`: 检查类型是否存在
- `handleInherentImpl()`: 处理固有实现
- `handleTraitImpl(std::string identifier)`: 处理特征实现

## 作用域遍历模式

StructChecker 使用以下模式在遍历 AST 的同时访问 scope tree：

```cpp
// 进入下一个 scope（仅用于 block expression、函数、trait、impl 和 LoopExpression）
auto prev_scope = current_scope;
current_scope = current_scope->getChild();
// do something，例如进一步访问 AST
current_scope = prev_scope;
current_scope->nextChild();
```

### 作用域创建规则

根据 SymbolCollector 的实现，只有以下节点会创建新的 scope：
- **BlockExpression**: 块表达式作用域
- **Function**: 函数作用域
- **Trait**: 特征作用域
- **Implementation (InherentImpl/TraitImpl)**: 实现块作用域
- **LoopExpression**: 循环表达式作用域

其他节点（如 Struct、Enumeration、ConstantItem）不会创建新的 scope，直接在当前作用域中进行符号检查。

## 类型检查策略

### 1. 类型存在性检查
- 对于所有包含类型信息的节点，递归访问其类型子节点
- 通过 `checkTypeExists()` 函数验证类型是否在作用域链中存在
- 支持内置类型、结构体类型、枚举类型的检查

### 2. 作用域管理
- 正确处理需要创建新作用域的节点：BlockExpression、Function、Trait、Implementation、LoopExpression
- 使用标准的作用域切换模式确保正确的符号查找

### 3. 特殊处理
- **Let Statement**: 检查变量声明中的类型是否存在
- **Function**: 检查参数类型和返回值类型
- **Struct/Enum**: 检查符号是否存在
- **Impl**: 调用专门的辅助函数处理实现块集成

## 设计原则

1. **完整性**: 实现了所有 ASTVisitor 虚函数，确保完整的 AST 遍历
2. **利用现有符号系统**: 通过 identifier 查找对应的 symbol，进而得到需要验证的类型信息
3. **避免重复实现**: 不实现 typeToString 等辅助函数，充分利用 scope 提供的递归查找功能
4. **遵循访问者模式**: 通过 AST 遍历进行结构化的类型检查
5. **错误一致性**: 统一使用 "Undefined Name" 错误信息
6. **作用域正确性**: 严格按照 SymbolCollector 创建的作用域结构进行遍历

## 使用示例

```cpp
// 创建 StructChecker 实例
StructChecker checker(root_scope);

// 遍历 AST 进行类型检查
checker.visit(crate_node);
```

## 注意事项

- 确保在调用 StructChecker 之前，SymbolCollector 已经完成符号收集和作用域树构建
- StructChecker 依赖于正确的 scope 树结构，包括父子关系和 self_type 设置
- 所有类型检查都基于符号表中已定义的类型信息