// given source code of program, generate tokens / words, without comments

#ifndef TOKENIZER_HPP
#define TOKENIZER_HPP

#include <string>

struct Token {
	std::string type;
	std::string value;
	int program_line_number;
	int start_position_of_token_in_program_line;
};

#endif
