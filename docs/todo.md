接下来实现 MethodCallExpression：

MethodCallExpression → Expression `.` PathExprSegment `(` CallParams? `)`

你需要新建从 ASTNode 继承来的 PathExprSegment 类，但是暂时不需要具体实现。

其次是 FieldExpression：

FieldExpression → Expression . IDENTIFIER

以及 PathExpression：

PathExpression →
      PathInExpression

先在 astnode.hpp 中实现尚未实现的类型，注意区分是从 ASTNode 继承还是从 Expression 继承，然后到 parser 模块进行声明和实现。

开始任务前，请阅读 docs/rule.md.