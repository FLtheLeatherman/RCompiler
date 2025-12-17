最后，我们将在 parsePrattExpression 中集成上述内容。

首先，所有在 parsePrattPrefix 中遇到的内容我们都可能遇到，除此之外还有二元的如 assign, compound assign, binary expressions, type cast expression 等。所有这些 expression 类型都应当被正确处理，你需要查看 parser.hpp 中支持的 parse 函数们，查看哪些 expression 类型是可以被处理的；注意分辨清楚它们的逻辑关系，比如有了 parseLoopExpression 就不需要有 parseInfiniteLoopExpression. 自行理清应当如何处理各种 expression 类型的顺序，再开始实现。

然后，有一些需要 lhs 的 expression 类型，比如 call expression 和 method call expression，它还需要我们提供前面的 expression，我们应当把这些需要的类型写实现一个 parseSomethingFromInfix 函数，传入需要的 lhs，后续内容和普通的 parseSomething 一致。

开始任务前，请阅读 docs/rule.md.