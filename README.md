# McBOOLE
## What is it?
A sophisticated exact minimizer for multiple output boolean equations, written in 1986 as the master thesis of Michel R. Dagenais. This repository contains the original code written in K&R C and also a cleaned up version which compiles with ANSI C compliant compilers

## How to build it?
Run `make` in the directory `build`

## How to use it?
We take a look at the example found in `examples/ex1.mcb` which contains among other things the definition of a truth-table:
```
/* ABCD FW */
0001 d1
0101 1d
0111 1d
1101 11
1111 d0
0011 11
1011 01
0000 0d
```
The first 4 columns are the inputs, the last two columns are the outputs. You can also use 'd' to indicate a don't care conditions in the output. If you pass this file as the argument to McBOOLE, you get the following output:
```
x101 11
x011 01
0xx1 11
```
The 'x' in the input show don't care conditions in the input. This can be translated to the following, minimized boolean equation:
`F=BC'D+A'D` and `W=BC'D+B'CD+A'D`

