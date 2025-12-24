Questions:

1. 如何将 func_params 塞入 function 的 scope 中？是不是要开创一个叫 var_symbols 的东西并进行维护，还是等到第四步再维护？

2. struct_checker 中的 type existence check 未完成，比如 struct fields 中的东西。

3. type existence check 的底层逻辑：checkTypeExists 并不是一个完备的东西：特别是面对数组类型。它有可能是一个高维数组。这里的处理仍然不足。
这里其实涉及到一个更深的问题，也就是对于 array symbols 的处理。记得要在 constant evaluation 之内处理掉它。

Thoughts:

1. symbol 不需要重写；只需要加入 array symbol，这需要 const eval 和进一步的（其他模块）改动。

2. variables 是另一套体系，单独维护 变量 这一类东西。比如 func params，应当为变量，那就不急着加入。

（done）3. struct checker 还需要修改。一个是 scope 没有正确访问，一个是 struct fields 没有进行检测。

4. createConstValueFromExpression 在类型检查中还应当被使用（Array Expression）。