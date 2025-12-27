# 符号系统文档

## 概述

符号系统是 Rust 子集编译器语义分析阶段的核心组件，负责表示和管理程序中的各种符号实体。该系统提供了完整的符号类型定义、作用域管理和符号查找功能，为类型检查、常量求值等后续分析阶段提供基础支持。

## 符号类型层次结构

### 基础符号类

#### Symbol
- **位置**: [`include/semantic/symbol.hpp:15`](include/semantic/symbol.hpp:15)
- **功能**: 所有符号类型的抽象基类
- **核心属性**:
  - `type`: 符号的类型信息，使用 `SymbolType`（即 `std::string`）表示
- **主要方法**:
  - `Symbol(const SymbolType& type)`: 构造函数，指定符号类型
  - `SymbolType getType()`: 获取符号类型
  - `void setType(SymbolType)`: 设置符号类型

### 具体符号类型

#### ConstSymbol - 常量符号
- **位置**: [`include/semantic/symbol.hpp:25`](include/semantic/symbol.hpp:25)
- **功能**: 表示程序中的常量声明
- **核心属性**:
  - `identifier`: 常量名称
  - `value`: 常量值（`std::shared_ptr<ConstValue>`）
- **主要方法**:
  - `std::string getIdentifier()`: 获取常量名称
  - `std::shared_ptr<ConstValue> getValue()`: 获取常量值
  - `void setValue(std::shared_ptr<ConstValue>)`: 设置常量值
  - `bool hasValue()`: 检查是否已赋值

#### VariableSymbol - 变量符号
- **位置**: [`include/semantic/symbol.hpp:38`](include/semantic/symbol.hpp:38)
- **功能**: 表示变量、函数参数等可变实体
- **核心属性**:
  - `identifier`: 变量名称
  - `is_ref`: 是否为引用类型
  - `is_mut`: 可变性标记（0=不可变，1=可变，2=mut self）
- **主要方法**:
  - `std::string getIdentifier()`: 获取变量名
  - `bool isRef()`: 检查是否为引用
  - `int getMut()`: 获取可变性级别

#### StructSymbol - 结构体符号
- **位置**: [`include/semantic/symbol.hpp:50`](include/semantic/symbol.hpp:50)
- **功能**: 表示结构体类型及其所有关联项
- **核心属性**:
  - `identifier`: 结构体名称
  - `vars`: 字段映射（`std::unordered_map<std::string, std::shared_ptr<VariableSymbol>>`）
  - `associated_consts`: 关联常量映射
  - `methods`: 方法映射（带 self 参数）
  - `functions`: 关联函数映射（不带 self 参数）

**字段管理**:
- `addField(std::shared_ptr<VariableSymbol>)`: 添加字段
- `bool hasField(const std::string&)`: 检查字段是否存在
- `std::shared_ptr<VariableSymbol> getField(const std::string&)`: 获取字段
- `std::vector<std::shared_ptr<VariableSymbol>> getFields()`: 获取所有字段

**关联常量管理**:
- `addAssociatedConst(std::shared_ptr<ConstSymbol>)`: 添加关联常量
- `bool hasAssociatedConst(const std::string&)`: 检查关联常量是否存在
- `std::shared_ptr<ConstSymbol> getAssociatedConst(const std::string&)`: 获取关联常量

**方法管理**（带 self 参数）:
- `addMethod(std::shared_ptr<FuncSymbol>)`: 添加方法
- `bool hasMethod(const std::string&)`: 检查方法是否存在
- `std::shared_ptr<FuncSymbol> getMethod(const std::string&)`: 获取方法

**关联函数管理**（不带 self 参数）:
- `addAssociatedFunction(std::shared_ptr<FuncSymbol>)`: 添加关联函数
- `bool hasAssociatedFunction(const std::string&)`: 检查关联函数是否存在
- `std::shared_ptr<FuncSymbol> getAssociatedFunction(const std::string&)`: 获取关联函数

#### EnumSymbol - 枚举符号
- **位置**: [`include/semantic/symbol.hpp:99`](include/semantic/symbol.hpp:99)
- **功能**: 表示枚举类型及其变体
- **核心属性**:
  - `identifier`: 枚举名称
  - `vars`: 变体列表（`std::vector<std::shared_ptr<EnumVar>>`）
- **主要方法**:
  - `void addVariant(std::shared_ptr<EnumVar>)`: 添加变体
  - `const std::vector<std::shared_ptr<EnumVar>>& getVariants()`: 获取所有变体

#### EnumVar - 枚举变体
- **位置**: [`include/semantic/symbol.hpp:91`](include/semantic/symbol.hpp:91)
- **功能**: 表示枚举的单个变体
- **核心属性**:
  - `identifier`: 变体名称
- **主要方法**:
  - `std::string getIdentifier()`: 获取变体名称

#### FuncSymbol - 函数符号
- **位置**: [`include/semantic/symbol.hpp:119`](include/semantic/symbol.hpp:119)
- **功能**: 表示函数声明，支持方法类型区分
- **核心属性**:
  - `identifier`: 函数名称
  - `is_const`: 是否为常量函数
  - `method_type`: 方法类型（`MethodType` 枚举）
  - `func_params`: 参数列表
  - `return_type`: 返回类型

**方法类型枚举**:
```cpp
enum class MethodType {
    NOT_METHOD,      // 不是方法（普通函数）
    SELF_VALUE,      // self (按值获取)
    SELF_REF,        // &self (不可变引用)
    SELF_MUT_REF,    // &mut self (可变引用)
    SELF_MUT_VALUE   // mut self (按值获取，但可变)
};
```

**主要方法**:
- `std::string getIdentifier()`: 获取函数名
- `bool isConst()`: 检查是否为常量函数
- `bool isMethod()`: 检查是否为方法（带 self 参数）
- `MethodType getMethodType()`: 获取方法类型
- `std::string getMethodTypeString()`: 获取方法类型的字符串表示
- `void addParameter(std::shared_ptr<VariableSymbol>)`: 添加参数
- `const std::vector<std::shared_ptr<VariableSymbol>>& getParameters()`: 获取所有参数
- `SymbolType getReturnType()`: 获取返回类型

#### TraitSymbol - 特征符号
- **位置**: [`include/semantic/symbol.hpp:139`](include/semantic/symbol.hpp:139)
- **功能**: 表示特征定义及其关联项
- **核心属性**:
  - `identifier`: 特征名称
  - `const_symbols`: 关联常量映射
  - `methods`: 方法映射（带 self 参数）
  - `functions`: 关联函数映射（不带 self 参数）

**管理方法**与 `StructSymbol` 类似，包括关联常量、方法和关联函数的增删查改操作。

#### ArraySymbol - 数组符号
- **位置**: [`include/semantic/symbol.hpp:171`](include/semantic/symbol.hpp:171)
- **功能**: 表示数组类型
- **核心属性**:
  - `identifier`: 数组标识符
  - `element_type`: 元素类型
  - `length`: 数组长度（`std::shared_ptr<ConstValue>`）
- **主要方法**:
  - `std::string getElementType()`: 获取元素类型
  - `std::shared_ptr<ConstValue> getLength()`: 获取数组长度
  - `void setLength(std::shared_ptr<ConstValue>)`: 设置数组长度
  - `bool hasLength()`: 检查是否有长度信息

## 作用域管理系统

### Scope 类
- **位置**: [`include/semantic/scope.hpp:27`](include/semantic/scope.hpp:27)
- **功能**: 管理作用域层次结构和符号表
- **继承**: 继承自 `std::enable_shared_from_this<Scope>` 支持共享指针

#### 作用域类型
```cpp
enum class ScopeType {
    GLOBAL,  // 全局作用域
    BLOCK,   // 块作用域
    FUNCTION,// 函数作用域
    TRAIT,   // 特征作用域
    IMPL,    // 实现作用域
    LOOP     // 循环作用域
};
```

#### 核心属性
- `type`: 作用域类型
- `parent_scope`: 父作用域指针
- `children`: 子作用域列表
- `self_type`: 用于 impl 和函数作用域的 self 类型
- `break_type`: 循环作用域中的 break 表达式类型
- `pos`: 当前访问的子作用域位置

#### 分类符号表
作用域使用独立的哈希表管理不同类型的符号，避免名称冲突：

- `const_symbols`: 常量符号表
- `struct_symbols`: 结构体符号表
- `enum_symbols`: 枚举符号表
- `func_symbols`: 函数符号表
- `trait_symbols`: 特征符号表

#### 变量表
每个作用域包含一个 `variable_table`，用于存储局部变量信息：

```cpp
struct VariableInfo {
    std::string type;    // 变量类型
    bool is_mutable;     // 可变性标记
    
    VariableInfo() : type(""), is_mutable(false) {}
    VariableInfo(const std::string& t, bool mut = false) : type(t), is_mutable(mut) {}
};
```

### 作用域管理方法

#### 层次结构管理
- `void addChild(std::shared_ptr<Scope>)`: 添加子作用域
- `void setParent(std::shared_ptr<Scope>)`: 设置父作用域
- `std::shared_ptr<Scope> getParent()`: 获取父作用域
- `std::shared_ptr<Scope> getChild()`: 获取当前子作用域
- `void nextChild()`: 移动到下一个子作用域

#### 符号管理
每个符号类型都有完整的 CRUD 操作：

**常量符号**:
- `void addConstSymbol(const std::string&, std::shared_ptr<ConstSymbol>)`
- `std::shared_ptr<ConstSymbol> getConstSymbol(const std::string&)`
- `bool hasConstSymbol(const std::string&)`

**结构体符号**:
- `void addStructSymbol(const std::string&, std::shared_ptr<StructSymbol>)`
- `std::shared_ptr<StructSymbol> getStructSymbol(const std::string&)`
- `bool hasStructSymbol(const std::string&)`

**其他符号类型**（枚举、函数、特征）类似...

#### 变量表管理
- `void addVariable(const std::string&, const std::string&, bool is_mutable = false)`: 添加变量
- `std::string getVariableType(const std::string&)`: 获取变量类型
- `bool isVariableMutable(const std::string&)`: 获取变量可变性
- `bool hasVariable(const std::string&)`: 检查变量是否存在

#### 作用域链查找
支持在作用域链中进行符号查找，实现符号遮蔽：

- `std::shared_ptr<ConstSymbol> findConstSymbol(const std::string&)`: 在作用域链中查找常量
- `std::shared_ptr<StructSymbol> findStructSymbol(const std::string&)`: 在作用域链中查找结构体
- `std::string findVariableType(const std::string&)`: 在作用域链中查找变量类型
- `bool variableExists(const std::string&)`: 检查变量是否在作用域链中存在

#### 调试支持
- `void printScope(int indent = 0)`: 打印作用域层次结构和符号信息
- `size_t getTotalSymbolCount()`: 获取作用域树中的总符号数量

## 符号收集器

### SymbolCollector 类
- **位置**: [`include/semantic/symbol_collector.hpp:13`](include/semantic/symbol_collector.hpp:13)
- **实现**: [`src/semantic/symbol_collector.cpp`](src/semantic/symbol_collector.cpp:1)
- **功能**: 继承自 `ASTVisitor`，负责遍历 AST 并构建符号表和作用域层次结构

#### 核心成员
- `current_scope`: 当前活动的作用域指针
- `root_scope`: 根作用域（全局作用域）指针

#### 作用域建立规则
SymbolCollector 在遍历 AST 时按以下规则创建作用域：

1. **全局作用域**: 在 `visit(Crate&)` 中创建，包含所有顶层符号
2. **函数作用域**: 在 `visit(Function&)` 中创建，包含函数参数
3. **块作用域**: 在 `visit(BlockExpression&)` 中创建，包含局部变量
4. **特征作用域**: 在 `visit(Trait&)` 中创建，包含关联项
5. **实现作用域**: 在 `visit(InherentImpl&)` 和 `visit(TraitImpl&)` 中创建
6. **循环作用域**: 在 `visit(InfiniteLoopExpression&)` 和 `visit(PredicateLoopExpression&)` 中创建

#### 符号收集策略

**常量项收集**:
- 收集标识符和类型信息
- 存储在当前作用域的常量表中
- 支持常量值的延迟求值

**结构体收集**:
- 收集结构体名和字段信息
- 字段作为 `VariableSymbol` 存储
- 支持字段类型检查

**函数收集**:
- 收集函数名、参数列表、返回类型
- 参数作为 `VariableSymbol` 存储在函数作用域中
- 识别方法类型（self 参数形式）

**特征收集**:
- 收集特征名、关联常量和关联函数
- 区分方法和关联函数

**局部变量收集**:
- 在 `visit(LetStatement&)` 中收集局部变量
- 使用 `addVariable()` 添加到当前作用域的变量表
- 支持可变性标记和引用类型

## 设计特性

### 1. 类型安全
- 使用强类型系统确保编译时类型安全
- `std::shared_ptr` 进行内存管理，避免内存泄漏

### 2. 性能优化
- 哈希表实现 O(1) 符号查找
- 分类符号表提高查找效率
- 共享指针避免不必要的拷贝

### 3. 扩展性
- 基于继承的设计，易于添加新的符号类型
- 模块化设计，便于维护和扩展

### 4. 作用域正确性
- 完整的作用域链支持
- 符号遮蔽机制
- 严格的作用域创建规则

## 使用示例

### 创建符号表
```cpp
// 创建符号收集器
SymbolCollector collector;

// 遍历 AST 构建符号表
collector.visit(crate_node);

// 获取根作用域
auto root_scope = collector.getRootScope();
```

### 符号查找
```cpp
// 在当前作用域查找符号
auto const_symbol = current_scope->getConstSymbol("MY_CONST");

// 在作用域链中查找符号
auto struct_symbol = current_scope->findStructSymbol("MyStruct");

// 查找变量类型和可变性
auto var_type = current_scope->findVariableType("my_var");
auto is_mutable = current_scope->findVariableMutable("my_var");
```

### 符号操作
```cpp
// 添加结构体符号
auto struct_symbol = std::make_shared<StructSymbol>("Point", "Point");
current_scope->addStructSymbol("Point", struct_symbol);

// 添加字段
auto field_x = std::make_shared<VariableSymbol>("x", "i32");
struct_symbol->addField(field_x);

// 添加方法
auto method = std::make_shared<FuncSymbol>("new_point", "Point", false, MethodType::ASSOCIATED_FUNCTION);
struct_symbol->addAssociatedFunction(method);
```

## 相关文档

- [作用域管理详细文档](../scope.md): 深入了解作用域系统的实现
- [符号收集器文档](../symbol_collector.md): 了解符号收集的详细流程
- [类型检查器文档](../type_checker.md): 了解如何使用符号系统进行类型检查
- [常量求值文档](../const_value.md): 了解常量值的表示和计算

## 扩展指南

### 添加新的符号类型
1. 在 [`symbol.hpp`](include/semantic/symbol.hpp:1) 中定义新的符号类，继承自 `Symbol`
2. 在 [`scope.hpp`](include/semantic/scope.hpp:1) 中添加相应的管理方法
3. 在 [`symbol_collector.cpp`](src/semantic/symbol_collector.cpp:1) 中添加收集逻辑
4. 更新相关的检查器以支持新符号类型

### 扩展作用域功能
1. 在 `Scope` 类中添加新的属性或方法
2. 更新作用域创建规则（如需要新的作用域类型）
3. 确保符号查找逻辑正确处理新功能

### 性能优化
1. 考虑使用更高效的数据结构
2. 优化符号查找算法
3. 减少不必要的内存分配

---

符号系统为 Rust 子集编译器提供了坚实的语义分析基础，通过精心设计的类型层次结构和作用域管理，确保了编译器的正确性和性能。该系统的模块化设计使其易于扩展和维护，为支持更复杂的语言特性奠定了基础。