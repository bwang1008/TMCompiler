#!/usr/bin/bash
set -euxo pipefail

# g++ -I. TMCompiler/utils/text_editor/text_editor.cpp TMCompiler/tests/test_text_editor.cpp && ./a.out
g++ -std=c++14 -I. TMCompiler/compiler/models/bnf_parser.cpp TMCompiler/tests/test_bnf_parser.cpp && ./a.out
