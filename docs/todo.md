本项目是一个 Rust 子集的编译器前端实现，目前已经实现 lexer 和 parser，可以从代码生成一棵 AST 树。具体的，你应当参考 docs/lexer/README.md 和 docs/parser/README.md. 

同时，我们已经完成了 semantic 中初步的符号收集。具体可以参考 semantic/docs/symbol.md.

接着，我们还完成了 semantic 中初步的对 struct 进行集成，具体可以参考 semantic/docs/struct_checker.md.

接下来要做的是：

1. 如何将 func_params 塞入 function 的 scope 中？是不是要开创一个叫 var_symbols 的东西并进行维护，还是等到第四步再维护？

2. struct_checker 中的 type existence check 未完成，比如 struct fields 中的东西。

3. type existence check 的底层逻辑：checkTypeExists 并不是一个完备的东西：特别是面对数组类型。它有可能是一个高维数组。这里的处理仍然不足。
这里其实涉及到一个更深的问题，也就是对于 array symbols 的处理。记得要在 constant evaluation 之内处理掉它。