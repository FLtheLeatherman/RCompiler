# 语义检查第一步：符号收集计划书

## 概述

本文档详细描述了 Rust 子集编译器前端语义检查的第一步：符号收集。这一步的目标是在 AST 上遍历所有节点，收集符号信息并建立 Scope 树，为后续的名称解析和类型检查奠定基础。

## 核心概念

### Scope 树结构

Scope 树是语义分析的核心数据结构，用于管理符号的作用域和可见性。根据需求，我们需要为以下结构创建 Scope：

1. **BlockExpression Scope**：最基本的块作用域
2. **Function Scope**：函数作用域，包含函数参数信息
3. **Trait Scope**：特征作用域，包含关联常量和关联函数
4. **Impl Scope**：实现作用域，记录 Self 类型
5. **LoopExpression Scope**：循环作用域

### Scope 树节点设计

```cpp
enum class ScopeType {
    Global,      // 全局作用域
    Block,       // 块作用域
    Function,    // 函数作用域
    Trait,       // 特征作用域
    Impl,        // 实现作用域
    Loop         // 循环作用域
};

struct SymbolInfo {
    std::string name;
    enum class SymbolType {
        Constant,    // const item
        StructField, // struct 字段
        EnumVariant, // enum 变体
        FunctionParam, // 函数参数
        AssociatedConst, // 关联常量
        AssociatedFunction, // 关联函数
        SelfType     // Self 类型
    } type;
    std::shared_ptr<Type> type_info;  // 类型信息
    // 其他元数据（如位置信息等）
};

class Scope {
public:
    ScopeType type;
    std::shared_ptr<Scope> parent;           // 父作用域
    std::vector<std::shared_ptr<Scope>> children; // 子作用域
    std::unordered_map<std::string, SymbolInfo> symbols; // 符号表
    
    // 特殊作用域的额外信息
    std::string function_name;               // 函数作用域：函数名
    std::vector<std::pair<std::string, std::shared_ptr<Type>>> function_params; // 函数参数
    std::shared_ptr<Type> return_type;       // 函数返回类型
    std::shared_ptr<Type> self_type;         // impl 作用域的 Self 类型
    std::string trait_name;                  // trait 作用域的 trait 名
};
```

## 符号收集算法

### 主要数据结构

```cpp
class SymbolCollector : public ASTVisitor {
private:
    std::shared_ptr<Scope> current_scope;
    std::shared_ptr<Scope> global_scope;
    
public:
    SymbolCollector();
    void collect(std::shared_ptr<Crate> crate);
    
    // 重写关键的 visit 方法
    void visit(Crate& crate) override;
    void visit(BlockExpression& block_expr) override;
    void visit(Function& function) override;
    void visit(Struct& struct_def) override;
    void visit(Enumeration& enum_def) override;
    void visit(ConstantItem& const_item) override;
    void visit(Trait& trait) override;
    void visit(Implementation& impl) override;
    void visit(LoopExpression& loop_expr) override;
    // ... 其他必要的 visit 方法
};
```

### 详细算法流程

#### 1. 全局作用域初始化

```cpp
void SymbolCollector::collect(std::shared_ptr<Crate> crate) {
    // 创建全局作用域
    global_scope = std::make_shared<Scope>();
    global_scope->type = ScopeType::Global;
    global_scope->parent = nullptr;
    current_scope = global_scope;
    
    // 遍历 crate 中的所有顶级 item
    for (auto& item : crate->items) {
        item->accept(this);
    }
}
```

#### 2. BlockExpression 处理

```cpp
void SymbolCollector::visit(BlockExpression& block_expr) {
    // 创建新的块作用域
    auto new_scope = std::make_shared<Scope>();
    new_scope->type = ScopeType::Block;
    new_scope->parent = current_scope;
    current_scope->children.push_back(new_scope);
    
    // 切换到新作用域
    auto old_scope = current_scope;
    current_scope = new_scope;
    
    // 遍历块中的所有语句
    if (block_expr.statements) {
        for (auto& stmt : block_expr.statements->statements) {
            stmt->accept(this);
        }
    }
    
    // 恢复到父作用域
    current_scope = old_scope;
}
```

#### 3. 函数处理

```cpp
void SymbolCollector::visit(Function& function) {
    // 创建函数作用域
    auto func_scope = std::make_shared<Scope>();
    func_scope->type = ScopeType::Function;
    func_scope->parent = current_scope;
    func_scope->function_name = function.identifier;
    current_scope->children.push_back(func_scope);
    
    // 在当前作用域中注册函数符号
    SymbolInfo func_symbol;
    func_symbol.name = function.identifier;
    func_symbol.type = SymbolInfo::SymbolType::AssociatedFunction;
    // TODO: 设置函数类型信息
    current_scope->symbols[function.identifier] = func_symbol;
    
    // 切换到函数作用域
    auto old_scope = current_scope;
    current_scope = func_scope;
    
    // 收集函数参数
    if (function.function_parameters) {
        if (function.function_parameters->self_param) {
            // 处理 self 参数
            function.function_parameters->self_param->accept(this);
        }
        
        for (auto& param : function.function_parameters->function_param) {
            param->accept(this);
        }
    }
    
    // 记录返回类型
    if (function.function_return_type) {
        func_scope->return_type = function.function_return_type->type;
    }
    
    // 处理函数体（BlockExpression）
    if (function.block_expression) {
        function.block_expression->accept(this);
    }
    
    // 恢复到父作用域
    current_scope = old_scope;
}
```

#### 4. 结构体处理

```cpp
void SymbolCollector::visit(Struct& struct_def) {
    if (!struct_def.struct_struct) return;
    
    // 在当前作用域注册结构体符号
    SymbolInfo struct_symbol;
    struct_symbol.name = struct_def.struct_struct->identifier;
    struct_symbol.type = SymbolInfo::SymbolType::Constant; // 临时使用，后续可扩展
    current_scope->symbols[struct_symbol.name] = struct_symbol;
    
    // 收集结构体字段信息
    if (struct_def.struct_struct->struct_fields) {
        for (auto& field : struct_def.struct_struct->struct_fields->struct_fields) {
            SymbolInfo field_symbol;
            field_symbol.name = field->identifier;
            field_symbol.type = SymbolInfo::SymbolType::StructField;
            field_symbol.type_info = field->type;
            
            // 将字段信息存储在结构体符号中（需要扩展 SymbolInfo 结构）
            // 或者使用特殊的作用域来管理字段
        }
    }
}
```

#### 5. 枚举处理

```cpp
void SymbolCollector::visit(Enumeration& enum_def) {
    // 在当前作用域注册枚举符号
    SymbolInfo enum_symbol;
    enum_symbol.name = enum_def.identifier;
    enum_symbol.type = SymbolInfo::SymbolType::Constant; // 临时使用
    current_scope->symbols[enum_symbol.name] = enum_symbol;
    
    // 收集枚举变体信息
    if (enum_def.enum_variants) {
        for (auto& variant : enum_def.enum_variants->enum_variant) {
            SymbolInfo variant_symbol;
            variant_symbol.name = variant->identifier;
            variant_symbol.type = SymbolInfo::SymbolType::EnumVariant;
            
            // 将变体信息存储在枚举符号中
        }
    }
}
```

#### 6. 常量处理

```cpp
void SymbolCollector::visit(ConstantItem& const_item) {
    // 在当前作用域注册常量符号
    SymbolInfo const_symbol;
    const_symbol.name = const_item.identifier;
    const_symbol.type = SymbolInfo::SymbolType::Constant;
    const_symbol.type_info = const_item.type; // 假定类型存在
    current_scope->symbols[const_symbol.name] = const_symbol;
}
```

#### 7. 特征处理

```cpp
void SymbolCollector::visit(Trait& trait) {
    // 创建特征作用域
    auto trait_scope = std::make_shared<Scope>();
    trait_scope->type = ScopeType::Trait;
    trait_scope->parent = current_scope;
    trait_scope->trait_name = trait.identifier;
    current_scope->children.push_back(trait_scope);
    
    // 在当前作用域注册特征符号
    SymbolInfo trait_symbol;
    trait_symbol.name = trait.identifier;
    trait_symbol.type = SymbolInfo::SymbolType::Constant; // 临时使用
    current_scope->symbols[trait_symbol.name] = trait_symbol;
    
    // 切换到特征作用域
    auto old_scope = current_scope;
    current_scope = trait_scope;
    
    // 收集关联项
    for (auto& associated_item : trait.associated_item) {
        associated_item->accept(this);
    }
    
    // 恢复到父作用域
    current_scope = old_scope;
}
```

#### 8. 实现处理

```cpp
void SymbolCollector::visit(Implementation& impl) {
    // 创建实现作用域
    auto impl_scope = std::make_shared<Scope>();
    impl_scope->type = ScopeType::Impl;
    impl_scope->parent = current_scope;
    current_scope->children.push_back(impl_scope);
    
    // 处理固有实现或特征实现
    if (auto inherent_impl = std::dynamic_pointer_cast<InherentImpl>(impl.impl)) {
        // 固有实现：记录 Self 类型
        impl_scope->self_type = inherent_impl->type;
    } else if (auto trait_impl = std::dynamic_pointer_cast<TraitImpl>(impl.impl)) {
        // 特征实现：记录 Self 类型和 trait 名称
        impl_scope->self_type = trait_impl->type;
        impl_scope->trait_name = trait_impl->identifier;
    }
    
    // 切换到实现作用域
    auto old_scope = current_scope;
    current_scope = impl_scope;
    
    // 收集关联项
    if (auto inherent_impl = std::dynamic_pointer_cast<InherentImpl>(impl.impl)) {
        for (auto& associated_item : inherent_impl->associated_item) {
            associated_item->accept(this);
        }
    } else if (auto trait_impl = std::dynamic_pointer_cast<TraitImpl>(impl.impl)) {
        for (auto& associated_item : trait_impl->associated_item) {
            associated_item->accept(this);
        }
    }
    
    // 恢复到父作用域
    current_scope = old_scope;
}
```

#### 9. 循环处理

```cpp
void SymbolCollector::visit(LoopExpression& loop_expr) {
    // 创建循环作用域
    auto loop_scope = std::make_shared<Scope>();
    loop_scope->type = ScopeType::Loop;
    loop_scope->parent = current_scope;
    current_scope->children.push_back(loop_scope);
    
    // 切换到循环作用域
    auto old_scope = current_scope;
    current_scope = loop_scope;
    
    // 处理循环体
    loop_expr.child->accept(this);
    
    // 恢复到父作用域
    current_scope = old_scope;
}
```

## 完整遍历策略

### 需要遍历的关键节点

1. **Crate**：入口点，遍历所有顶级 Item
2. **BlockExpression**：创建块作用域，遍历所有语句
3. **Function**：创建函数作用域，收集参数信息，处理函数体
4. **Struct**：注册结构体，收集字段信息
5. **Enumeration**：注册枚举，收集变体信息
6. **ConstantItem**：注册常量符号
7. **Trait**：创建特征作用域，收集关联项
8. **Implementation**：创建实现作用域，收集关联项
9. **LoopExpression**：创建循环作用域
10. **IfExpression**：处理条件分支中的 BlockExpression
11. **AssociatedItem**：处理关联常量和关联函数

### 特殊情况处理

1. **嵌套 BlockExpression**：任何表达式中都可能出现 BlockExpression，需要完整遍历
2. **函数参数模式**：需要处理参数中的模式匹配
3. **Self 参数**：特殊处理不同形式的 self 参数
4. **路径表达式**：在符号收集阶段暂不处理，留到名称解析阶段

## 实现注意事项

### 1. 类型信息处理

- 在第一步中，**假定所有类型都存在且正确**
- 暂不进行类型检查，只记录类型信息
- 类型信息的完整表示需要在后续步骤中完善

### 2. 错误处理

- 符号重复定义：记录警告或错误
- 缺少必要信息：记录错误但继续处理
- 作用域嵌套错误：确保作用域树的正确性

### 3. 性能考虑

- 使用哈希表存储符号表，确保 O(1) 查找
- 合理设计 Scope 树的遍历顺序
- 避免重复遍历相同的 AST 节点

### 4. 扩展性

- SymbolInfo 结构需要支持后续的类型检查
- Scope 树需要支持名称解析的回溯查找
- 为后续的语义分析步骤预留接口

## 输出结果

符号收集阶段完成后，应该得到：

1. **完整的 Scope 树**：反映程序的嵌套结构
2. **每个 Scope 的符号表**：包含该作用域内定义的所有符号
3. **符号的基本信息**：名称、类型、定义位置等
4. **作用域间的父子关系**：支持名称解析时的回溯查找

## 后续步骤

符号收集完成后，下一步将是：

1. **名称解析**：在 Scope 树中查找符号引用
2. **类型检查**：验证类型表达式的正确性
3. **其他语义检查**：如可变性检查、生命周期检查等

## 总结

符号收集是语义分析的基础，通过建立完整的 Scope 树和符号表，为后续的名称解析和类型检查提供了必要的基础设施。这一步的关键是要完整、准确地收集所有符号信息，并正确建立作用域的层次结构。