# Models
This directory contains core data structures and related algorithms needed to implement a compiler front-end. The main header to expose is `grammar.hpp`.

Most header files (`*.hpp` file) is accompanied by an implementation file (`*.cpp` file) of the same name.

## Files
- `lexer`: data structure that parses text and assigns a label to substrings based off of regex patterns
- `grammar`: data structure that provides a `parse` wrapper function, to parse a program by a specific grammar
- `earley_parser`: functionality to parse input tokens by a specific grammar
- `token`: data structure to read in an input program and generate tokens, to be parsed later

## How it Works
`Compiler` reads in token regexes and the syntactical context-free-grammar in a TOML configuration file, which is parsed by `LanguageSpecification`. `Compiler` then uses the `Lexer` to convert an input program's characters into tokens / words. Then the `Grammar` uses Earley Parsing to convert tokens into a parse tree.
