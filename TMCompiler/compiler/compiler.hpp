/**
 * Wrapper program that compiles source code.
 *
 * compile_text internally parses the source code into an internal
 * representation, then generates an equivalent program in the target
 * architecture.
 */

#ifndef COMPILER_HPP
#define COMPILER_HPP

#include <string>  // std::string
#include <vector>  // std::vector

#include <TMCompiler/compiler/models/grammar.hpp>		 // Grammar
#include <TMCompiler/compiler/models/token.hpp>			 // Token
#include <TMCompiler/compiler/parser/earley_parser.hpp>	 // SubParse

class Compiler {
public:
	/**
	 * Constructor for Compiler class.
	 *
	 * @param language_spec_file_name: path of TOML file specifying
	 * token regexes and BNF grammar.
	 * Ex: "TMCompiler/config/language.toml"
	 */
	Compiler(const std::string& language_spec_file_name);

	/**
	 * Wrapper program that reads in source code from file_name and compiles the
	 * text.
	 *
	 * @param file_name: name of file containing source code to be compiled
	 */
	auto compile(const std::string& file_name) const -> void;

	/**
	 * Program that parses source code string and generates equivalent program
	 * in a different backend architecture.
	 *
	 * @param program_text: source code to be be compiled, with '\n' between
	 * lines
	 */
	auto compile_text(const std::string& program_text) const -> void;

private:
	// programming language of source code can be described using 2
	// specifications: list of regexes for how to parse tokens / words from
	// letters, and a grammar for how to parse tokens into programming-language
	// constructs
	std::string lexical_file;
	Grammar syntactical_grammar;

	// convert lexical parse tree into list of tokens
	[[nodiscard]] auto tokenize(const std::vector<SubParse>& parse_tree,
								const std::string& program_text) const
		-> std::vector<Token>;
	// frontend of compiler: turn source code text into a parse tree
	[[nodiscard]] auto generate_parse_tree(
		const std::string& program_text) const -> std::vector<SubParse>;
};

#endif
