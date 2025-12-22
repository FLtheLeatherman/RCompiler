本项目是一个 Rust 子集的编译器前端实现，目前已经实现 lexer 和 parser，可以从代码生成一棵 AST 树。具体的，你应当参考 docs/lexer/README.md 和 docs/parser/README.md.

现在我将开始实现语义检查部分。我们将根据以下提示进行开展：

第一步：首先是符号收集。你需要检查 BlockExpression 下的所有 item 和所有 stmt（这是为了遍历到每一个 BlockExpression），将以下几项内容的信息记录在 scope 节点中（这一步先假定函数、const item 用到的类型都存在）：

const item 及其类型（先假定类型和值匹配）
struct 以及它所有的字段+类型
enum 及其 variants
函数/关联函数的参数 pattern 类型/顺序、返回类型
trait 及其所有关联 const 和关联函数

同时建立 Scope 节点之间的连接，为后面的 name resolution 提供方便。注意任何 Expr 中都有可能出现 BlockExpression，所以需要完整遍历 AST。

根据以上的描述，symbol 相关设施已经完成。

接下来，你需要完善 semantic/scope.hpp 中 Scope 类的相关接口和函数实现。