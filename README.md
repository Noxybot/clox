# clox [WIP]
Byte code based implementation of clox programming language. Only scanning, parsing, compiling to bytecode, and executing of arithmetic expressions is supported so far.

## How to build/test
```bash
./build.sh && ./test.sh
```
## Run REPL loop (compiler/VM are now on 'verbose' mode by default)
```bash
./run.sh
> 42+42/42*42
== code ==

0000    1 OP_CONSTANT      0000 '42'
0002    | OP_CONSTANT      0001 '42'
0004    | OP_CONSTANT      0002 '42'
0006    | OP_DIVIDE
0007    | OP_CONSTANT      0003 '42'
0009    | OP_MULTIPLY
0010    | OP_ADD
0011    | OP_RETURN

'42'
          [ '42' ]
'42'
          [ '42' ][ '42' ]
'42'
          [ '42' ][ '42' ][ '42' ]
          [ '42' ][ '1' ]
'42'
          [ '42' ][ '1' ][ '42' ]
          [ '42' ][ '42' ]
          [ '84' ]
'84'
```
