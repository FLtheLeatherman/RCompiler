本项目是一个 Rust 子集的编译器前端实现，目前已经实现 lexer 和 parser，可以从代码生成一棵 AST 树。具体的，你应当参考 docs/lexer/README.md 和 docs/parser/README.md. 

同时，我们已经完成了 semantic 中初步的符号收集。具体可以参考 semantic/docs/symbol.md.

接着，我们还完成了 semantic 中初步的对 struct 进行集成，具体可以参考 semantic/docs/struct_checker.md.

接下来要做的是，常量表达式求值。我已经做了一些初步的工作，你可以查看 semantic/const_value.hpp 和 semantic/const_value.cpp，了解当前的 const_value 类型的基本实现，还可以查看 semantic/const_evaluator.hpp 和 semantic/const_value.md 了解我们目前实现的一个函数：createConstValueFromExpression，来了解其如何从 expression 中求值。

接下来：修改 array symbol，完成配套的工作。