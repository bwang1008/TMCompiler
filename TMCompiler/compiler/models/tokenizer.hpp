// given source code of program, generate tokens / words, without comments

#ifndef TOKENIZER_HPP
#define TOKENIZER_HPP

#include <fstream>
#include <string>
#include <vector>

#include <TMCompiler/compiler/models/bnf_parser.hpp>

class Token {
public:
	Token(std::string type, std::string value);
	auto get_type() const -> std::string;
	auto get_value() const -> std::string;

private:
	std::string type;
	std::string value;
	int program_line_number;
	int start_position_of_token_in_program_line;
};

class Tokenizer {
public:
	explicit Tokenizer(std::ifstream bnf_file);
	auto tokenize(std::ifstream program_file) -> std::vector<Token>;
	auto tokenize(const std::string& program_text) -> std::vector<Token>;

private:
	Rules lexical_rules;

	// auto
};

#endif
