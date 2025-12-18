# RCompiler 开发计划

## 近期目标

### 1. 完成表达式解析功能
#### 1.0 字面量表达式（已完成）
- [x] 实现所有字面量类型直接继承 ASTNode：`CharLiteral`、`StringLiteral`、`RawStringLiteral`、`CStringLiteral`、`RawCStringLiteral`、`IntegerLiteral`、`BoolLiteral`
- [x] 实现各个具体字面量类型的解析函数：`parseCharLiteral()`、`parseStringLiteral()`、`parseRawStringLiteral()`、`parseCStringLiteral()`、`parseRawCStringLiteral()`、`parseIntegerLiteral()`、`parseBoolLiteral()`
- [x] 为 Pratt parsing 做准备，删除了 `LiteralExpression` 组合类

#### 1.1 基础表达式类型（已完成）
- [x] 实现 `PathExpression` 解析
- [x] 实现 `AssignmentExpression`、`CompoundAssignmentExpression`、`BinaryExpression`、`TypeCastExpression` 解析
- [x] 实现 `GroupedExpression` 解析（括号表达式）
- [x] 实现 Pratt parsing 基础设施（BindingPower 枚举和绑定力函数）

#### 1.2 复合表达式类型（已完成）
- [x] 实现 `ArrayExpression` 解析
- [x] 实现 `IndexExpression` 解析
- [x] 实现 `StructExpression` 解析
- [x] 实现 `CallExpression` 解析
- [x] 实现 `MethodCallExpression` 解析
- [x] 实现 `FieldExpression` 解析
- [x] 实现中缀表达式解析函数：`parseCallExpressionFromInfix()`、`parseMethodCallExpressionFromInfix()`、`parseFieldExpressionFromInfix()`、`parseIndexExpressionFromInfix()`

#### 1.3 控制流表达式（已完成）
- [x] 实现 `ContinueExpression`、`BreakExpression`、`ReturnExpression` 解析
- [x] 实现 `IfExpression` 解析
- [x] 实现 `Condition` 类，专门处理 if 条件

#### 1.4 块表达式和循环（已完成）
- [x] 实现 `BlockExpression` 解析，支持 `{ Statements? }` 语法
- [x] 实现 `LoopExpression` 解析
- [x] 实现 `InfiniteLoopExpression` 解析，支持 `loop { }` 语法
- [x] 实现 `PredicateLoopExpression` 解析，支持 `while condition { }` 语法

#### 1.5 Pratt parsing 完整集成（已完成）
- [x] 完全集成 `parsePrattExpression()` 函数，支持所有中缀表达式类型
- [x] 修正了 token 消耗逻辑错误，确保不同类型的中缀表达式使用正确的解析策略
- [x] 实现了完整的 switch 语句处理所有运算符类型
- [x] 支持正确的运算符优先级和结合性
- [x] 实现了 `parseExpressionWithoutBlock()` 函数，包含类型检查确保不包含 ExpressionWithBlock

### 2. 完成类型解析功能
#### 2.1 基础类型（已完成）
- [x] 实现 `TypePath` 解析（简单路径和泛型参数）
- [x] 实现 `ReferenceType` 解析（引用类型 &T, &mut T）
- [x] 实现 `ArrayType` 解析（数组类型 [T; N]）
- [x] 完善 `UnitType` 解析（单元类型 ()）

#### 2.2 复杂类型（未完成）
- [ ] 实现元组类型解析
- [ ] 实现函数指针类型解析
- [ ] 实现泛型类型参数解析
- [ ] 实现生命周期参数解析
- [ ] 实现动态大小类型解析

### 3. 完成模式解析功能
#### 3.1 基础模式（已完成）
- [x] 实现标识符模式（变量绑定）
- [x] 实现引用模式（& 和 && 引用）
- [ ] 实现通配符模式（_）
- [ ] 实现或模式（|）

#### 3.2 复杂模式（未完成）
- [ ] 实现结构体模式
- [ ] 实现枚举模式
- [ ] 实现切片模式
- [ ] 实现范围模式（..）

### 4. 完成名称系统
#### 4.1 基础名称（未完成）
- [ ] 实现 namespace 解析（use 语句）
- [ ] 实现 scope 解析（作用域和可见性）
- [ ] 实现 preludes 解析（外部 crate 导入）

#### 4.2 高级名称（未完成）
- [ ] 实现绝对路径解析
- [ ] 实现相对路径解析
- [ ] 实现泛型参数解析
- [ ] 实现关联类型解析
- [ ] 实现 Trait 相关名称解析

### 5. 实现访问者模式
#### 5.1 设计并实现 `ASTVisitor` 基类
- [ ] 设计并实现 `ASTPrinter`：用于打印 AST 结构
- [ ] 设计并实现 `TypeChecker`：用于类型检查和推断
- [ ] 设计并实现 `CodeGenerator`：用于生成中间表示或目标代码

### 6. 错误处理和恢复
#### 6.1 基础错误处理（未完成）
- [ ] 改进解析错误信息
- [ ] 实现错误恢复机制
- [ ] 添加错误位置信息
- [ ] 实现警告系统

#### 6.2 高级错误处理（未完成）
- [ ] 实现多错误收集和报告
- [ ] 实现错误恢复策略
- [ ] 实现上下文相关错误处理

### 7. 测试和验证
#### 7.1 基础测试（未完成）
- [ ] 编写单元测试
- [ ] 编写集成测试
- [ ] 添加基准测试
- [ ] 创建测试用例集合

#### 7.2 高级测试（未完成）
- [ ] 实现模糊测试
- [ ] 实现性能测试
- [ ] 实现回归测试
- [ ] 实现兼容性测试

### 8. 代码生成
#### 8.1 中间表示（未完成）
- [ ] 设计并实现中间表示（IR）
- [ ] 实现基础优化 passes
- [ ] 实现控制流图生成
- [ ] 实现数据流分析

#### 8.2 目标代码生成（未完成）
- [ ] 实现 LLVM 后端
- [ ] 实现优化 passes
- [ ] 实现目标代码生成
- [ ] 实现内联汇编支持
- [ ] 实现调试信息生成

### 9. 标准库支持
#### 9.1 基础库（未完成）
- [ ] 实现基础类型和函数
- [ ] 实现 I/O 操作
- [ ] 实现集合类型
- [ ] 实现字符串操作
- [ ] 实现错误处理

#### 9.2 高级库（未完成）
- [ ] 实现并发原语
- [ ] 实现网络编程支持
- [ ] 实现文件系统操作
- [ ] 实现时间处理

### 10. 工具链和集成
#### 10.1 构建系统（未完成）
- [ ] 配置 CMake 构建系统
- [ ] 添加包管理支持
- [ ] 实现持续集成
- [ ] 配置文档生成

#### 10.2 开发工具（未完成）
- [ ] 配置开发环境
- [ ] 实现代码格式化工具
- [ ] 添加调试和分析工具

## 开发优先级

1. **高优先级**：完成类型系统、模式系统、名称系统
2. **中优先级**：实现访问者模式、错误处理、测试框架
3. **低优先级**：代码生成、标准库、构建系统

## 里程碑

- **v0.1**：完成基础语法解析（词法分析器 + 基础语法分析器）
- **v0.2**：完成表达式解析和类型系统
- **v0.3**：完成模式解析和名称系统
- **v0.4**：实现访问者模式和代码生成
- **v0.5**：实现标准库支持和优化
- **v1.0**：可用的 Rust 编译器

## 技术债务

- [ ] 重构解析器以减少代码重复
- [ ] 优化内存使用和性能
- [ ] 改进代码文档
- [ ] 添加更多配置选项
- [ ] 扩展字面量表达式以支持复合字面量（如带有后缀的数字字面量）
- [ ] 移除调试代码和 std::cerr 输出
- [ ] 添加更友好的错误消息

## 当前架构

- 采用访问者模式设计 AST
- 使用智能指针（shared_ptr）管理节点生命周期
- 模块化设计，分离头文件和实现文件
- 支持 C++17 标准
- 完整的 Rust 语法子集支持
- 高效的 Pratt parsing 表达式解析系统

这个项目为 Rust 编译器的前端开发奠定了坚实的基础，已经具备了处理复杂 Rust 代码的能力。