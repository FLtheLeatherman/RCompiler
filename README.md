A C++ implementation of a compiler for a subset of the Rust programming language.

目前进度为：lexer 已经完成，正在实现 parser，目标是把 lex 好的 token 转化为一棵 AST tree，方便后续功能实现。

采用 visitor pattern，尽管 visitor 还没有写。

在 include/astnode.hpp 中，有部分目前写好的 astnode 类的声明；

在 include/parser.hpp 中，有部分目前写好的 astnode 对应的 parser 函数的声明；src/parser.cpp 是其实现。