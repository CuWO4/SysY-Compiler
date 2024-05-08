# SysY to RISC-V

将 `C` 的子集 `SysY` 编译为 `RISC-V` 的玩具编译器.

参考文档: [北大编译实践在线文档](https://pku-minic.github.io/online-doc/#/).

---

## 简述

实现 `SysY` 向 `Koopa IR` 或 `RISC-V` 的编译.

`SysY` 是 `C` 的子集. 详见 [SysY 标准概述](#SysY概述).

`Koopa IR` 是北大编译实践课模仿 `LLVM IR` 设计的中间表示. 详见 [Koopa IR 标准概述](https://pku-minic.github.io/online-doc/#/misc-app-ref/koopa).

`RISC-V` 是由加州大学伯克利分校设计并推广的第五代 RISC 指令系统体系结构. 详见 [RISC-V手册](https://riscv.org/wp-content/uploads/2017/05/riscv-spec-v2.2.pdf).

**项目支持了部分原版 `SysY` 不支持的 `C` 语法. 此外, 项目未使用北大编译实践课程提供的 `Koopa IR` 框架**.

## 编译

```bash
make
```

或

```bash
make release
```

Windows 平台若因为 `sysy.lex.cpp` 缺少 `unistd.h` 头文件导致编译失败, 可尝试下载 `MinGW` 提供的 `flex` 依赖后, 将 `makefile` 中的 `CXX := clang++` 替换为 `CXX := g++`.

## 运行

```bash
sysyc [MODE] -o [TARGET] [DBG-FLAGS]
```

- `[MODE]` 指定编译模式, 可选值:

    * `-koopa`: 生成 `Koopa IR`;

    * `-riscv`: 生成 `RISC-V`;

    * `-test`: 向控制台打印 `AST`.

- `[TARGET]` 指定目标文件.

- `[DBG-FLAGS]` 指定 `DEBUG` 模式, 可选值(可同时启用):

    * `-dbg-k`: 生成的 `Koopa IR` 代码中会包含类型信息;

    * `-dbg-r`: 生成的 `RISC-V` 代码中会包含 `Koopa IR` 原句.

## 测试

本节内容依赖 `Docker` 镜像 `maxxing/compiler-dev`, 须在相应容器中运行. 

### 自动测试

```bash
test.py [STAGE] [TARGET-LANG]
```

- `STAGE`: 指定测试样例阶段; 当未指定时, 测试所有阶段. 可选值: `lv1`, `lv2`, ... , `lv9`, `perf`.

- `TARGET-LANG`: 指定测试语言; 当未指定时, 测试所有语言. 可选值: `koopa`, `riscv`.

### 手动测试

```bash
test.py hello [MODE] [DBG-FLAGS]
```

以指定的 `MODE` 和 `DBG-FLAGS` 编译 `testcases/hello/hello.c`, 详见 [运行](#运行).


## 示例

| 主题 | `C` | `Koopa IR` | `RISC-V` |
| :--: | :--: | :--: | :--: |
| **斐波那契数列** | <a href="sample/fib.c">fib.c</a> | <a href="sample/fib.koopa">fib.koopa</a> | <a href="sample/fib.S">fib.S</a> |
| **归并排序** | <a href="sample/msort.c">msort.c</a> | <a href="sample/msort.koopa">msort.koopa</a> | <a href="sample/msort.S">msort.S</a> |
| **快速傅里叶变换** | <a href="sample/fft.c">fft.c</a> | <a href="sample/fft.koopa">fft.koopa</a> | <a href="sample/fft.S">fft.S</a> |

## 历史版本

`v1.0` 2024-5-5. 实现无优化的编译.

## 作者

CuWO4, [邮箱](mailto:wutong.tony@outlook.com).

## 环境

- Unbuntu 20.04.6 LTS;

- Ubuntu clang version 13.0.1;

- flex 2.6.4;

- bison (GNU Bison) 3.5.1;

- GNU Make 4.2.1.

## TODO LIST

- [ ] 寄存器分配.

- [ ] 支持更多语法.

    * [ ] 函数声明;

    * [ ] 自增, 自减, 算术赋值, 位运算, 三目运算符;

    * [ ] 指针运算;

    * [ ] 字符串常量;

    * [ ] 结构体;

    * [ ] 预处理命令;

    * [ ] 更多类型;

    * [ ] `do-while` 语句, `switch` 语句, `goto` 语句.

- [ ] 编译优化.

    * [ ] `DCE`;

    * [ ] 常量传播;

    * [ ] 控制流化简;

    * [ ] 窥孔优化;

    * [ ] 强度削弱.

## SysY 概述

**支持的全局语句**: 函数定义, 全局变量定义.

**支持的子句**: 变量定义, 表达式语句, `return`, `if`, `else`, `while`, `for`, `break`, `continue`.

**支持的类型**: `int`, `void`, 数组, 形如`T[]`的指针.

**支持的运算符**: 
    
* **算术**:  `+`, `-`, `*`, `/`, `%`, `+(单目)`, `-(单目)`;

* **逻辑**: `!`, `||`, `&&`;

* **比较**: `==`, `!=`, `<`, `>`, `<=`, `>=`;

* **赋值**: `=`;

* **其它**: `()`[^1], `[]`[^2], `,`.

**运行时库**：无需包含头文件, 自动声明以下八个函数:

```c 
int getint();
int getch();
int getarray(int[]);
void putint(int);
void putch(int);
void putarray(int, int[]);
void starttime();
void stoptime();
```

规范详见 [SysY 运行时库](https://gitlab.eduxiji.net/csc1/nscscc/compiler2021/-/blob/master/SysY%E8%BF%90%E8%A1%8C%E6%97%B6%E5%BA%93.pdf).

---

[^1]: 函数调用.

[^2]: 数组解引用.