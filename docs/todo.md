本项目是一个 Rust 子集的编译器前端实现，目前已经实现 lexer 和 parser，可以从代码生成一棵 AST 树。

我现在的需求是，实现基于 visitor 实现一个可视化工具。

我已经实现了 visitor 作为基类的接口，在 src/parser/visitor.hpp 中。

同时我也已经实现了 astprinter : visitr 的接口，在 src/parser/astprinter.hpp 中。

根据这些接口完成 astprinter 的具体实现，在 src/parser/astprinter.cpp 中。

你可以阅读 docs/lexer/README.md 和 docs/parser/README.md，了解项目概况；如有需要，也可以进一步阅读代码来了解结构。

你可能会需要 include/parser/utils.hpp 来了解类型的声明。你可能会需要 include/parser/astnode.hpp 来了解各个具体节点是怎么实现的。