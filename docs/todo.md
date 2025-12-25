本项目是一个 Rust 子集的编译器前端实现，目前已经实现 lexer 和 parser，可以从代码生成一棵 AST 树。具体的，你应当参考 docs/lexer/README.md 和 docs/parser/README.md. 

同时，我们已经完成了 semantic 中初步的符号收集。具体可以参考 semantic/docs/symbol.md.

现在我希望你能修改 scope 部分，让每个 scope 还能存储一个 unordered_map<std::string, std::string> variable_table，用来存储当前变量的状态，key 是 identifier, value 则是其对应的 type（我们用一个字符串表示）。同时，完成相关必要函数接口的实现。