#!/usr/bin/bash

# usage:
# $ ./diagnostic.sh <file_name>
# Example:
# $ ./diagnostic.sh TMCompiler/src/unittesting/unittesting.hpp

# .clang-tidy file generated by 
# $ clang-tidy TMCompiler/src/text_editor.[ch]pp -dump-config -checks=*,-llvm-header-guard,-modernize-use-trailing-return-type -- -std=c++14 > .clang-tidy

# reference: https://www.labri.fr/perso/fleury/posts/programming/using-clang-tidy-and-clang-format.html

clang-tidy "$1" -config='' -header-filter=.* --quiet -- -std=c++14 -I.
