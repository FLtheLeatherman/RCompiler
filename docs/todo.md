你的 parseStatements 逻辑似乎有一些问题。ExpressionWithoutBlock 只会出现至多一个，所以你应当在 while 循环结束之后再尝试 parseExpressionWithoutBlock. 修改它。

开始任务前，请阅读 docs/rule.md.