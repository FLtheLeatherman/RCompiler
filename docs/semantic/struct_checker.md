# 结构体检查器 (StructChecker) 实现文档

## 概述

StructChecker 是一个继承自 ASTVisitor 的语义分析器，专门负责进行 Rust 子集中 struct 相关字段、方法和函数的检查。它在符号收集的基础上，对结构体定义、实现块和特征实现进行深度的语义验证。

## 主要功能

### 1. 结构体字段类型检查
- 检查结构体字段中使用的类型是否存在和可见
- 验证字段类型的有效性
- 对未定义的类型抛出 "Undefined Name" 异常

### 2. 实现块处理
- **固有实现 (InherentImpl)**：处理结构体的固有方法和关联函数
- **特征实现 (TraitImpl)**：处理结构体对特定特征的实现
- 将关联项（方法、关联函数、关联常量）添加到对应的结构体符号中

### 3. 特征实现完整性检查
- 验证特征实现是否包含了特征中定义的所有关联项
- 检查关联函数和关联常量的完整性
- 对缺失的实现抛出相应的错误信息

### 4. 类型存在性验证
- 检查所有用到的类型（字段类型、参数类型、返回类型等）是否存在
- 支持基础类型、结构体类型、枚举类型和特征类型
- 在作用域链中进行类型查找

## 类设计

### 核心成员变量

```cpp
std::shared_ptr<Scope> current_scope;  // 当前活动作用域
std::shared_ptr<Scope> root_scope;      // 根作用域（全局作用域）
```

### 辅助方法

#### 类型转换和检查
- `typeToString(std::shared_ptr<Type> type)`: 将 AST 中的 Type 节点转换为字符串表示
- `checkTypeExists(const std::string& type_name)`: 检查指定类型名是否存在

#### 符号查找
- `findStructSymbol(const std::string& struct_name)`: 在作用域链中查找结构体符号
- `findTraitSymbol(const std::string& trait_name)`: 在作用域链中查找特征符号

#### 函数分析
- `hasSelfParameter(std::shared_ptr<Function> func)`: 检查函数是否有 self 参数

#### 特征实现验证
- `checkTraitImplementation()`: 检查特征实现的完整性

## 访问者模式实现

### 顶层节点访问

#### Crate 和 Item
- `visit(Crate&)`: 遍历所有顶层项
- `visit(Item&)`: 访问单个项

### 结构体相关节点

#### 结构体定义处理
```cpp
void visit(Struct& node);           // 访问结构体定义
void visit(StructStruct& node);     // 检查结构体字段类型
void visit(StructFields& node);     // 遍历字段列表
void visit(StructField& node);      // 检查单个字段类型
```

### 实现相关节点

#### 实现块处理
```cpp
void visit(Implementation& node);   // 访问实现块
void visit(InherentImpl& node);      // 处理固有实现
void visit(TraitImpl& node);        // 处理特征实现
void visit(AssociatedItem& node);   // 访问关联项
```

### 函数相关节点

#### 函数定义处理
```cpp
void visit(Function& node);               // 检查函数参数和返回类型
void visit(FunctionParameters& node);     // 处理函数参数列表
void visit(SelfParam& node);              // 处理 Self 参数
void visit(ShorthandSelf& node);          // 处理简写 Self
void visit(TypedSelf& node);              // 处理带类型的 Self
void visit(FunctionParam& node);          // 检查参数类型
void visit(FunctionReturnType& node);     // 处理返回类型
```

### 其他重要节点

```cpp
void visit(ConstantItem& node);    // 检查常量项
void visit(Trait& node);           // 访问特征定义
void visit(Type& node);            // 访问类型节点
```

## 核心算法

### 1. 类型检查算法

```cpp
bool StructChecker::checkTypeExists(const std::string& type_name) {
    // 1. 检查基础类型
    if (isBasicType(type_name)) return true;
    
    // 2. 在作用域链中查找用户定义类型
    auto struct_symbol = current_scope->findStructSymbol(type_name);
    if (struct_symbol) return true;
    
    auto enum_symbol = current_scope->findEnumSymbol(type_name);
    if (enum_symbol) return true;
    
    auto trait_symbol = current_scope->findTraitSymbol(type_name);
    if (trait_symbol) return true;
    
    return false;
}
```

### 2. 方法与关联函数区分算法

```cpp
bool StructChecker::hasSelfParameter(std::shared_ptr<Function> func) {
    if (!func || !func->function_parameters) {
        return false;
    }
    
    // 检查是否有 self 参数
    return func->function_parameters->self_param != nullptr;
}
```

### 3. 特征实现完整性检查算法

```cpp
void StructChecker::checkTraitImplementation(
    std::shared_ptr<TraitSymbol> trait_symbol, 
    std::shared_ptr<StructSymbol> struct_symbol,
    const std::vector<std::shared_ptr<AssociatedItem>>& impl_items) {
    
    // 1. 创建已实现项的映射
    std::unordered_map<std::string, bool> implemented_funcs;
    std::unordered_map<std::string, bool> implemented_consts;
    
    // 2. 标记已实现的项
    for (auto& item : impl_items) {
        if (auto func = std::dynamic_pointer_cast<Function>(item->child)) {
            implemented_funcs[func->identifier] = true;
        } else if (auto const_item = std::dynamic_pointer_cast<ConstantItem>(item->child)) {
            implemented_consts[const_item->identifier] = true;
        }
    }
    
    // 3. 检查特征中的关联函数是否全部实现
    for (auto& trait_func : trait_symbol->getAssociatedFunctions()) {
        if (!implemented_funcs[trait_func->getIdentifier()]) {
            throw std::runtime_error("Missing implementation of trait function: " + 
                                   trait_func->getIdentifier());
        }
    }
    
    // 4. 检查特征中的关联常量是否全部实现
    for (auto& trait_const : trait_symbol->getConstSymbols()) {
        if (!implemented_consts[trait_const->getIdentifier()]) {
            throw std::runtime_error("Missing implementation of trait constant: " + 
                                   trait_const->getIdentifier());
        }
    }
}
```

## 错误处理

### 错误类型

1. **未定义名称错误**: 当类型、结构体、特征或常量不存在时抛出
   ```cpp
   throw std::runtime_error("Undefined Name: " + name);
   ```

2. **特征实现不完整错误**: 当特征实现缺少必需的关联项时抛出
   ```cpp
   throw std::runtime_error("Missing implementation of trait function: " + func_name);
   throw std::runtime_error("Missing implementation of trait constant: " + const_name);
   ```

### 错误处理策略

- 使用 C++ 异常机制进行错误报告
- 在检测到语义错误时立即抛出异常
- 提供清晰的错误信息，包含具体的名称和错误类型

## 使用方式

### 基本使用

```cpp
// 创建根作用域（通常由 SymbolCollector 创建）
auto root_scope = symbol_collector.getRootScope();

// 创建 StructChecker 实例
StructChecker checker(root_scope);

// 遍历 AST 进行语义检查
checker.visit(crate_node);
```

### 与其他组件的集成

1. **符号收集器**: StructChecker 依赖 SymbolCollector 建立的符号表和作用域结构
2. **作用域管理**: 利用作用域链进行符号查找和类型验证
3. **符号系统**: 将检查结果存储到相应的符号对象中

## 设计特点

### 1. 模块化设计
- 每个功能都有明确的职责分工
- 辅助方法专注于特定的检查任务
- 易于维护和扩展

### 2. 完整的 AST 遍历
- 实现了所有必要的 visit 方法
- 确保能够处理各种复杂的 AST 结构
- 保持遍历的完整性

### 3. 类型安全
- 使用强类型系统确保编译时安全
- 利用智能指针进行内存管理
- 避免内存泄漏和悬空指针

### 4. 错误处理
- 统一的错误处理机制
- 清晰的错误信息
- 便于调试和问题定位

## 文件结构

```
include/semantic/
└── struct_checker.hpp    # StructChecker 类定义和接口

src/semantic/
└── struct_checker.cpp    # StructChecker 类实现

docs/semantic/
└── struct_checker.md     # StructChecker 文档（本文件）
```

## 相关文档

- [符号系统文档](symbol.md)：了解符号系统的设计和实现
- [作用域管理文档](symbol.md#作用域管理-scope-management)：了解作用域层次结构
- [AST 节点文档](../parser/README.md)：了解 AST 节点的定义

## 总结

StructChecker 是编译器语义分析阶段的重要组成部分，它负责对结构体相关的语言特性进行深度检查。通过与符号系统和作用域管理的紧密集成，它能够有效地验证代码的语义正确性，为后续的类型检查和代码生成阶段提供坚实的基础。

其设计充分考虑了 Rust 语言的特性，特别是结构体、impl 块和特征系统的复杂性，提供了一个可扩展、可维护的语义检查框架。