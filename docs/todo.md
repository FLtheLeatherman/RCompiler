本项目是一个 Rust 子集的编译器前端实现，目前已经实现 lexer 和 parser，可以从代码生成一棵 AST 树。具体的，你应当参考 docs/lexer/README.md 和 docs/parser/README.md. 

同时，我们已经完成了 semantic 中初步的符号收集。具体可以参考 semantic/docs/symbol.md.

接着，我们还完成了 semantic 中初步的对 struct 进行集成，具体可以参考 semantic/docs/struct_checker.md.

接下来，我们要继续完成 struct_checker 的其他部分。具体的，你应当把所有虚函数都实现。

