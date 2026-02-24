debug stage:

1. function 套 function

2. global variable 初始化

3. 各种指针问题

4. 内建函数

5. string 相关（一点没写！）

目前进度：（抛去编译不过的点）

sema-1 47/115

sema-2 17/50

IR-1 18/51

----

-1. binary expression（done）

0. let stmt（done）

1. ANDAND 和 OROR 支持短路（done）

2. assignment（done）

2. if expression（done）

3. loop expression（done）

4. struct 相关（done）

5. function parameters 和 return 的处理，call expression（done）

6. impl 相关（done）

7. array 相关（done）

8. 内建函数（初步 done）

9. 全局变量及初始化（初步 done）

10. 指针相关（&, *）（done？）

11. 整理函数声明顺序（main 函数开头进行声明）

12. type cast（done）