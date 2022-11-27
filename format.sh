#!/usr/bin/bash

# usage:
# $ ./format.sh <file_name>
# Example:
# $ ./format.sh TMCompiler/src/unittesting/unittesting.hpp

# .clang-format file generated through online sources, then pasted to .clang-format
clang-format -i --style=file $1
