/**
 * Wrapper program that compiles source code.
 *
 * TODO(bwang): Figure out where to specify backend. Perhaps not in the
 * constructor: we can specify a compiler first based only on grammars, then
 * later specify which source program, and which backend too.
 */

#include "compiler.hpp"

#include <algorithm>  // std::max
#include <cstddef>	  // std::size_t
#include <fstream>	  // std::ifstream
#include <iostream>	  // std::endl
#include <set>		  // std::set
#include <stdexcept>  // std::invalid_argument
#include <string>	  // std::string, std::getline
#include <vector>	  // std::vector

#include <TMCompiler/compiler/models/rule.hpp>			 // Rule
#include <TMCompiler/compiler/models/token.hpp>			 // Token
#include <TMCompiler/compiler/parser/earley_parser.hpp>	 // SubParse
#include <TMCompiler/utils/logger/logger.hpp>			 // LOG

/**
 * Constructor for Compiler class.
 *
 * This initializes both the lexical and syntactical Grammar instance variables.
 * Then it marks certain symbols in the syntactical BNF as terminal: some
 * non-terminals actually appear in the lexical BNF instead, like "identifier"
 * and "constants".
 *
 * @param lexical_bnf: input stream to BNF file specifying lexical grammar, i.e.
 * how tokens are formed from letters
 * @param syntax_bnf: input stream to BNF file specifying syntactical grammar,
 * i.e. how each language construct is made up of smaller constructs
 */
Compiler::Compiler(std::ifstream& lexical_bnf, std::ifstream& syntax_bnf)
	: lexical_grammar(lexical_bnf, "tokens"),
	  syntactical_grammar(syntax_bnf, "compilation-unit") {
	// detect default start from both BNFs

	// modify rules for special tokens
	// for instance, <identifier> becomes terminal
	// instead of non-terminal
	//
	// TODO(bwang): make this automatic, by comparing leaves in syntax with
	// lexical
	const std::set<std::string> special_tokens{
		"keyword", "identifier", "constant", "punctuator"};
	syntactical_grammar.mark_special_symbols_as_terminal(special_tokens);
}

/**
 * Wrapper program that reads in source code from file_name and compiles the
 * text.
 *
 * @param file_name: name of file containing source code to be compiled
 */
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

/**
 * Program that parses source code string and generates equivalent program
 * in a different backend architecture.
 *
 * @param program_text: source code to be be compiled, with '\n' between lines
 */
auto Compiler::compile_text(const std::string& program_text) const -> void {
	// 1. Front-end: tokenization and parsing of program_text
	std::vector<SubParse> parse_tree = generate_parse_tree(program_text);

	// 2. Middle-end: type-checking, identifiers are declared, functions that
	// are called exist, main exists, no double declaration
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

/**
 * Frontend of compiler: turns source code text into a parse tree, described
 * by the syntactical grammar.
 *
 * @param program_text: source code to be processed, with '\n' between newlines
 */
auto Compiler::generate_parse_tree(const std::string& program_text) const
	-> std::vector<SubParse> {
	// split program_text into individual letters

	LOG("INFO") << "Generating character tokens" << std::endl;

	// TODO(bwang): move this to a function
	// convert each individual letter into a Token type
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

	// retrieve valid subparses from letters
	LOG("INFO") << "Tokenizing input" << std::endl;
	std::vector<SubParse> parse_tree_lexical = lexical_grammar.parse(letters);

	LOG("INFO") << "Turn character subparses into tokens" << std::endl;
	std::vector<Token> words = tokenize(parse_tree_lexical, program_text);

	LOG("DEBUG") << "Tokens = " << std::endl;
	for(const Token& t : words) {
		LOG("DEBUG") << "\t"
					 << "token(" << t.type << ", " << t.value << ")"
					 << std::endl;
	}

	// obtain parse tree of source program from tokens
	LOG("INFO") << "Parsing tokens into parse tree" << std::endl;
	std::vector<SubParse> parse_tree_syntactical =
		syntactical_grammar.parse(words);

	return parse_tree_syntactical;
}

/**
 * Given a parse tree built from Earley parsing of letters from program,
 * generate the tokens associated with the lexical BNF. Whitespace
 * is ignored.
 *
 * TODO(bwang): perhaps put comments in as well to ignore
 *
 * For instance, if the source program was "int x = 123;", the parse_tree
 * would look like [([0, 11) is a subparse of tokens), ([0, 3) is a valid
 * subparse of a keyword), ([0, 1) is a valid letter), ...]. After tokenizing,
 * this should return [Token(keyword, "int"), Token(identifier, "x"),
 * Token(punctuator, "="), * Token(constant, "123"), Token(punctuator, ",")].
 */
auto Compiler::tokenize(const std::vector<SubParse>& parse_tree,
						const std::string& program_text) const
	-> std::vector<Token> {
	// list of tokens to keep: when we see "123" as a constant, we do not need
	// to know that "1", "2", "3" are all non-zero-digits.
	const std::set<std::string> tokens{
		"keyword", "identifier", "constant", "punctuator"};

	// in this programming language, whitespace is ignored
	const std::set<std::string> token_delimiter{"whitespace"};

	const std::vector<Rule> grammar_rules = lexical_grammar.get_rules();

	std::size_t end = 0;
	for(SubParse subparse : parse_tree) {
		end = std::max(subparse.end, end);
	}

	std::vector<Token> result;
	std::size_t index = 0;

	/* Start at index 0. Look at all valid subparses starting at index 0.
	 * Say you found subparse(rule 2, start=0, end=3), and rule 2 is a
	 * "constant". We store this result as a Token, and now we try again at
	 * index 3.
	 */
	while(index < end) {
		const std::size_t old_index = index;

		for(SubParse subparse : parse_tree) {
			if(subparse.start == index) {
				const std::string production =
					grammar_rules[subparse.rule].production.value;

				// ignore whitespace
				if(token_delimiter.find(production) != token_delimiter.end()) {
					index = subparse.end;
					break;
				}

				if(tokens.find(production) != tokens.end()) {
					result.push_back(Token{
						production,
						program_text.substr(subparse.start,
											subparse.end - subparse.start),
						0,
						0});
					index = subparse.end;
					break;
				}
			}
		}

		// no token starts at index
		if(index == old_index) {
			LOG("ERROR") << "Unrecognized token at index " << index
						 << std::endl;
			throw std::invalid_argument("Unrecognized token at index " +
										std::to_string(index));
		}
	}

	return result;
}
