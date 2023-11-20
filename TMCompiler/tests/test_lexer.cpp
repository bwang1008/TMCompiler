#include <string>  // std::string
#include <vector>  // std::vector

#include <TMCompiler/compiler/lexer/lexer.hpp>	 // Lexer
#include <TMCompiler/compiler/models/token.hpp>	 // Token
#include <catch2/catch_amalgamated.hpp>

TEST_CASE("test_lexer_comment_0") {
	Lexer lexer("TMCompiler/config/regex_lexical_grammar.bnf");

	std::string program_text = "/**/";

	lexer.set_text(program_text);
	std::vector<Token> tokens;

	while(lexer.has_next_token()) {
		tokens.push_back(lexer.get_next_token());
	}

	REQUIRE(tokens.size() == 1);
	REQUIRE(tokens[0].type == "block-comment");
}

TEST_CASE("test_lexer_comment_1") {
	Lexer lexer("TMCompiler/config/regex_lexical_grammar.bnf");

	std::string program_text = "/* */";
	lexer.set_text(program_text);
	std::vector<Token> tokens;

	while(lexer.has_next_token()) {
		tokens.push_back(lexer.get_next_token());
	}

	REQUIRE(tokens.size() == 1);
	REQUIRE(tokens[0].type == "block-comment");
}

TEST_CASE("test_lexer_comment_2") {
	Lexer lexer("TMCompiler/config/regex_lexical_grammar.bnf");

	std::string program_text = "/***/";
	lexer.set_text(program_text);
	std::vector<Token> tokens;

	while(lexer.has_next_token()) {
		tokens.push_back(lexer.get_next_token());
	}

	REQUIRE(tokens.size() == 1);
	REQUIRE(tokens[0].type == "block-comment");
}

TEST_CASE("test_lexer_comment_3") {
	Lexer lexer("TMCompiler/config/regex_lexical_grammar.bnf");

	std::string program_text = "/**********/";
	lexer.set_text(program_text);
	std::vector<Token> tokens;

	while(lexer.has_next_token()) {
		tokens.push_back(lexer.get_next_token());
	}

	REQUIRE(tokens.size() == 1);
	REQUIRE(tokens[0].type == "block-comment");
}

TEST_CASE("test_lexer_comment_4") {
	Lexer lexer("tmcompiler/config/regex_lexical_grammar.bnf");

	std::string program_text = "/*/*/";
	lexer.set_text(program_text);
	std::vector<Token> tokens;

	while(lexer.has_next_token()) {
		tokens.push_back(lexer.get_next_token());
	}

	REQUIRE(tokens.size() == 1);
	REQUIRE(tokens[0].type == "block-comment");
}

TEST_CASE("test_lexer_comment_5") {
	Lexer lexer("TMCompiler/config/regex_lexical_grammar.bnf");

	std::string program_text = "/*/////////////*/";
	lexer.set_text(program_text);
	std::vector<Token> tokens;

	while(lexer.has_next_token()) {
		tokens.push_back(lexer.get_next_token());
	}

	REQUIRE(tokens.size() == 1);
	REQUIRE(tokens[0].type == "block-comment");
}
