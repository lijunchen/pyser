# pyser
A Python 3.10 parser written in C++20 without any dependencies.

**Python grammar can be not only for Python.** For example, if you disable class and nested function, you get a C-like language. When you learn compiler, you will find that many toy languages can be reduced to a subset of Python grammar, such as LLVM's Kaleidoscope tutorial. We can use Python grammar for many purposes without sticking to the semantics of Python.

Pyser aims to be a general language front-end framework by using Python grammar. You can target Python AST to different backends. It can be used in any C++20 project. Pyser has very clean code and interface, easy for beginners.

This project demostrate how to
- handle whitespace sensitive grammar 
- combine recursive decent parser with pratt parsing
- represent and pretty print AST in C++
- use visitor pattern
- ...

Test idea: the output of `source -> pyser -> ast in text -> eval -> ast -> astpretty.pprint` shouble be equal to `source -> ast.parse -> ast -> astpretty.pprint`


TODO:
- Node generator
- Parser generator
- Port LLVM tutorial
- Python VM

Note, even though I claim it is zero dependencies, I decided to use [re2c](https://github.com/skvadrik/re2c) to ramp up tokenizer.

References and Resources:
- [AST definition](https://github.com/python/cpython/blob/main/Parser/Python.asdl)
- [Token definition](https://github.com/python/cpython/blob/main/Grammar/Tokens)
- [Full Grammar specification](https://docs.python.org/3.10/reference/grammar.html)
- [Simple but Powerful Pratt Parsing](https://matklad.github.io/2020/04/13/simple-but-powerful-pratt-parsing.html)
