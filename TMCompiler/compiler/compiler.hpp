#ifndef COMPILER_HPP
#define COMPILER_HPP

#include <fstream>	// std::ifstream
#include <string>	// std::string
#include <vector>	// std::vector

#include <TMCompiler/compiler/front_end/earley_parser.hpp>	// SubParse
#include <TMCompiler/compiler/models/grammar.hpp>			// Grammar
#include <TMCompiler/compiler/models/token.hpp>				// Token

class Compiler {
public:
	Compiler(std::ifstream& lexical_bnf, std::ifstream& syntax_bnf);
	auto compile(const std::string& file_name) const -> void;
	auto compile_text(const std::string& program_text) const -> void;

	auto generate_parse_tree(const std::string& program_text) const
		-> std::vector<SubParse>;

private:
	Grammar lexical_grammar;
	Grammar syntactical_grammar;

	auto tokenize(const std::vector<SubParse>& parse_tree) const
		-> std::vector<Token>;
};

#endif
