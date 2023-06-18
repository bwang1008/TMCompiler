#include "compiler.hpp"

#include <algorithm>	// std::max
#include <fstream>	// std::ifstream
#include <string>	// std::string, std::getline
#include <set>		// std::set
#include <exception>	// std::exception
#include <stdexcept>	// std::invalid_argument
#include <vector>		// std::vector

#include <TMCompiler/compiler/models/grammar.hpp>	// Grammar
#include <TMCompiler/compiler/models/token.hpp>		// Token
#include <TMCompiler/compiler/front_end/earley_parser.hpp>		// EarleyRule, SubParse
#include <TMCompiler/util/logger/logger.hpp>

Compiler::Compiler(std::ifstream& lexical_bnf, std::ifstream& syntax_bnf) : lexical_grammar(lexical_bnf), syntactical_grammar(syntax_bnf) {

}

auto Compiler::compile(const std::string file_name) const -> void {
	LOG("INFO", std::string("Compiling ") + file_name);

	std::ifstream program_file{file_name};
	if(!program_file.is_open()) {
		LOG("ERROR", std::string("Unable to open file ") + file_name);
		throw std::invalid_argument(std::string("Unable to open file ") + file_name);
	}

	std::string program_text;
	std::string line;
	while(std::getline(program_file, line)) {
		program_file.append(line);
		program_file.append("\n");
	}

	program_file.close();

	compile_text(program_text);
}

auto Compiler::compile_text(const std::string program_text) const -> void {
	// split program_text into individual letters
	std::vector<Token> letters;
	for(std::size_t index = 0, line_number = 0, col_number = 0; index < program_text.size(); ++index, ++col_number) {
		const char letter = program_text[index];
		if(letter == '\n') {
			++line_number;
			col_number = 0;
		}

		letters.push_back(Token{"letter", std::string(1, c), line_number, col_number});
	}

	// retrieve words from letters
	std::vector<SubParse> parse_tree_lexical = lexical_grammar.parse(letters);
	std::vector<Tokens> words = tokenize(parse_tree_lexical);

	// obtain parse tree
	std::vector<SubParse> parse_tree_syntactical = syntactical_grammar.parse(words);

	// pass to back-end handler

}

/**
 * Given a parse tree built from Earley parsing of letters from program.
 */
auto Compiler::tokenize(const std::vector<SubParse>& parse_tree, const std::vector<EarleyRule> &grammar_rules) -> std::vector<Token> {

	const std::set<std::string> tokens {"keyword", "identifier", "constant", "punctuator"};
	const std::set<std::string> token_delimiter {"whitespace"};


	std::size_t end = 0;
	for(SubParse subparse : parse_tree) {
		end = std::max(subparse.end, end);
	}

	std::vector<Token> result;
	std::size_t index = 0;

	while(index < end) {

		for(SubParse subparse: parse_tree) {
			if(subparse.begin == index) {

				const std::string production = grammar_rules[subparse.rule].production.value;

				if(token_delimiter.find(production) != token_delimiter.end()) {
					// found a whitespace; do not need to put token-delimiter in list of tokens
					index = subparse.end;
					break;
				}

				if(token.find(production) != token.end()) {
					result.push_back(Token{production, "?", 0, 0});
					index = subparse.end;
					break;
				}
			}
		}

		throw std::exception("bad");
	}

	return result;
}
