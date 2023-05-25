#include "tokenizer.hpp"

#include <set>
#include <string>
#include <vector>

// Token::Token(std::string type, std::string value)
	// : type(type),
	  // value(value),
	  // program_line_number(0),
	  // start_position_of_token_in_program_line(0) {
// }

// Token::Token(const std::string value): value(value), program_line_number{0}, start_position_of_token_in_program_line{0} {

// }

/*
std::vector<Token> Tokenizer::tokenize(const std::string &programText) {
	std::vector<Token> tokens;

	const std::set<char> whitespace = {'\t', '\n', ' '};
	const std::set<std::string> operators = {"=", "*=", "/=", "%=", "+=", "-=",
"<<=", ">>=", "&=", "^=", "|=", "!", "&&", "||", "^", "*", "/", "+", "-",  "%"};

	bool isIdentifier = false;
	bool isOperator = false;
	bool isComment = false;

	std::string currentToken = "";
	for(std::size_t i = 0; i < programText.size(); ++i) {
		const char c = programText[i];

		if(c == '(' || c == ')' || true) {

		}

	}

	if(currentToken.size() > 0) {
		// tokens.push_back(currentToken);
	}

	return tokens;
}
*/
