// given source code of program, generate tokens / words, without comments

#ifndef TOKENIZER_HPP
#define TOKENIZER_HPP

#include <cstddef>	// std::size_t
#include <string>	// std::string

struct Token {
	std::string type;
	std::string value;
	std::size_t program_line_number;
	std::size_t start_position_of_token_in_program_line;
};

#endif
