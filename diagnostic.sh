#!/usr/bin/bash

set -euo pipefail

# usage:
# $ ./diagnostic.sh <file_name>
# Example:
# $ ./diagnostic.sh TMCompiler/src/unittesting/unittesting.hpp

# .clang-tidy file generated by
# $ clang-tidy --dump-config --checks=*,-llvm-header-guard,-fuchsia-trailing-return,-fuchsia-statically-constructed-objects,-fuchsia-default-arguments-calls,-fuchsia-overloaded-operator,-cert-err58-cpp,-cppcoreguidelines-macro-usage,-google-runtime-references -- --std=c++17 > .clang-tidy

# Note: in zsh, use "noglob clang-tidy" instead of "clang-tidy": see https://stackoverflow.com/questions/62266965/clang-tidy-resolving-no-match-error-when-adding-arguments

# reference: https://www.labri.fr/perso/fleury/posts/programming/using-clang-tidy-and-clang-format.html

# reason for not including specific checks:
# llvm-header-guard: Turns header guards into long names that show entire file path on my machine
# fuchsia-trailing-return: warns about trailing return types, when I want to use it
# fuchsia-statically-constructed-objects: forbids global objects, even though I do have global variables
# fuchsia-default-arguments-calls: forbids calling functions with default arguments
# fuchsia-overloaded-operator: forbids overloading operators like <<
# cert-err58-cpp: warns about global variables with static storage that may throw an exception, even though I do use global variables
# cppcoreguidelines-macro-usage: forbids function-like macros, but I need it for logging and unittests
# google-runtime-references: I prefer to pass by reference to modify in a function, as opposed to passing by pointer (see https://stackoverflow.com/questions/2139224/how-should-i-pass-objects-to-functions)

clang-tidy "$1" -config='' -header-filter=.* --quiet -- -std=c++17 -I . -isystem build/_deps/tomlplusplus-src/include -isystem build/_deps/catch2-src/src -isystem build/_deps/catch2-build/generated-includes/
