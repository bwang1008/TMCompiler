// Tokens represent words / lexeme: for instance "x+=5"
// split into the tokens "x", "+=", "5"

#ifndef TOKENIZER_HPP
#define TOKENIZER_HPP

#include <cstddef>	// std::size_t
#include <string>	// std::string

struct Token {
	std::string type;	// ex: identifier, keyword
	std::string value;  // ex: "my_name", "int", "return", "78"
	std::size_t program_line_number;
	std::size_t start_position_of_token_in_program_line;
};

#endif
