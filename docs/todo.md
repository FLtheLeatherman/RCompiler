本项目是一个 Rust 子集的编译器前端实现，目前已经实现 lexer 和 parser，可以从代码生成一棵 AST 树。具体的，你应当参考 docs/lexer/README.md 和 docs/parser/README.md. 同时，我们已经完成了 semantic 中初步的符号收集的基础部分，具体可以参考 docs/semantic/symbol.md.

然后，我完成了 struct 相关字段、方法和函数的检查. 具体的，我修改了 StructSymbol 的实现，完成了 StructChecker。具体步骤如下：

我们把所有 associated item 都存入 struct 中，也因此对 StructSymbol 进行改造。改造之后，它当中可以存储所有的：原本的 struct field 中的内容，以及 impl 中的 const item，和 function，其中 function 被区分为 方法 和 普通的函数。这个可以参考 semantic/symbol.hpp 和 semantic/symbol.cpp.

在每个 scope，构建具体的 struct 的字段、方法和函数的信息。

对于 struct 中的字段、 impl/ trait 中的关联函数参数和返回值 / 关联const，以及任何用到类型的地方，检查类型是否存在/可见。

对于 impl ，先寻找它所对应的 struct 类型和 trait （如果是某个 trait 的 impl），然后在那个 struct 中添加 associated item ；同时检查 trait 中未定义的 associated items 是否全部实现。

如果在当前 scope 或它的所有 parent scope 中找不到这个 const /struct / trait，报错 Undefined Name。（throw std::runtime_error("Undefined Name");）

依据上述内容，我实现了 StructChecker 类，具体可以参考 semantic/struct_checker.hpp 和 semantic/struct_checker.cpp.

现在，你的任务是读取我的代码文件，不修改代码，然后仅更新文档：docs/semantic/symbol.md，并创建 docs/semantic/struct_checker.md，描述目前做了什么事情。