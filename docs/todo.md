本项目是一个 Rust 子集的编译器前端实现，目前已经实现 lexer 和 parser，可以从代码生成一棵 AST 树。具体的，你应当参考 docs/lexer/README.md 和 docs/parser/README.md. 

同时，我们已经完成了 semantic 中初步的符号收集（symbol_collector）、常量求值（const_evaluator）、struct 集成（struct_checker）、类型检查（type_checker）。你可以通过读取 semantic 文件夹中的文档来了解这一点。

现在，我希望你在 main.cpp 中帮我添加一些内建的函数。我在 main.cpp 中已经有了一些例子，你可以参考一下，来了解如何加入这些函数。对于方法型的函数，你可以先创建一个对应的 struct，再往这个 struct 里面塞入这些方法。

内建函数列表如下：
```rust
fn getString() -> String
fn getInt() -> i32
fn exit(code: i32) -> ()
fn to_string(&self) -> String (Available on: u32, usize)
fn as_str(&self) -> &str (Available on: String)
fn len(&self) -> u32 (Available on: String, &str)
```