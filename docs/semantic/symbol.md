# 符号类型枚举

本文档详细列举了在 Rust 子集编译器中需要收集的所有符号类型，为符号收集阶段提供完整的参考。

## 符号分类体系

### 1. 声明类符号 (Declaration Symbols)

#### 1.1 顶级声明 (Top-level Declarations)

##### 函数符号 (Function Symbols)
- **位置**: [`Function`](include/parser/astnode.hpp:40) 节点
- **标识符**: `function.identifier`
- **符号信息**:
  - 函数名
  - 是否为 const 函数 (`function.is_const`)
  - 参数列表 (`function.function_parameters`)
  - 返回类型 (`function.function_return_type`)
  - 函数体 (`function.block_expression`)
- **作用域**: 全局作用域或 impl/trait 作用域
- **示例**:
  ```rust
  fn add(x: i32, y: i32) -> i32 { x + y }
  const fn const_add(x: i32, y: i32) -> i32 { x + y }
  ```

##### 结构体符号 (Struct Symbols)
- **位置**: [`Struct`](include/parser/astnode.hpp:63) -> [`StructStruct`](include/parser/astnode.hpp:197) 节点
- **标识符**: `struct_struct.identifier`
- **符号信息**:
  - 结构体名
  - 字段列表 (`struct_struct.struct_fields`)
- **作用域**: 全局作用域
- **示例**:
  ```rust
  struct Point {
      x: i32,
      y: i32,
  }
  ```

##### 枚举符号 (Enum Symbols)
- **位置**: [`Enumeration`](include/parser/astnode.hpp:74) 节点
- **标识符**: `enumeration.identifier`
- **符号信息**:
  - 枚举名
  - 变体列表 (`enumeration.enum_variants`)
- **作用域**: 全局作用域
- **示例**:
  ```rust
  enum Color {
      Red,
      Green,
      Blue,
  }
  ```

##### 常量符号 (Constant Symbols)
- **位置**: [`ConstantItem`](include/parser/astnode.hpp:86) 节点
- **标识符**: `constant_item.identifier`
- **符号信息**:
  - 常量名
  - 类型 (`constant_item.type`)
  - 初始值 (`constant_item.expression`)
- **作用域**: 全局作用域或 trait/impl 作用域
- **示例**:
  ```rust
  const MAX_SIZE: usize = 1024;
  ```

##### 特征符号 (Trait Symbols)
- **位置**: [`Trait`](include/parser/astnode.hpp:99) 节点
- **标识符**: `trait.identifier`
- **符号信息**:
  - 特征名
  - 关联项列表 (`trait.associated_item`)
- **作用域**: 全局作用域
- **示例**:
  ```rust
  trait Drawable {
      fn draw(&self);
      const DEFAULT_COLOR: Color;
  }
  ```

##### 实现符号 (Implementation Symbols)
- **位置**: [`Implementation`](include/parser/astnode.hpp:111) 节点
- **类型**: [`InherentImpl`](include/parser/astnode.hpp:265) 或 [`TraitImpl`](include/parser/astnode.hpp:276)
- **符号信息**:
  - 实现的类型 (`impl.type`)
  - 对于特征实现：特征名 (`trait_impl.identifier`)
  - 关联项列表 (`impl.associated_item`)
- **作用域**: 创建新的 impl 作用域
- **示例**:
  ```rust
  // 固有实现
  impl Point {
      fn new(x: i32, y: i32) -> Self { Point { x, y } }
  }
  
  // 特征实现
  impl Drawable for Point {
      fn draw(&self) { /* ... */ }
      const DEFAULT_COLOR: Color = Color::Black;
  }
  ```

#### 1.2 关联项符号 (Associated Item Symbols)

##### 关联常量符号 (Associated Constant Symbols)
- **位置**: [`AssociatedItem`](include/parser/astnode.hpp:254) -> [`ConstantItem`](include/parser/astnode.hpp:86)
- **标识符**: `constant_item.identifier`
- **符号信息**:
  - 常量名
  - 类型
  - 初始值（如果在 trait 中可能为空）
- **作用域**: trait 或 impl 作用域
- **示例**:
  ```rust
  trait Math {
      const PI: f64;
  }
  
  impl Math for f64 {
      const PI: f64 = 3.14159;
  }
  ```

##### 关联函数符号 (Associated Function Symbols)
- **位置**: [`AssociatedItem`](include/parser/astnode.hpp:254) -> [`Function`](include/parser/astnode.hpp:40)
- **标识符**: `function.identifier`
- **符号信息**:
  - 函数名
  - 参数列表（可能包含 self 参数）
  - 返回类型
- **作用域**: trait 或 impl 作用域
- **示例**:
  ```rust
  trait Clone {
      fn clone(&self) -> Self;
  }
  
  impl Clone for Point {
      fn clone(&self) -> Self { Point { x: self.x, y: self.y } }
  }
  ```

### 2. 变量类符号 (Variable Symbols)

#### 2.1 函数参数符号 (Function Parameter Symbols)

##### Self 参数符号 (Self Parameter Symbols)
- **位置**: [`FunctionParameters`](include/parser/astnode.hpp:121) -> [`SelfParam`](include/parser/astnode.hpp:133)
- **类型**: [`ShorthandSelf`](include/parser/astnode.hpp:144) 或 [`TypedSelf`](include/parser/astnode.hpp:156)
- **符号信息**:
  - `self`, `&self`, `&mut self`, `self: Type`, `&self: Type`, `&mut self: Type`
- **作用域**: 函数作用域
- **示例**:
  ```rust
  impl Point {
      fn method1(self) { /* ... */ }           // by value
      fn method2(&self) { /* ... */ }          // by reference
      fn method3(&mut self) { /* ... */ }      // by mutable reference
      fn method4(self: Box<Self>) { /* ... */ } // explicit type
  }
  ```

##### 普通参数符号 (Regular Parameter Symbols)
- **位置**: [`FunctionParameters`](include/parser/astnode.hpp:121) -> [`FunctionParam`](include/parser/astnode.hpp:170)
- **标识符**: 从 [`PatternNoTopAlt`](include/parser/astnode.hpp:701) 中提取
- **符号信息**:
  - 参数名
  - 参数类型 (`function_param.type`)
- **作用域**: 函数作用域
- **示例**:
  ```rust
  fn process(x: i32, y: String, z: &mut Vec<i32>) { /* ... */ }
  // 参数符号: x (i32), y (String), z (&mut Vec<i32>)
  ```

#### 2.2 局部变量符号 (Local Variable Symbols)

##### Let 绑定符号 (Let Binding Symbols)
- **位置**: [`LetStatement`](include/parser/astnode.hpp:300) 节点
- **标识符**: 从 [`PatternNoTopAlt`](include/parser/astnode.hpp:701) 中提取
- **符号信息**:
  - 变量名
  - 类型注解 (`let_statement.type`，可选）
  - 初始值 (`let_statement.expression`，可选)
- **作用域**: 当前块作用域
- **示例**:
  ```rust
  let x = 42;                    // x: i32 (推断)
  let y: String = "hello".to_string();  // y: String (显式)
  let (a, b) = (1, 2);          // a: i32, b: i32 (模式匹配)
  let ref z = x;                 // z: &i32 (引用模式)
  ```

##### 模式绑定符号 (Pattern Binding Symbols)
- **位置**: 各种模式节点中
- **类型**: [`IdentifierPattern`](include/parser/astnode.hpp:712), [`ReferencePattern`](include/parser/astnode.hpp:725)
- **符号信息**:
  - 绑定名
  - 是否为引用 (`is_ref`)
  - 是否可变 (`is_mutable`)
- **作用域**: 当前块作用域
- **示例**:
  ```rust
  let x = 42;                    // IdentifierPattern
  let ref y = x;                 // ReferencePattern -> IdentifierPattern
  let &mut z = &mut value;       // ReferencePattern -> IdentifierPattern
  ```

### 3. 结构体字段符号 (Struct Field Symbols)

#### 3.1 结构体字段定义符号
- **位置**: [`StructField`](include/parser/astnode.hpp:220) 节点
- **标识符**: `struct_field.identifier`
- **符号信息**:
  - 字段名
  - 字段类型 (`struct_field.type`)
- **作用域**: 结构体定义内部（特殊的符号空间）
- **示例**:
  ```rust
  struct Person {
      name: String,      // 字段符号: name (String)
      age: u32,          // 字段符号: age (u32)
      address: Option<String>, // 字段符号: address (Option<String>)
  }
  ```

### 4. 枚举变体符号 (Enum Variant Symbols)

#### 4.1 枚举变体定义符号
- **位置**: [`EnumVariant`](include/parser/astnode.hpp:243) 节点
- **标识符**: `enum_variant.identifier`
- **符号信息**:
  - 变体名
  - 变体类型（当前实现中为简单变体，无数据）
- **作用域**: 枚举定义内部
- **示例**:
  ```rust
  enum Status {
      Active,      // 变体符号: Active
      Inactive,    // 变体符号: Inactive
      Pending,     // 变体符号: Pending
  }
  ```

### 5. 特殊符号 (Special Symbols)

#### 5.1 Self 类型符号
- **位置**: impl 块中
- **符号信息**:
  - Self 类型定义
  - 在 trait impl 中，Self 指代实现该 trait 的类型
- **作用域**: impl 作用域
- **示例**:
  ```rust
  impl Point {
      fn new() -> Self { Point { x: 0, y: 0 } }  // Self 指代 Point
  }
  
  impl Clone for Point {
      fn clone(&self) -> Self { /* ... */ }      // Self 指代 Point
  }
  ```

#### 5.2 内置类型符号 (Built-in Type Symbols)
- **位置**: 预定义
- **符号信息**:
  - 基础类型：i32, u32, f64, bool, char, str
  - 容器类型：Box, Vec, Option, Result
  - 引用类型：&, &mut
- **作用域**: 全局预定义作用域
- **示例**:
  ```rust
  let x: i32 = 42;        // i32 是内置类型
  let y: Vec<String>;     // Vec 是内置类型
  let z: Option<i32>;     // Option 是内置类型
  ```

## 符号属性总结

每个符号需要记录以下属性：

### 基本属性
- **名称** (name): 符号的标识符
- **类型** (type): 符号的类型信息
- **种类** (kind): 符号的种类（函数、变量、结构体等）
- **作用域** (scope): 符号所属的作用域
- **可见性** (visibility): 符号的可见性（pub, private 等）

### 位置信息
- **定义位置** (definition_location): 源码中的位置
- **声明节点** (declaration_node): 对应的 AST 节点

### 特殊属性
- **可变性** (mutability): 对于变量符号
- **常量性** (constness): 对于函数和常量
- **参数列表** (parameters): 对于函数符号
- **返回类型** (return_type): 对于函数符号
- **字段列表** (fields): 对于结构体符号
- **变体列表** (variants): 对于枚举符号
- **Self 类型** (self_type): 对于 impl 块

## 符号收集策略

### 按作用域收集
1. **全局作用域**: 顶级函数、结构体、枚举、常量、特征
2. **函数作用域**: 函数参数、局部变量
3. **块作用域**: let 绑定、嵌套块中的符号
4. **特征作用域**: 关联函数、关联常量
5. **实现作用域**: 关联函数、关联常量、Self 类型
6. **循环作用域**: 循环特有的符号（如循环变量）

### 按遍历顺序收集
1. 首先收集类型定义（结构体、枚举、特征）
2. 然后收集函数签名（不包括函数体）
3. 最后收集函数体和块中的局部符号

### 处理重复定义
- 同一作用域内的重复定义应报错
- 不同作用域的同名符号允许存在（遮蔽）
- 内层作用域的符号遮蔽外层作用域的同名符号

这个符号类型枚举为符号收集阶段提供了完整的参考，确保不会遗漏任何需要收集的符号信息。