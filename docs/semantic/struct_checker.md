# StructChecker 文档

## 概述

StructChecker 是编译器语义分析阶段的核心组件，负责检查结构体、枚举、常量、特征和实现块中的类型是否存在/可见，以及处理 impl 块与 struct 的集成。

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

### 已实现的函数
- `visit(Struct& node)`: 处理结构体定义
- `visit(Enumeration& node)`: 处理枚举定义
- `visit(ConstantItem& node)`: 处理常量项
- `visit(Trait& node)`: 处理特征定义
- `visit(Implementation& node)`: 处理实现块
- `visit(InherentImpl& node)`: 处理固有实现
- `visit(TraitImpl& node)`: 处理特征实现
- `visit(AssociatedItem& node)`: 处理关联项

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

## 设计原则

1. **利用现有符号系统**: 通过 identifier 查找对应的 symbol，进而得到需要验证的类型信息
2. **避免重复实现**: 不实现 typeToString 等辅助函数，充分利用 scope 提供的递归查找功能
3. **遵循访问者模式**: 通过 AST 遍历进行结构化的类型检查
4. **错误一致性**: 统一使用 "Undefined Name" 错误信息

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