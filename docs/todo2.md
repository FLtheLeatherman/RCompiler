Thoughts:

1. symbol 不需要重写；只需要加入 array symbol，这需要 const eval 和进一步的（其他模块）改动。

2. variables 是另一套体系，单独维护 变量 这一类东西。比如 func params，应当为变量，那就不急着加入。

3. createConstValueFromExpression 在类型检查中还应当被使用（Array Expression）。

4. 检查 break 和 continue 是否都在 loop 中。

5. 控制流检查：

检测每个 block expression 中，最后一个 expression 的情况。如果是 break/continue/return/全是!的if，那就已经是 ! 了；否则可能是其他的表达式类型，比如 block expr/group expr，要检查是不是 !；loop expr 的话，要检查有没有全 return.

6. 设计一套良好的类型系统。

你可能要处理的：

LetStmt, Function, TypeCastExpr, AssignExpr