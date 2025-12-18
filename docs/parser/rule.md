可以使用 parser 中的 peek() 函数判断下一个 token 的类型，具体类型对应 应当阅读 lexer.hpp 中的内容以获取准确名称，例如，`return` 对应着 Token::kReturn.

parser 最后生成的 AST 树是一个树状结构，每个节点（除 Expression 相关外）的类型都应该是**直接**从 ASTNode 基类派生而来，包含构造函数和继承的 visit 函数；

上层 AST 节点应有对应的 child 以及可能的对应信息; 底层的 AST 节点只需要存储对应信息，比如 CharLiteral 这种，只需要存储一个字符串。

完成相关内容时，应当去 astnode.hpp 修改对应的声明，并到 parser.hpp 和 parser.cpp 中修改对应的接口和实现。

所有 astnode.hpp 中新增加的声明应当到 include/utils.hpp 中添加，便于通过编译。

对于 Expression 相关，我们会采用 pratt parsing 的方式。这一部分到最后可能需要修改，现在不需理会。

如果 parse 函数失败，你应当抛出异常，并且做好异常处理。

暂时，你不需要进行测试。
