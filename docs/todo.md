本项目是一个 Rust 子集的编译器前端实现，目前已经实现 lexer 和 parser，可以从代码生成一棵 AST 树。具体的，你应当参考 docs/lexer/README.md 和 docs/parser/README.md. 

同时，我们已经完成了 semantic 中初步的符号收集。具体可以参考 semantic/docs/symbol.md.

接着，我们还完成了 semantic 中初步的对 struct 进行集成，具体可以参考 semantic/docs/struct_checker.md.

接下来要做的是，常量表达式求值。我已经做了一些初步的工作，你可以查看 semantic/const_value.hpp 和 semantic/const_value.cpp，了解当前的 const_value 类型的基本实现，还可以查看 semantic/const_evaluator.hpp 和 semantic/const_value.md 了解我们目前实现的一个函数：createConstValueFromExpression，来了解其如何从 expression 中求值。同时，我们还有一个辅助函数 handleArraySymbol，用于将 ArrayType 中代表数组长度的常量表达式也进行处理。

有了以上的辅助函数之后，我只需要在 const_evaluator.cpp 中完成 ConstEvaluator 的剩余实现。这里的实现仍旧要求 AST 和 Scope 同进退的原则，可以参考 semantic/docs/struct_checker.md 来了解如何实现这一点。目前，我已经实现了 visit(Crate&), visit(ConstantItem&), visit(Function&), visit(StructStruct&)。我需要你实现其他部分的 visit 函数，继承自 ASTVisitor，使其能够同时遍历整个 AST 和 scope 树。注意，只是实现遍历，而不作其他的任何操作。

现在，你不需要实现全部的 visit，而是只需要实现这些：

```cpp
    // 声明类节点 (Items)
    virtual void visit(Function&) {}
    virtual void visit(Struct&) {}
    virtual void visit(Enumeration&) {}
    virtual void visit(ConstantItem&) {}
    virtual void visit(Trait&) {}
    virtual void visit(Implementation&) {}
    
    // 函数相关节点
    virtual void visit(FunctionParameters&) {}
    virtual void visit(SelfParam&) {}
    virtual void visit(ShorthandSelf&) {}
    virtual void visit(TypedSelf&) {}
    virtual void visit(FunctionParam&) {}
    virtual void visit(FunctionReturnType&) {}
    
    // 结构体相关节点
    virtual void visit(StructStruct&) {}
    virtual void visit(StructFields&) {}
    virtual void visit(StructField&) {}
```


你先实现好这些 visit 函数之后，帮我思考一下还有哪些地方可能出现 ArraySymbol 的，把这些输出到 docs/todo3.md 里。

实现完成后，更新 semantic/const_value.md.