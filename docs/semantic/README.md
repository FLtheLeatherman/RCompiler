# Semantic 模块文档

## 概述

Semantic 模块是 Rust 子集编译器的语义分析阶段，负责在语法分析（Parser）生成的 AST 基础上进行语义检查和分析。该模块实现了完整的符号表管理、类型检查、常量求值等核心功能，为后续的代码生成阶段提供必要的语义信息。

## 模块架构

Semantic 模块采用分层架构设计，包含以下核心组件：

### 1. 符号系统 (Symbol System)
- **位置**: [`include/semantic/symbol.hpp`](include/semantic/symbol.hpp:1), [`src/semantic/symbol.cpp`](src/semantic/symbol.cpp:1)
- **功能**: 定义编译器中所有符号类型的抽象表示
- **核心类**:
  - `Symbol`: 符号基类，提供类型信息
  - `ConstSymbol`: 常量符号，存储常量值
  - `VariableSymbol`: 变量符号，包含可变性信息
  - `StructSymbol`: 结构体符号，管理字段和关联项
  - `EnumSymbol`: 枚举符号，存储枚举变体
  - `FuncSymbol`: 函数符号，支持方法类型区分
  - `TraitSymbol`: 特征符号，管理关联项
  - `ArraySymbol`: 数组符号，存储元素类型和长度

### 2. 作用域管理 (Scope Management)
- **位置**: [`include/semantic/scope.hpp`](include/semantic/scope.hpp:1), [`src/semantic/scope.cpp`](src/semantic/scope.cpp:1)
- **功能**: 管理作用域层次结构和符号查找
- **核心特性**:
  - 分层作用域：全局、块、函数、特征、实现、循环作用域
  - 分类符号表：按类型分别存储常量、结构体、枚举、函数、特征符号
  - 变量表：跟踪局部变量的类型和可变性
  - 作用域链查找：支持符号遮蔽和向上查找

### 3. 常量值系统 (ConstValue System)
- **位置**: [`include/semantic/const_value.hpp`](include/semantic/const_value.hpp:1), [`src/semantic/const_value.cpp`](src/semantic/const_value.cpp:1)
- **功能**: 表示和操作编译时常量值
- **支持的类型**:
  - `ConstValueInt`: 整型常量
  - `ConstValueBool`: 布尔常量
  - `ConstValueChar`: 字符常量
  - `ConstValueString`: 字符串常量
  - `ConstValueStruct`: 结构体常量
  - `ConstValueEnum`: 枚举常量

### 4. 工具函数 (Utils)
- **位置**: [`include/semantic/utils.hpp`](include/semantic/utils.hpp:1)
- **功能**: 提供语义分析阶段的辅助功能
- **核心函数**:
  - `typeToString()`: 类型转换为字符串表示
  - `createVariableSymbolFromPattern()`: 从模式创建变量符号
  - `createConstValueFromExpression()`: 从表达式创建常量值
  - `handleArraySymbol()`: 处理数组类型
  - `checkTypeExists()`: 检查类型是否存在

## 语义分析流程

Semantic 模块按照以下顺序进行语义分析：

### 第一阶段：符号收集 (Symbol Collection)
**组件**: [`SymbolCollector`](include/semantic/symbol_collector.hpp:13)
- 遍历 AST，构建符号表和作用域层次结构
- 收集所有声明：函数、结构体、枚举、常量、特征
- 建立作用域关系：函数、块、循环等创建子作用域
- 处理方法类型识别：区分 self 参数的不同形式

### 第二阶段：常量求值 (Constant Evaluation)
**组件**: [`ConstEvaluator`](include/semantic/const_evaluator.hpp:11)
- 在符号收集的基础上进行常量表达式求值
- 支持算术运算、位运算、一元运算
- 处理数组长度求值
- 集成 ConstValue 系统，为常量符号赋值

### 第三阶段：结构体检查 (Struct Checking)
**组件**: [`StructChecker`](include/semantic/struct_checker.hpp:19)
- 检查所有类型的存在性和可见性
- 处理 impl 块与结构体的集成
- 验证 trait 实现的完整性
- 确保类型引用的正确性

### 第四阶段：类型检查 (Type Checking)
**组件**: [`TypeChecker`](include/semantic/type_checker.hpp:13)
- 进行完整的类型推断和兼容性检查
- 验证表达式类型匹配
- 检查函数调用参数类型
- 处理控制流类型推断（if、loop、break、return）
- 支持变量可变性检查

## 核心特性

### 1. 完整的 Rust 子集支持
- **基本类型**: bool, i32, u32, isize, usize, char, str
- **复合类型**: 引用、数组、结构体、枚举
- **控制流**: if 表达式、循环表达式、break/return
- **函数系统**: 普通函数、方法、关联函数
- **面向对象**: 结构体、特征、impl 块

### 2. 高级语义功能
- **方法类型识别**: 支持 self, &self, &mut self, mut self 等形式
- **作用域管理**: 完整的作用域链和符号遮蔽
- **常量求值**: 编译时常量表达式计算
- **类型推断**: 自动类型提升和上下文推断
- **可变性检查**: Rust 所有权系统的基础支持

### 3. 错误处理和诊断
- 统一的错误报告机制
- 详细的类型不匹配信息
- 符号未定义错误检测
- 类型存在性验证

## 设计原则

### 1. 模块化设计
- 每个组件职责单一，接口清晰
- 组件间通过符号表和作用域系统通信
- 支持独立测试和维护

### 2. 访问者模式
- 所有分析器都继承自 `ASTVisitor`
- 统一的 AST 遍历接口
- 易于扩展新的分析功能

### 3. 类型安全
- 使用强类型系统表示符号和值
- 编译时类型检查
- 避免运行时类型错误

### 4. 性能优化
- 使用哈希表实现 O(1) 符号查找
- 共享指针管理内存，避免拷贝
- 分类符号表提高查找效率

## 文件结构

```
include/semantic/
├── symbol.hpp           # 符号类型定义
├── scope.hpp            # 作用域管理
├── const_value.hpp      # 常量值表示
├── const_evaluator.hpp  # 常量求值器
├── symbol_collector.hpp # 符号收集器
├── struct_checker.hpp   # 结构体检查器
├── type_checker.hpp     # 类型检查器
└── utils.hpp           # 工具函数

src/semantic/
├── symbol.cpp           # 符号类型实现
├── scope.cpp            # 作用域管理实现
├── const_value.cpp      # 常量值实现
├── const_evaluator.cpp  # 常量求值器实现
├── symbol_collector.cpp # 符号收集器实现
├── struct_checker.cpp   # 结构体检查器实现
├── type_checker.cpp     # 类型检查器实现
└── (utils.hpp 为头文件实现)

docs/semantic/
├── README.md           # 本文档
├── symbol.md           # 符号系统详细文档
├── const_value.md      # 常量求值文档
├── struct_checker.md   # 结构体检查器文档
└── type_checker.md     # 类型检查器文档
```

## 使用示例

```cpp
// 创建符号收集器并进行符号收集
SymbolCollector collector;
collector.visit(crate_node);
auto root_scope = collector.getRootScope();

// 进行常量求值
ConstEvaluator const_evaluator(root_scope);
const_evaluator.visit(crate_node);

// 进行结构体检查
StructChecker struct_checker(root_scope);
struct_checker.visit(crate_node);

// 进行类型检查
TypeChecker type_checker(root_scope);
type_checker.visit(crate_node);
```

## 当前实现状态

### ✅ 已完成的功能
1. **符号系统**: 完整的符号类型定义和管理
2. **作用域管理**: 分层作用域和符号查找
3. **符号收集**: AST 遍历和符号表构建
4. **常量求值**: 编译时常量表达式计算
5. **结构体检查**: 类型存在性验证和 impl 块处理
6. **类型检查**: 完整的类型推断和兼容性检查
7. **工具函数**: 类型转换和辅助操作

### 🔄 部分实现的功能
1. **借用检查**: 基础的引用类型处理
2. **所有权系统**: 可变性检查的基础框架
3. **数组处理**: 基本的数组类型支持

### ❌ 待实现的功能
1. **生命周期检查**: 完整的生命周期分析
2. **泛型支持**: 泛型类型参数和约束
3. **模式匹配**: 完整的模式类型检查
4. **宏系统**: 宏展开和类型检查
5. **完整的借用检查**: 详细的借用规则验证

## 相关文档

- [词法分析器文档](../lexer/README.md): 了解词法分析阶段
- [语法分析器文档](../parser/README.md): 了解语法分析和 AST 生成
- [符号系统详细文档](symbol.md): 深入了解符号系统的实现
- [类型检查器文档](type_checker.md): 详细了解类型检查的实现
- [常量求值文档](const_value.md): 了解常量表达式求值
- [结构体检查器文档](struct_checker.md): 了解结构体相关的语义检查

## 扩展指南

### 添加新的符号类型
1. 在 [`symbol.hpp`](include/semantic/symbol.hpp:1) 中定义新的符号类
2. 在 [`scope.hpp`](include/semantic/scope.hpp:1) 中添加相应的管理方法
3. 在 [`symbol_collector.cpp`](src/semantic/symbol_collector.cpp:1) 中添加收集逻辑
4. 更新相关的检查器以支持新符号类型

### 添加新的类型检查规则
1. 在 [`type_checker.cpp`](src/semantic/type_checker.cpp:1) 中添加相应的 visit 方法
2. 实现类型推断和兼容性检查逻辑
3. 添加必要的错误处理和诊断信息
4. 编写测试用例验证新规则

### 扩展常量求值
1. 在 [`const_value.hpp`](include/semantic/const_value.hpp:1) 中定义新的常量值类型
2. 在 [`utils.hpp`](include/semantic/utils.hpp:1) 中的 `createConstValueFromExpression` 添加处理逻辑
3. 在 [`const_evaluator.cpp`](src/semantic/const_evaluator.cpp:1) 中添加相应的 visit 方法
4. 确保类型安全和错误处理

---

Semantic 模块为 Rust 子集编译器提供了完整的语义分析基础，确保代码在类型安全、作用域规则和语言规范方面的正确性。通过模块化设计和清晰的接口，该模块易于维护和扩展，为后续的编译器开发奠定了坚实的基础。