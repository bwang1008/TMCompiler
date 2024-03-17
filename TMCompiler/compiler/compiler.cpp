/**
 * Wrapper program that compiles source code.
 *
 * TODO(bwang): Figure out where to specify backend. Perhaps not in the
 * constructor: we can specify a compiler first based only on grammars, then
 * later specify which source program, and which backend too.
 */

#include "compiler.hpp"

#include <fstream>	  // std::ifstream
#include <iostream>	  // std::endl
#include <set>		  // std::set
#include <stdexcept>  // std::invalid_argument
#include <string>	  // std::string, std::getline
#include <vector>	  // std::vector

#include <TMCompiler/compiler/lexer/lexer.hpp>					  // Lexer
#include <TMCompiler/compiler/models/language_specification.hpp>  // LanguageSpecification
#include <TMCompiler/compiler/models/rule.hpp>					  // Rule
#include <TMCompiler/compiler/models/token.hpp>					  // Token
#include <TMCompiler/compiler/parser/earley_parser.hpp>	 // SubParse
#include <TMCompiler/utils/logger/logger.hpp>			 // LOG

/**
 * Constructor for Compiler class.
 *
 * Initializes the LanguageSpecification struct containing
 * token regexes and BNF grammar, the data to parse source code.
 *
 * Then it marks certain symbols in the syntactical BNF as terminal: some
 * non-terminals actually appear in the lexical BNF instead, like "identifier"
 * and "constants".
 *
 * @param language_spec_file_name: path of TOML file specifying
 * programming language syntax
 */
Compiler::Compiler(const std::string& language_spec_file_name) {
	spec = LanguageSpecification::read_language_specification_toml(
		language_spec_file_name);
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
	LOG("INFO") << "Tokenizing input" << std::endl;

	Lexer lexer{spec.token_regexes};
	lexer.set_text(program_text);
	std::vector<Token> words;

	while(lexer.has_next_token()) {
		const Token token = lexer.get_next_token();

		if(spec.token_regexes_ignore.find(token.type) ==
		   spec.token_regexes_ignore.end()) {
			words.push_back(token);
		}
	}

	LOG("DEBUG") << "Tokens = " << std::endl;
	for(const Token& t : words) {
		LOG("DEBUG") << "\t"
					 << "token(" << t.type << ", " << t.value << ")"
					 << std::endl;
	}

	LOG("INFO") << "Generating grammar" << std::endl;
	Grammar grammar{spec.syntax_rules, spec.syntax_main};

	// modify rules for special tokens
	// for instance, <identifier> becomes terminal
	// instead of non-terminal
	//
	// TODO(bwang): make this automatic, by comparing leaves in syntax with
	// lexical
	const std::set<std::string> special_tokens{"keyword",
											   "identifier",
											   "integer-constant",
											   "boolean-constant",
											   "punctuator"};
	grammar.mark_special_symbols_as_terminal(special_tokens);

	// obtain parse tree of source program from tokens
	LOG("INFO") << "Parsing tokens into parse tree" << std::endl;
	std::vector<SubParse> parse_tree_syntactical = grammar.parse(words);

	return parse_tree_syntactical;
}
