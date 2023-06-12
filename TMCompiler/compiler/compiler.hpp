#ifndef COMPILER_HPP
#define COMPILER_HPP

#include <fstream>	// std::ifstream
#include <string>	// std::string
#include <vector>	// std::vector

#include <TMCompiler/compiler/models/grammar.hpp>	// Grammar
#include <TMCompiler/compiler/models/token.hpp>		// Token
#include <TMCompiler/compiler/front_end/earley_parser.hpp>		// EarleyRule, SubParse

class Compiler {
public:
	Compiler(std::ifstream& lexical_bnf, std::ifstream& syntax_bnf);
	auto compile(std::ifstream& program_file) const -> void;
	auto compile(const std::string program_text) const -> void;

private:
	Grammar lexical_grammar;
	Grammar syntactical_grammar;

	auto tokenize(const std::vector<SubParse>& parse_tree, const std::vector<EarleyRule> &grammar_rules) -> std::vector<Token>;
};

#endif
