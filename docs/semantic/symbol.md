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
- **功能**: 表示结构体符号，支持存储所有关联项（associated items）
- **属性**: 标识符、类型、字段映射、关联常量映射、方法映射、关联函数映射
- **主要方法**:
  - `getIdentifier()`: 获取结构体名
  
  - **字段管理**:
    - `addField()`: 添加字段
    - `hasField()`: 检查是否存在指定字段
    - `getField()`: 获取指定字段
    - `getFields()`: 获取所有字段
  
  - **关联常量管理**:
    - `addAssociatedConst()`: 添加关联常量（来自 impl 块中的 const item）
    - `hasAssociatedConst()`: 检查是否存在指定关联常量
    - `getAssociatedConst()`: 获取指定关联常量
    - `getAssociatedConsts()`: 获取所有关联常量
  
  - **方法管理**（带 self 参数）:
    - `addMethod()`: 添加方法（带 self 参数的函数）
    - `hasMethod()`: 检查是否存在指定方法
    - `getMethod()`: 获取指定方法
    - `getMethods()`: 获取所有方法
  
  - **关联函数管理**（不带 self 参数）:
    - `addAssociatedFunction()`: 添加关联函数（不带 self 参数的函数）
    - `hasAssociatedFunction()`: 检查是否存在指定关联函数
    - `getAssociatedFunction()`: 获取指定关联函数
    - `getAssociatedFunctions()`: 获取所有关联函数
  
  - `getAllAssociatedFunctions()`: 获取所有关联项（方法 + 关联函数）

**存储优化**: 所有关联项现在使用 `std::unordered_map<std::string, std::shared_ptr<T>>` 存储，以名称为键，提供 O(1) 的查找性能。

#### EnumVar & EnumSymbol
- **功能**: 表示枚举变体和枚举符号
- **属性**: 标识符、变体列表
- **主要方法**:
  - `getIdentifier()`: 获取标识符
  - `addVariant()`: 添加变体
  - `getVariants()`: 获取所有变体

#### 方法类型枚举 (MethodType)
- **功能**: 定义不同的方法接收器类型
- **枚举值**:
  - `NOT_METHOD`: 不是方法（普通函数）
  - `SELF_VALUE`: `self` (按值获取)
  - `SELF_REF`: `&self` (不可变引用)
  - `SELF_MUT_REF`: `&mut self` (可变引用)
  - `SELF_MUT_VALUE`: `mut self` (按值获取，但可变)

#### FuncSymbol
- **功能**: 表示函数符号，支持详细的方法类型区分
- **属性**: 标识符、返回类型、是否为常量函数、方法类型、参数列表
- **主要方法**:
  - `getIdentifier()`: 获取函数名
  - `isConst()`: 检查是否为常量函数
  - `isMethod()`: 检查是否为方法（带 self 参数）
  - `getMethodType()`: 获取具体的方法类型
  - `getMethodTypeString()`: 获取方法类型的字符串表示
  - `addParameter()`: 添加参数
  - `getParameters()`: 获取所有参数
  - `getReturnType()`: 获取返回类型

**方法类型识别**: 系统能够精确识别以下 self 参数形式：
- `self`: 按值获取所有权
- `&self`: 不可变引用
- `&mut self`: 可变引用
- `mut self`: 按值获取但可变
- `self: Type`: 带类型注解的按值获取
- `mut self: Type`: 带类型注解的按值可变获取

**方法判断**: 通过分析函数参数中的 self 参数类型来确定具体的方法类型，支持简写形式和类型注解形式。

#### TraitSymbol
- **功能**: 表示特征符号
- **属性**: 标识符、关联常量映射、方法映射、关联函数映射
- **主要方法**:
  - `getIdentifier()`: 获取特征名
  
  - **关联常量管理**:
    - `addConstSymbol()`: 添加关联常量
    - `hasConstSymbol()`: 检查是否存在指定关联常量
    - `getConstSymbol()`: 获取指定关联常量
    - `getConstSymbols()`: 获取所有关联常量
  
  - **方法管理**（带 self 参数）:
    - `addMethod()`: 添加方法（带 self 参数的函数）
    - `hasMethod()`: 检查是否存在指定方法
    - `getMethod()`: 获取指定方法
    - `getMethods()`: 获取所有方法
  
  - **关联函数管理**（不带 self 参数）:
    - `addAssociatedFunction()`: 添加关联函数（不带 self 参数的函数）
    - `hasAssociatedFunction()`: 检查是否存在指定关联函数
    - `getAssociatedFunction()`: 获取指定关联函数
    - `getAssociatedFunctions()`: 获取所有关联函数
  
  - `getAllAssociatedFunctions()`: 获取所有关联项（方法 + 关联函数）

**存储优化**: 所有关联项现在使用 `std::unordered_map<std::string, std::shared_ptr<T>>` 存储，以名称为键，提供 O(1) 的查找性能。

**方法与关联函数分离**: TraitSymbol 现在将方法和关联函数分开存储，类似于 StructSymbol，通过检查函数是否有 self 参数来进行区分。

### 2. 常量值系统 (ConstValue System)

注意，ConstValue 应当在常量求值部分再进行深度处理，此处仅进行符号的收集。

#### ConstValue 基类
- **位置**: [`include/semantic/const_value.hpp`](include/semantic/const_value.hpp:8)
- **功能**: 所有常量值的基类，记录对应的 AST 节点指针
- **主要方法**:
  - `getExpressionNode()`: 获取对应的 AST 节点
  - `getValueType()`: 获取值的类型（纯虚函数）
  - `toString()`: 获取字符串表示（纯虚函数）
  - `isInt()`, `isBool()`, `isChar()`, `isString()`, `isStruct()`, `isEnum()`: 类型检查方法

#### ConstValue 派生类

**ConstValueInt**: 整型常量值
- **功能**: 表示整型常量
- **主要方法**: `getValue()`, `setValue()`, `getValueType()`, `toString()`, `isInt()`

**ConstValueBool**: 布尔常量值
- **功能**: 表示布尔常量
- **主要方法**: `getValue()`, `setValue()`, `getValueType()`, `toString()`, `isBool()`

**ConstValueChar**: 字符常量值
- **功能**: 表示字符常量
- **主要方法**: `getValue()`, `setValue()`, `getValueType()`, `toString()`, `isChar()`

**ConstValueString**: 字符串常量值
- **功能**: 表示字符串常量
- **主要方法**: `getValue()`, `setValue()`, `getValueType()`, `toString()`, `isString()`

**ConstValueStruct**: 结构体常量值
- **功能**: 表示结构体常量，存储所有字段内容
- **主要方法**:
  - `getStructName()`, `setStructName()`: 结构体名称管理
  - `setField()`, `getField()`, `hasField()`, `getFields()`: 字段管理
  - `getValueType()`, `toString()`, `isStruct()`

**ConstValueEnum**: 枚举常量值
- **功能**: 表示枚举常量
- **主要方法**:
  - `getEnumName()`, `setEnumName()`: 枚举名称管理
  - `getVariantName()`, `setVariantName()`: 变体名称管理
  - `getValueType()`, `toString()`, `isEnum()`

#### 辅助函数
- `createConstValueFromExpression()`: 从表达式节点创建对应的 ConstValue

### 3. 作用域管理 (Scope Management)

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

### 3. 符号收集器 (SymbolCollector)

#### SymbolCollector 类
- **位置**: [`include/semantic/symbol_collector.hpp`](include/semantic/symbol_collector.hpp:8)
- **实现**: [`src/semantic/symbol_collector.cpp`](src/semantic/symbol_collector.cpp:1)
- **功能**: 继承自 ASTVisitor，负责遍历 AST 并构建符号表和作用域层次结构

#### 主要成员
- `current_scope`: 当前活动的作用域指针
- `root_scope`: 根作用域（全局作用域）指针

#### 辅助方法
- `typeToString(std::shared_ptr<Type> type)`: 将 Type 节点转换为字符串表示
- `createVariableSymbolFromPattern()`: 从模式匹配节点创建变量符号
- `getRootScope()`: 获取根作用域

#### 符号收集功能

**顶层符号收集**:
- [`visit(Crate&)`](src/semantic/symbol_collector.cpp:35): 创建全局作用域，遍历所有顶层项
- [`visit(Item&)`](src/semantic/symbol_collector.cpp:49): 访问单个项

**声明类符号收集**:
- [`visit(Function&)`](src/semantic/symbol_collector.cpp:53): 收集函数符号，创建函数作用域，处理参数和返回类型
- [`visit(Struct&)`](src/semantic/symbol_collector.cpp:120): 访问结构体定义
- [`visit(StructStruct&)`](src/semantic/symbol_collector.cpp:125): 收集结构体符号和字段信息
- [`visit(Enumeration&)`](src/semantic/symbol_collector.cpp:165): 收集枚举符号和变体
- [`visit(ConstantItem&)`](src/semantic/symbol_collector.cpp:207): 收集常量符号，（暂不）支持 ConstValue 集成
- [`visit(Trait&)`](src/semantic/symbol_collector.cpp:218): 收集特征符号和关联项，创建特征作用域
- [`visit(Implementation&)`](src/semantic/symbol_collector.cpp:260): 访问实现块
- [`visit(InherentImpl&)`](src/semantic/symbol_collector.cpp:265): 处理固有实现，创建实现作用域
- [`visit(TraitImpl&)`](src/semantic/symbol_collector.cpp:285): 处理特征实现

**语句和表达式符号收集**:
- [`visit(BlockExpression&)`](src/semantic/symbol_collector.cpp:340): 创建块作用域，处理局部变量
- [`visit(LetStatement&)`](src/semantic/symbol_collector.cpp:461): 收集局部变量符号，支持数组类型处理
- [`visit(LoopExpression&)`](src/semantic/symbol_collector.cpp:405): 处理循环表达式
- [`visit(InfiniteLoopExpression&)`](src/semantic/symbol_collector.cpp:410): 创建循环作用域
- [`visit(PredicateLoopExpression&)`](src/semantic/symbol_collector.cpp:428): 处理条件循环

**完整 AST 遍历**:
实现了所有必要的 visit 方法，确保能够完整遍历 AST，包括：
- 所有表达式类型（字面量、运算符、调用、访问等）
- 所有语句类型（let、表达式语句等）
- 所有模式匹配类型
- 所有类型定义

#### 作用域建立规则

1. **全局作用域**: 在 `visit(Crate&)` 中创建，包含所有顶层符号
2. **函数作用域**: 在 `visit(Function&)` 中创建，包含函数参数
3. **块作用域**: 在 `visit(BlockExpression&)` 中创建，包含局部变量
4. **特征作用域**: 在 `visit(Trait&)` 中创建，包含关联项
5. **实现作用域**: 在 `visit(InherentImpl&)` 和 `visit(TraitImpl&)` 中创建
6. **循环作用域**: 在 `visit(InfiniteLoopExpression&)` 和 `visit(PredicateLoopExpression&)` 中创建

#### 符号收集策略

- **常量项**: 收集标识符和类型信息，存储在当前作用域的常量表中
- **结构体**: 收集结构体名和字段信息，字段作为 VariableSymbol 存储
- **枚举**: 收集枚举名和所有变体
- **函数**: 收集函数名、参数列表、返回类型，参数作为 VariableSymbol 存储在函数作用域中
- **特征**: 收集特征名、关联常量和关联函数

值得注意的是，我们没有处理局部变量，这应当是类型检查部分的工作。

同时，我们也没有处理数组类型，因为确定其长度需要常量求值。

### 4. StructSymbol 的关联项管理

#### 关联项类型
StructSymbol 现在支持存储以下类型的关联项：

1. **结构体字段**：原始的字段信息，通过 `VariableSymbol` 存储
2. **关联常量**：来自 impl 块中的 const item，通过 `ConstSymbol` 存储
3. **方法**：带 self 参数的函数，通过 `FuncSymbol` 存储在 `methods` 列表中
4. **关联函数**：不带 self 参数的函数，通过 `FuncSymbol` 存储在 `functions` 列表中

#### 关联项的添加和管理

**固有实现（InherentImpl）中的关联项**：
- 关联常量：通过 `addAssociatedConst()` 添加到 `associated_consts` 列表
- 方法：通过 `addMethod()` 添加到 `methods` 列表（需要检查是否有 self 参数）
- 关联函数：通过 `addAssociatedFunction()` 添加到 `functions` 列表

**特征实现（TraitImpl）中的关联项**：
- 同样通过上述方法添加到对应的列表中
- 需要验证与特征定义的兼容性
- 确保特征中定义的所有关联项都被实现

#### 方法与关联函数的区分

系统通过检查函数是否有 self 参数来区分方法和关联函数：
- **方法**：带有 self 参数（`self`, `&self`, `&mut self`, `self: Type` 等）
- **关联函数**：不带 self 参数，类似于静态方法

这种设计使得 StructSymbol 能够完整地表示一个结构体的所有相关信息，包括其字段、方法和关联函数，为后续的类型检查和代码生成提供了完整的符号信息。

## 下一步

符号系统、符号收集器和结构体检查器已完成实现。下一步可以：

1. 实现名称解析功能，利用已建立的符号表和作用域结构
2. 添加类型检查功能
3. 实现更复杂的语义分析功能
4. 添加错误处理和诊断信息

## 相关文档

- [结构体检查器文档](struct_checker.md)：详细描述 StructChecker 的实现和功能

## 文件结构

```
include/semantic/
├── symbol.hpp         # 符号类型定义
├── scope.hpp          # 作用域管理
├── symbol_collector.hpp # 符号收集器接口
└── struct_checker.hpp # 结构体检查器接口

src/semantic/
├── symbol.cpp         # 符号类型实现
├── scope.cpp          # 作用域管理实现
├── symbol_collector.cpp # 符号收集器实现
└── struct_checker.cpp # 结构体检查器实现

docs/semantic/
├── symbol.md           # 符号系统完整文档
└── struct_checker.md  # 结构体检查器文档