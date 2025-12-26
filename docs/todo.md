本项目是一个 Rust 子集的编译器前端实现，目前已经实现 lexer 和 parser，可以从代码生成一棵 AST 树。具体的，你应当参考 docs/lexer/README.md 和 docs/parser/README.md. 

同时，我们已经完成了 semantic 中初步的符号收集、常量求值、struct 集成。具体可以参考 semantic/docs/symbol.md, semantic/docs/const_value.md, semantic/docs/symbol.md.

现在，我们正在进行 semantic 中最关键的 type_check. 目前 type_check 的进度可以参考 semantic/docs/type_checker.md. 我们将要实现的语句是 Unary Expr. 具体的，'-' 可以作用到 integer（或 i32, u32, isize, usize） 上，而 '!' 可以作用到 integer 或者 bool 上，其它类型均不可以被 Unary Expr 访问。不用实现 '?' 相关。最后，Unary Expr node 的类型应当与其成员 expr 相同。

实现完成后，更新文档。
