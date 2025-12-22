# 符号系统实现文档

## 概述

本文档描述了 Rust 子集编译器中符号系统的实现。该系统为语义分析阶段的符号收集提供了基础支持。

## 实现的组件

### 1. 符号类型 (Symbol Types)

#### 基类 Symbol
- **位置**: [`include/semantic/symbol.hpp`](include/semantic/symbol.hpp:10)
- **功能**: 所有符号类型的基类，提供基本的类型信息
- **主要方法**:
  - `Symbol(const SymbolType& type)`: 构造函数
  - `SymbolType getType()`: 获取符号类型

#### ConstSymbol
- **功能**: 表示常量符号
- **属性**: 标识符、类型
- **主要方法**:
  - `getIdentifier()`: 获取常量名

#### VariableSymbol
- **功能**: 表示变量符号（包括函数参数和局部变量）
- **属性**: 标识符、类型、是否为引用、是否可变
- **主要方法**:
  - `getIdentifier()`: 获取变量名
  - `isRef()`: 检查是否为引用
  - `isMut()`: 检查是否可变

#### StructSymbol
- **功能**: 表示结构体符号
- **属性**: 标识符、类型、字段列表
- **主要方法**:
  - `getIdentifier()`: 获取结构体名
  - `addField()`: 添加字段
  - `getFields()`: 获取所有字段

#### EnumVar & EnumSymbol
- **功能**: 表示枚举变体和枚举符号
- **属性**: 标识符、变体列表
- **主要方法**:
  - `getIdentifier()`: 获取标识符
  - `addVariant()`: 添加变体
  - `getVariants()`: 获取所有变体

#### FuncSymbol
- **功能**: 表示函数符号
- **属性**: 标识符、返回类型、是否为常量函数、参数列表
- **主要方法**:
  - `getIdentifier()`: 获取函数名
  - `isConst()`: 检查是否为常量函数
  - `addParameter()`: 添加参数
  - `getParameters()`: 获取所有参数
  - `getReturnType()`: 获取返回类型

#### TraitSymbol
- **功能**: 表示特征符号
- **属性**: 标识符、关联常量列表、关联函数列表
- **主要方法**:
  - `getIdentifier()`: 获取特征名
  - `addConstSymbol()`: 添加关联常量
  - `addAssociatedFunction()`: 添加关联函数
  - `getConstSymbols()`: 获取所有关联常量
  - `getAssociatedFunctions()`: 获取所有关联函数

### 2. 作用域管理 (Scope Management)

#### Scope 类
- **位置**: [`include/semantic/scope.hpp`](include/semantic/scope.hpp:19)
- **功能**: 管理作用域层次结构和分类符号表
- **继承**: 继承自 `std::enable_shared_from_this<Scope>` 以支持共享指针管理
- **作用域类型**:
  - `GLOBAL`: 全局作用域
  - `BLOCK`: 块作用域
  - `FUNCTION`: 函数作用域
  - `TRAIT`: 特征作用域
  - `IMPL`: 实现作用域
  - `LOOP`: 循环作用域

- **分类符号表设计**: 使用独立的哈希表管理不同类型的符号，避免名称冲突和提高查找效率

#### 主要方法

**基本访问器**:
- `getType()`: 获取作用域类型
- `getParent()`: 获取父作用域
- `getChildren()`: 获取所有子作用域

**作用域层次结构管理**:
- `addChild()`: 添加子作用域
- `setParent()`: 设置父作用域

**分类符号管理**:
- **常量符号**: `addConstSymbol()`, `getConstSymbol()`, `hasConstSymbol()`, `getConstSymbols()`
- **结构体符号**: `addStructSymbol()`, `getStructSymbol()`, `hasStructSymbol()`, `getStructSymbols()`
- **枚举符号**: `addEnumSymbol()`, `getEnumSymbol()`, `hasEnumSymbol()`, `getEnumSymbols()`
- **函数符号**: `addFuncSymbol()`, `getFuncSymbol()`, `hasFuncSymbol()`, `getFuncSymbols()`
- **特征符号**: `addTraitSymbol()`, `getTraitSymbol()`, `hasTraitSymbol()`, `getTraitSymbols()`

**作用域链查找**:
- `findSymbol()`: 在作用域链中查找常量符号
- `findStructSymbol()`: 在作用域链中查找结构体符号
- `findEnumSymbol()`: 在作用域链中查找枚举符号
- `findFuncSymbol()`: 在作用域链中查找函数符号
- `findTraitSymbol()`: 在作用域链中查找特征符号

**符号存在性检查**:
- `symbolExists()`: 检查常量符号是否存在于作用域链中
- `structSymbolExists()`: 检查结构体符号是否存在于作用域链中
- `enumSymbolExists()`: 检查枚举符号是否存在于作用域链中
- `funcSymbolExists()`: 检查函数符号是否存在于作用域链中
- `traitSymbolExists()`: 检查特征符号是否存在于作用域链中

**当前作用域操作**:
- `getSymbolInCurrentScope()`: 仅在当前作用域查找符号
- `hasSymbolInCurrentScope()`: 检查当前作用域中是否存在符号

**调试和工具**:
- `printScope()`: 打印作用域层次结构和符号信息
- `getTotalSymbolCount()`: 获取作用域树中的总符号数量

## 特性

### 1. 作用域链查找
- 符号查找遵循作用域链规则：当前作用域 → 父作用域 → 祖父作用域 → ... → 全局作用域
- 支持符号遮蔽：内层作用域的符号可以遮蔽外层作用域的同名符号

### 2. 类型安全
- 使用 `std::shared_ptr` 进行内存管理，避免内存泄漏
- 强类型系统确保编译时类型安全

### 3. 扩展性
- 基于继承的设计，易于添加新的符号类型
- 模块化设计，便于维护和扩展

## 测试

测试文件位于 [`test/symbol_test.cpp`](test/symbol_test.cpp:1)，包含：

1. **符号类型测试**: 验证所有符号类型的构造和基本功能
2. **作用域管理测试**: 验证作用域层次结构和符号查找功能

### 运行测试
```bash
cd build
g++ -std=c++17 -I../include ../test/symbol_test.cpp ../src/semantic/symbol.cpp ../src/semantic/scope.cpp -o symbol_test
./symbol_test
```

## 编译验证

整个项目已通过编译验证：
```bash
cd build
make
```

## 下一步

符号系统已为符号收集阶段做好准备。下一步可以：

1. 实现 SymbolCollector 类，遍历 AST 并构建符号表
2. 实现名称解析功能
3. 添加类型检查功能
4. 实现更复杂的语义分析功能

## 文件结构

```
include/semantic/
├── symbol.hpp     # 符号类型定义
└── scope.hpp      # 作用域管理

src/semantic/
├── symbol.cpp     # 符号类型实现
└── scope.cpp      # 作用域管理实现

test/
└── symbol_test.cpp # 符号系统测试

docs/semantic/
├── symbol.md           # 符号类型详细说明
└── symbol_implementation.md # 本实现文档