本项目是一个 Rust 子集的编译器前端实现，目前已经实现 lexer 和 parser，可以从代码生成一棵 AST 树。具体的，你应当参考 docs/lexer/README.md 和 docs/parser/README.md.

现在的任务就是重新实现 parser_test.py，直接从 sema1_result.txt 中读取标准结果，然后和我的运行结果作比较，并把比较的结果输出到 sema1_test_result.md 中。

parser_test.py 应当能做到每次启动时，编译一遍 parser_test.cpp. (用 cmake，cd build && cmake ..，cd build && make parser_test)

parser_test 的用法形如：./build/parser_test array1. 你也可以直接查看 test/parser_test.cpp 了解其用法。

若结果为 -1，parser_test 会抛出错误，否则就会正常终止程序。以此为根据来获得我的运行结果。