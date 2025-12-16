# RCompiler 开发计划

## 近期目标

### 1. 完成表达式解析功能
#### 1.0 字面量表达式（已完成）
- [x] 实现所有字面量类型直接继承 ASTNode：`CharLiteral`、`StringLiteral`、`RawStringLiteral`、`CStringLiteral`、`RawCStringLiteral`、`IntegerLiteral`、`BoolLiteral`
- [x] 实现各个具体字面量类型的解析函数：`parseCharLiteral()`、`parseStringLiteral()`、`parseRawStringLiteral()`、`parseCStringLiteral()`、`parseRawCStringLiteral()`、`parseIntegerLiteral()`、`parseBoolLiteral()`
- [x] 为 Pratt parsing 做准备，删除了 `LiteralExpression` 组合类

#### 1.1 基础表达式类型
- [ ] 实现 `PathExpression` 解析
- [ ] 实现 `OperatorExpression` 解析（包括运算符优先级）
- [ ] 实现 `GroupedExpression` 解析（括号表达式）

#### 1.2 复合表达式类型
- [ ] 实现 `ArrayExpression` 解析
- [ ] 实现 `IndexExpression` 解析
- [ ] 实现 `StructExpression` 解析
- [ ] 实现 `CallExpression` 解析
- [ ] 实现 `MethodCallExpression` 解析
- [ ] 实现 `FieldExpression` 解析

#### 1.3 控制流表达式
- [x] 实现 `ContinueExpression` 解析
- [x] 实现 `BreakExpression` 解析
- [x] 实现 `ReturnExpression` 解析
- [x] 实现 `IfExpression` 解析

### 2. 完成块表达式和循环
- [x] 实现 `BlockExpression` 解析（语句块）
- [x] 实现 `LoopExpression` 解析（循环表达式）
- [x] 实现 `IfExpression` 解析（条件表达式）
- [x] 完善 `ExpressionWithBlock` 解析（支持 BlockExpression、LoopExpression、IfExpression）

### 3. 完成类型解析功能
#### 3.1 基础类型
- [ ] 实现 `Type` 解析（类型路径）
- [ ] 实现 `ReferenceType` 解析（引用类型 &T, &mut T）
- [ ] 实现 `ArrayType` 解析（数组类型 [T; N]）
- [ ] 完善 `UnitType` 解析（单元类型 ()）

#### 3.2 复杂类型
- [ ] 实现元组类型解析
- [ ] 实现函数指针类型解析
- [ ] 实现泛型类型参数解析

### 4. 完成模式解析功能
- [ ] 实现 `PatternNoTopAlt` 解析
- [ ] 支持字面量模式
- [ ] 支持标识符模式
- [ ] 支持通配符模式
- [ ] 支持结构体模式
- [ ] 支持枚举模式

### 5. 完善表达式解析集成
- [ ] 更新 `parseExpressionWithoutBlock` 支持所有无块表达式
- [ ] 更新 `parseExpressionWithBlock` 支持所有有块表达式
- [ ] 实现表达式优先级处理
- [ ] 添加表达式类型检查

## 中期目标

### 6. 实现访问者模式
- [ ] 设计并实现 `ASTVisitor` 基类
- [ ] 为所有 AST 节点实现 visit 方法
- [ ] 实现具体的访问者类：
  - [ ] `ASTPrinter`：用于打印 AST 结构
  - [ ] `TypeChecker`：用于类型检查
  - [ ] `CodeGenerator`：用于代码生成

### 7. 错误处理和恢复
- [ ] 改进解析错误信息
- [ ] 实现错误恢复机制
- [ ] 添加错误位置信息
- [ ] 实现警告系统

### 8. 测试和验证
- [ ] 编写单元测试
- [ ] 编写集成测试
- [ ] 添加基准测试
- [ ] 创建测试用例集合

## 长期目标

### 9. 语义分析
- [ ] 实现符号表管理
- [ ] 实现作用域分析
- [ ] 实现类型推断
- [ ] 实现所有权检查

### 10. 代码生成
- [ ] 设计中间表示（IR）
- [ ] 实现 LLVM 后端
- [ ] 实现优化 passes
- [ ] 生成可执行文件

### 11. 标准库支持
- [ ] 实现基础类型和函数
- [ ] 实现 I/O 操作
- [ ] 实现集合类型
- [ ] 实现并发原语

## 开发优先级

1. **高优先级**：完成基础表达式和类型解析（目标 1.1-4）
2. **中优先级**：实现访问者模式和错误处理（目标 5-7）
3. **低优先级**：测试、语义分析和代码生成（目标 8-11）

## 技术债务

- [ ] 重构解析器以减少代码重复
- [ ] 优化内存使用和性能
- [ ] 改进代码文档
- [ ] 添加更多配置选项
- [ ] 扩展字面量表达式以支持复合字面量（如带有后缀的数字字面量）

## 里程碑

- **v0.1**：完成基础语法解析（目标 1.1-4）
- **v0.2**：实现访问者模式和基础工具（目标 5-7）
- **v0.3**：完整的解析器和类型检查器（目标 8）
- **v0.4**：基础代码生成（目标 9-10）
- **v1.0**：可用的编译器（目标 11）

## 已完成功能

### 表达式类层次结构重构
- 所有表达式类现在都继承自 `Expression` 基类
- 包括字面量类、控制流表达式、复合表达式等
- 为 Pratt parsing 做好了架构准备

### 字面量表达式系统
- 所有 Rust 字面量类型的支持，直接继承 Expression，包含各自的值
- 完整的解析函数集合，支持所有字面量类型的独立解析
- 为 Pratt parsing 做准备，删除了 `LiteralExpression` 组合类

### Condition 和 IfExpression 系统
- 实现了 `Condition` 类，继承自 Expression，专门处理 if 条件
- 实现了 `IfExpression` 类，包含条件、then 块和可选的 else 分支
- 支持条件表达式（但不能是 StructExpression，只可能出现在 ExpressionWithoutBlock 中），通过专门的 `parseCondition()` 函数处理
- 支持 `if condition { }` 和 `if condition { } else { }` 以及 `if condition { } else if { }` 形式
- `else_branch` 现在是 `Expression` 类型，支持 `BlockExpression` 或另一个 `IfExpression`
- 集成到 `parseExpressionWithoutBlock()` 中

### ReturnExpression 系统
- 实现了 `ReturnExpression` 类，包含可选的表达式子节点
- 支持 `return;` 和 `return expression;` 两种形式
- 集成到 `parseExpressionWithoutBlock()` 中

### 循环表达式系统
- 实现了 `LoopExpression` 类，包含 InfiniteLoopExpression 或 PredicateLoopExpression 子节点
- 实现了 `InfiniteLoopExpression` 类，支持 `loop { }` 形式的无限循环
- 实现了 `PredicateLoopExpression` 类，支持 `while condition { }` 形式的条件循环
- 集成到 `parseExpressionWithBlock()` 中

### 控制流表达式系统
- 实现了 `BreakExpression` 类，支持可选的表达式子节点，支持 `break;` 和 `break expression;` 两种形式
- 实现了 `ContinueExpression` 类，支持 `continue;` 形式
- 已实现但等待 Pratt parsing 系统集成

### 表达式结构重构
- 重新定义了 `ExpressionWithBlock` 的正则表达式：BlockExpression | LoopExpression | IfExpression
- 更新了 `parseExpressionWithBlock()` 函数以正确支持这三种表达式类型
- 简化了 `parseExpressionWithoutBlock()` 函数，为未来的 Pratt parsing 实现做准备
- `IfExpression` 从 `ExpressionWithoutBlock` 移动到 `ExpressionWithBlock` 中，符合 Rust 语法规范

### BlockExpression 系统
- 实现了 `Statements` 类，支持 Statement+ 或 Statement+ ExpressionWithoutBlock 或 ExpressionWithoutBlock 的解析
- 实现了 `parseStatements()` 函数，按照要求先尝试 parseStatement，失败时尝试 parseExpressionWithoutBlock
- 实现了 `parseBlockExpression()` 函数，支持 `{ Statements? }` 语法
- 集成到 `parseExpressionWithBlock()` 中，完善了有块表达式的解析能力