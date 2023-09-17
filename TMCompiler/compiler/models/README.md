# Models
This directory contains core data structures and related algorithms needed to implement a compiler front-end. The main header to expose is `grammar.hpp`.

Most header files (`*.hpp` file) is accompanied by an implementation file (`*.cpp` file) of the same name.

## Files
- `grammar`: data structure that provides a `parse` wrapper function, to parse a program by a specific grammar
- `earley_parser`: functionality to parse input tokens by a specific grammar
- `token`: data structure to read in an input program and generate tokens, to be parsed later

## How it Works
`Grammar` reads in lexical and syntatical context-free-grammars in the form of BNF files. This is parsed by `BnfParser`. `Grammar` then uses the `Tokenizer` to convert an input program's characters into tokens / words. Then the `Grammar` uses Earley Parsing to convert tokens into a parse tree.
