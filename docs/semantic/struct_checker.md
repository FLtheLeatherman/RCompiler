# 结构体检查器文档

## 概述

StructChecker 是 Rust 子集编译器语义分析阶段的重要组件，负责在符号收集和常量求值完成后，对整个 AST 进行类型存在性检查。该组件验证所有使用到的类型是否存在且可见，同时处理 impl 块与结构体的集成，确保类型系统的完整性。

## 核心功能

### 1. 类型存在性检查
- **结构体字段类型**: 验证结构体中所有字段的类型是否存在
- **函数参数和返回值类型**: 检查函数声明中的类型引用
- **关联常量类型**: 验证 impl 块中关联常量的类型
- **所有类型引用**: 确保代码中出现的每个类型都是有效的

### 2. Impl 块处理
- **InherentImpl**: 将固有实现中的关联项集成到对应的结构体中
- **TraitImpl**: 验证特征实现的完整性，并将关联项集成到结构体中

### 3. 错误诊断
- **未定义类型**: 检测并报告类型未定义错误
- **作用域问题**: 验证类型的可见性和作用域规则
- **集成错误**: 处理 impl 块与结构体集成过程中的错误

## StructChecker 类

### 类定义
- **位置**: [`include/semantic/struct_checker.hpp:19`](include/semantic/struct_checker.hpp:19)
- **继承**: 继承自 `ASTVisitor`
- **功能**: 遍历 AST 并进行类型检查

### 核心成员
```cpp
class StructChecker : public ASTVisitor {
private:
    std::shared_ptr<Scope> current_scope;  // 当前作用域
    std::shared_ptr<Scope> root_scope;     // 根作用域
    
    // 辅助方法
    void handleInherentImpl();              // 处理固有实现
    void handleTraitImpl(std::string);     // 处理特征实现
};
```

## 作用域遍历模式

StructChecker 使用标准的作用域遍历模式，确保正确访问作用域树：

```cpp
// 进入新作用域（仅用于特定节点）
auto prev_scope = current_scope;
current_scope = current_scope->getChild();
// 进行类型检查...
current_scope = prev_scope;
current_scope->nextChild();
```

### 作用域创建规则
根据 SymbolCollector 的实现，以下节点会创建新的作用域：
- **BlockExpression**: 块表达式作用域
- **Function**: 函数作用域
- **Trait**: 特征作用域
- **Implementation**: 实现块作用域（InherentImpl/TraitImpl）
- **LoopExpression**: 循环表达式作用域

其他节点（如 Struct、Enumeration、ConstantItem）不会创建新的作用域，直接在当前作用域中进行类型检查。

## 实现的 Visit 方法

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
- `visit(LetStatement& node)`: 处理 let 语句，**不**检查变量声明中的类型是否存在
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

## 核心辅助方法

### checkTypeExists
- **功能**: 检查类型是否存在
- **位置**: [`include/semantic/utils.hpp:236`](include/semantic/utils.hpp:236)
- **签名**: `bool checkTypeExists(std::shared_ptr<Scope> current_scope, SymbolType type)`

#### 检查逻辑
1. **预处理**: 去除引用标记和可变标记
2. **数组类型**: 处理数组类型的基础类型
3. **内置类型**: 检查是否为内置类型
4. **用户定义类型**: 在作用域链中查找结构体和枚举类型

```cpp
inline bool checkTypeExists(std::shared_ptr<Scope> current_scope, SymbolType type) {
    // 去除引用和可变标记
    if (type.length() > 0 && type[0] == '&') type = type.substr(1);
    if (type.length() >= 3 && type.substr(0, 3) == "mut") type = type.substr(3);
    
    // 处理数组类型
    while (type.length() > 0 && type[0] == '[') {
        while (type.back() != ']') type.pop_back();
        type = type.substr(1, type.length() - 2);
    }
    
    // 检查内置类型
    for (auto builtin_type: builtin_types) {
        if (type == builtin_type) {
            return true;
        }
    }
    
    // 检查用户定义类型
    if (current_scope->structSymbolExists(type)) return true;
    if (current_scope->enumSymbolExists(type)) return true;
    return false;
}
```

### handleInherentImpl
- **功能**: 处理固有实现（InherentImpl）
- **调用位置**: `visit(InherentImpl& node)`
- **处理流程**:
  1. 获取 impl 块的目标类型
  2. 在作用域中查找对应的结构体符号
  3. 将 impl 块中的关联项添加到结构体中
  4. 区分方法和关联函数（基于是否有 self 参数）

### handleTraitImpl
- **功能**: 处理特征实现（TraitImpl）
- **调用位置**: `visit(TraitImpl& node)`
- **参数**: trait 标识符字符串
- **处理流程**:
  1. 获取 impl 块的目标类型和 trait 名称
  2. 在作用域中查找对应的结构体符号和 trait 符号
  3. 验证 trait 中定义的所有关联项是否都被实现
  4. 将实现的关联项添加到结构体中

## 类型检查策略

### 1. 类型存在性验证
- **递归检查**: 对于所有包含类型信息的节点，递归访问其类型子节点
- **作用域链查找**: 通过 `checkTypeExists()` 函数在作用域链中验证类型
- **内置类型支持**: 支持所有 Rust 内置类型的检查

### 2. 作用域管理
- **正确切换**: 严格按照 SymbolCollector 创建的作用域结构进行遍历
- **标准模式**: 使用统一的作用域切换模式确保正确的符号查找
- **作用域隔离**: 确保不同作用域中的符号不会相互干扰

### 3. 特殊处理规则
- **Let Statement**: **不**检查变量声明中的类型是否存在（由类型检查器处理）
- **Function**: 检查参数类型和返回值类型的存在性
- **Struct/Enum**: 检查符号本身是否存在（由符号收集器保证）
- **Impl**: 调用专门的辅助函数处理实现块集成

## 错误处理

### 错误类型
1. **"Undefined Name"**: 类型、常量或结构体未定义
2. **作用域错误**: 类型不在当前作用域或其父作用域中
3. **集成错误**: impl 块与目标类型不匹配

### 错误处理策略
- **统一错误信息**: 使用一致的错误格式便于调试
- **详细位置信息**: 利用 AST 节点提供错误位置
- **早期检测**: 在遍历过程中尽早发现和报告错误

## 设计原则

### 1. 完整性
- **全覆盖**: 实现了所有 ASTVisitor 虚函数，确保完整的 AST 遍历
- **无遗漏**: 检查所有可能包含类型信息的节点

### 2. 利用现有系统
- **符号系统**: 充分利用已建立的符号表和作用域系统
- **类型工具**: 使用 `checkTypeExists()` 等工具函数避免重复实现

### 3. 遵循访问者模式
- **结构化遍历**: 通过 AST 遍历进行系统化的类型检查
- **一致性**: 与其他语义分析组件保持一致的遍历模式

### 4. 错误一致性
- **统一格式**: 使用 "Undefined Name" 等标准错误信息
- **可预测性**: 错误处理逻辑清晰且一致

## 使用示例

### 基本使用
```cpp
// 创建 StructChecker 实例
StructChecker checker(root_scope);

// 遍历 AST 进行类型检查
checker.visit(crate_node);
```

### 类型检查示例
```rust
// 这些类型会被检查是否存在
struct Point {
    x: i32,        // 检查 i32 是否存在
    y: i32,        // 检查 i32 是否存在
}

impl Point {
    fn new(x: i32, y: i32) -> Self {  // 检查 Self, i32 是否存在
        Point { x, y }
    }
}

fn distance(p1: Point, p2: Point) -> f64 {  // 检查 Point, f64 是否存在
    // ...
}
```

### Impl 块处理示例
```rust
struct MyStruct {
    value: i32,
}

impl MyStruct {
    const DEFAULT: i32 = 0;  // 关联常量
    
    fn new() -> Self {        // 关联函数
        MyStruct { value: Self::DEFAULT }
    }
    
    fn get_value(&self) -> i32 {  // 方法
        self.value
    }
}

trait MyTrait {
    fn method(&self) -> i32;
}

impl MyTrait for MyStruct {
    fn method(&self) -> i32 {  // trait 方法实现
        self.value
    }
}
```

## 性能考虑

### 优化策略
1. **早期退出**: 在发现错误时尽早停止检查
2. **缓存结果**: 对重复的类型检查进行缓存
3. **最小化遍历**: 只遍历包含类型信息的节点

### 内存管理
- **共享指针**: 使用 `std::shared_ptr` 管理作用域和符号
- **避免拷贝**: 通过引用传递大型对象
- **及时清理**: 在不需要时释放资源

## 扩展指南

### 添加新的类型检查规则
1. 在相应的 visit 方法中添加检查逻辑
2. 使用 `checkTypeExists()` 函数验证类型存在性
3. 添加适当的错误处理和诊断信息
4. 编写测试用例验证新规则

### 支持新的类型系统特性
1. 扩展 `checkTypeExists()` 函数以支持新类型
2. 更新内置类型列表（如需要）
3. 修改作用域查找逻辑（如需要）
4. 确保与现有系统的兼容性

### 改进错误报告
1. 添加更详细的错误信息
2. 提供错误恢复机制
3. 支持错误位置的高亮显示
4. 添加错误修复建议

## 相关文档

- [符号系统文档](symbol.md): 了解符号类型和作用域管理
- [类型检查器文档](type_checker.md): 了解更详细的类型检查逻辑
- [工具函数文档](../utils.md): 了解辅助函数的实现
- [AST 访问者模式文档](../parser/visitor.md): 了解 AST 遍历机制

## 注意事项

1. **依赖关系**: 确保在调用 StructChecker 之前，SymbolCollector 已经完成符号收集和作用域树构建
2. **作用域正确性**: StructChecker 依赖于正确的 scope 树结构，包括父子关系和 self_type 设置
3. **类型系统**: 所有类型检查都基于符号表中已定义的类型信息
4. **错误处理**: 确保错误信息清晰且有助于调试

---

StructChecker 为 Rust 子集编译器提供了关键的类型存在性验证功能，通过系统化的 AST 遍历和类型检查，确保了代码的类型安全性和正确性。该组件与符号系统和作用域管理紧密集成，为后续的类型检查和代码生成阶段奠定了坚实的基础。