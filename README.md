# pyser
A Python 3.10 parser written in pure C++17 without any dependencies.

**Python grammar can be not only for Python.** For example, if you disable class and nested function, you get a C-like language. When you learn compiler, you will find that many toy languages can be reduced to a subset of Python grammar, such as LLVM's Kaleidoscope tutorial. We can use Python grammar for many purposes without sticking to the semantics of Python.

Pyser aims to be a general language front-end framework by using Python grammar. You can target Python AST to different backends. It can be used in any C++17 project. Pyser has very clean code and interface, easy for beginners.

This project demostrate how to
- handle whitespace sensitive grammar 
- combine recursive decent parser with pratt parsing
- represent AST in modern C++
- use visitor pattern
- ...
