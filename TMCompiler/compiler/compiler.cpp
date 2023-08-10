#include "compiler.hpp"

#include <algorithm>  // std::max
#include <cstddef>	  // std::size_t
#include <fstream>	  // std::ifstream
#include <iostream>	  // std::endl
#include <set>		  // std::set
#include <stdexcept>  // std::invalid_argument
#include <string>	  // std::string, std::getline
#include <vector>	  // std::vector

#include <TMCompiler/compiler/front_end/earley_parser.hpp>	// SubParse
#include <TMCompiler/compiler/models/grammar.hpp>			// Grammar
#include <TMCompiler/compiler/models/rule.hpp>				// Rule
#include <TMCompiler/compiler/models/token.hpp>				// Token
#include <TMCompiler/utils/logger/logger.hpp>

Compiler::Compiler(std::ifstream& lexical_bnf, std::ifstream& syntax_bnf)
	: lexical_grammar(lexical_bnf, "tokens"),
	  syntactical_grammar(syntax_bnf, "compilation-unit") {
	// modify rules for special tokens
	// for instance, <identifier> becomes terminal
	// instead of non-terminal
	const std::set<std::string> special_tokens{
		"keyword", "identifier", "constant", "punctuator"};
	syntactical_grammar.mark_special_symbols_as_terminal(special_tokens);
}

auto Compiler::compile(const std::string& file_name) const -> void {
	LOG("INFO") << "Compiling " << file_name << std::endl;

	std::ifstream program_file{file_name};
	if(!program_file.is_open()) {
		LOG("ERROR") << "Unable to open file " << file_name << std::endl;
		throw std::invalid_argument(std::string("Unable to open file ") +
									file_name);
	}

	std::string program_text;
	std::string line;
	while(std::getline(program_file, line)) {
		program_text.append(line);
		program_text.append("\n");
	}

	program_file.close();

	// TODO(bwang1008): should compile_text be responsible for writing out to
	// files?
	compile_text(program_text);

	LOG("INFO") << "Compilation finished!" << std::endl;
}

auto Compiler::compile_text(const std::string& program_text) const -> void {
	// 1. Front-end: tokenization and parsing of program_text
	std::vector<SubParse> parse_tree = generate_parse_tree(program_text);

	// 2. Middle-end: type-checking, identifiers are declared, functions that
	// are called exist, main exists
	LOG("INFO") << "Performing standard checks" << std::endl;
	// TODO(bwang1008): implement middle-end
	//
	// (semantic analysis)
	// make sure identifiers are declared
	// type checking
	// no "break;" on its own line outside of loop

	// 3. Back-end: convert parse_tree into architecture-specific representation
	// / code-generation
	LOG("INFO") << "Pass to backend "
				<< "Multitape Turing Machine" << std::endl;
	// TODO(bwang1008):  implement back-ends
}

auto Compiler::generate_parse_tree(const std::string& program_text) const
	-> std::vector<SubParse> {
	// split program_text into individual letters

	LOG("INFO") << "Generate char tokens" << std::endl;

	std::vector<Token> letters;
	for(std::size_t index = 0, line_number = 0, col_number = 0;
		index < program_text.size();
		++index, ++col_number) {
		const char letter = program_text[index];
		if(letter == '\n') {
			++line_number;
			col_number = 0;
		}

		letters.push_back(
			Token{"letter", std::string(1, letter), line_number, col_number});
	}

	// retrieve words from letters
	LOG("INFO") << "Tokenizing input" << std::endl;
	std::vector<SubParse> parse_tree_lexical = lexical_grammar.parse(letters);

	for(SubParse sp : parse_tree_lexical) {
		LOG("DEBUG") << "{" << sp.rule << ", " << sp.start << ", " << sp.end << ", " << sp.parent << "}" << std::endl;
	}

	LOG("INFO") << "Turn character subparses into tokens" << std::endl;
	std::vector<Token> words = tokenize(parse_tree_lexical, program_text);

	// obtain parse tree
	LOG("INFO") << "Parsing tokens into parse tree" << std::endl;
	std::vector<SubParse> parse_tree_syntactical =
		syntactical_grammar.parse(words);

	return parse_tree_syntactical;
}

/**
 * Given a parse tree built from Earley parsing of letters from program.
 */
auto Compiler::tokenize(const std::vector<SubParse>& parse_tree, const std::string& program_text) const
	-> std::vector<Token> {
	const std::set<std::string> tokens{
		"keyword", "identifier", "constant", "punctuator"};
	const std::set<std::string> token_delimiter{"whitespace"};

	const std::vector<Rule> grammar_rules = lexical_grammar.get_rules();

	std::size_t end = 0;
	for(SubParse subparse : parse_tree) {
		end = std::max(subparse.end, end);
	}

	std::vector<Token> result;
	std::size_t index = 0;

	while(index < end) {

		const std::size_t old_index = index;

		LOG("DEBUG") << "index = " << index << std::endl;

		for(SubParse subparse : parse_tree) {
			if(subparse.start == index) {

				LOG("DEBUG") << "SubParse{" << subparse.rule << ", " << subparse.start << ", " << subparse.end << ", " << subparse.parent << "}" << std::endl;
				const std::string production =
					grammar_rules[subparse.rule].production.value;

				LOG("DEBUG") << "production = " << production << std::endl;

				if(token_delimiter.find(production) != token_delimiter.end()) {
					// found a whitespace; do not need to put token-delimiter in
					// list of tokens
					index = subparse.end;

					LOG("DEBUG") << "opt1: index now = " << index << std::endl;
					break;
				}

				if(tokens.find(production) != tokens.end()) {
					result.push_back(Token{production, program_text.substr(subparse.start, subparse.end - subparse.start), 0, 0});
					index = subparse.end;

					LOG("DEBUG") << "opt2: index now = " << index << std::endl;
					break;
				}
			}
		}

		// no token starts at index
		if(index == old_index) {
			LOG("ERROR") << "Unrecognized token at index " << index << std::endl;
			throw std::invalid_argument("Unrecognized token at index " +
										std::to_string(index));
		}
	}

	LOG("INFO") << "Let's see our word tokens!" << std::endl;
	for(Token t : result) {
		LOG("DEBUG") << "Token{" << t.type << ", " << t.value << "}" << std::endl;
	}

	return result;
}
