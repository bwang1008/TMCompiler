#include "compiler.hpp"

#include <algorithm>  // std::max
#include <exception>  // std::exception
#include <fstream>	  // std::ifstream
#include <set>		  // std::set
#include <stdexcept>  // std::invalid_argument
#include <string>	  // std::string, std::getline
#include <vector>	  // std::vector

#include <TMCompiler/compiler/front_end/earley_parser.hpp>	// SubParse
#include <TMCompiler/compiler/models/grammar.hpp>			// Grammar
#include <TMCompiler/compiler/models/rule.hpp>		// Rule
#include <TMCompiler/compiler/models/token.hpp>				// Token
#include <TMCompiler/utils/logger/logger.hpp>

Compiler::Compiler(std::ifstream& lexical_bnf, std::ifstream& syntax_bnf)
	: lexical_grammar(lexical_bnf, "tokens"), syntactical_grammar(syntax_bnf, "???") {
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

	// TODO(bwang1008): should compile_text be responsible for writing out to files?
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

	// 3. Back-end: convert parse_tree into architecture-specific representation
	// / code-generation
	LOG("INFO") << "Pass to backend "
				<< "Multitape Turing Machine" << std::endl;
	// TODO(bwang1008):  implement back-ends
}

auto Compiler::generate_parse_tree(const std::string& program_text) const
	-> std::vector<SubParse> {
	// split program_text into individual letters
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
	std::vector<Token> words = tokenize(parse_tree_lexical);

	// obtain parse tree
	LOG("INFO") << "Parsing tokens into parse tree" << std::endl;
	std::vector<SubParse> parse_tree_syntactical =
		syntactical_grammar.parse(words);

	return parse_tree_syntactical;
}

/**
 * Given a parse tree built from Earley parsing of letters from program.
 */
auto Compiler::tokenize(const std::vector<SubParse>& parse_tree) const
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
		for(SubParse subparse : parse_tree) {
			if(subparse.start == index) {
				const std::string production =
					grammar_rules[subparse.rule].production.value;

				if(token_delimiter.find(production) != token_delimiter.end()) {
					// found a whitespace; do not need to put token-delimiter in
					// list of tokens
					index = subparse.end;
					break;
				}

				if(tokens.find(production) != tokens.end()) {
					result.push_back(Token{production, "?", 0, 0});
					index = subparse.end;
					break;
				}
			}
		}

		throw std::invalid_argument("Unrecognized token at index " +
							 std::to_string(index));
	}

	return result;
}
