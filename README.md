# clox [WIP]
Byte code based implementation of clox programming language. Only scanning, parsing, compiling to bytecode, and executing of arithmetic expressions is supported so far.

## How to build/test
```bash
./build.sh && ./test.sh
```
## Run REPL loop (compiler/VM are now on 'verbose' mode by default)
```bash
./run.sh
> !(5 - 4 > 3 * 2 == !nil)
== code ==

0000    1 OP_CONSTANT      0000 '5'
0002    | OP_CONSTANT      0001 '4'
0004    | OP_SUBTRACT
0005    | OP_CONSTANT      0002 '3'
0007    | OP_CONSTANT      0003 '2'
0009    | OP_MULTIPLY
0010    | OP_GREATER
0011    | OP_NIL
0012    | OP_NOT
0013    | OP_EQUAL
0014    | OP_NOT
0015    | OP_RETURN

== run ==


0000    1 OP_CONSTANT      0000 '5'
          [ '5' ]
0002    | OP_CONSTANT      0001 '4'
          [ '5' ][ '4' ]
0004    | OP_SUBTRACT
          [ '1' ]
0005    | OP_CONSTANT      0002 '3'
          [ '1' ][ '3' ]
0007    | OP_CONSTANT      0003 '2'
          [ '1' ][ '3' ][ '2' ]
0009    | OP_MULTIPLY
          [ '1' ][ '6' ]
0010    | OP_GREATER
```
