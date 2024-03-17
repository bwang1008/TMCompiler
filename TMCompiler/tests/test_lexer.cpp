#include <string>  // std::string
#include <vector>  // std::vector

#include <TMCompiler/compiler/lexer/lexer.hpp>					  // Lexer
#include <TMCompiler/compiler/models/language_specification.hpp>  // LanguageSpecification
#include <TMCompiler/compiler/models/token.hpp>					  // Token

#include <catch2/catch_test_macros.hpp>

TEST_CASE("test_lexer_comment_0") {
	const LanguageSpecification spec =
		LanguageSpecification::read_language_specification_toml(
			"TMCompiler/config/language.toml");
	Lexer lexer(spec.token_regexes);

	std::string program_text;
	SECTION("text0") {
		program_text = "/**/";
	}
	SECTION("text1") {
		program_text = "/* */";
	}
	SECTION("text2") {
		program_text = "/***/";
	}
	SECTION("text3") {
		program_text = "/**********/";
	}
	SECTION("text4") {
		program_text = "/*/*/";
	}
	SECTION("text5") {
		program_text = "/*/////////////*/";
	}

	lexer.set_text(program_text);
	std::vector<Token> tokens;

	while(lexer.has_next_token()) {
		tokens.push_back(lexer.get_next_token());
	}

	REQUIRE(tokens.size() == 1);
	REQUIRE(tokens[0].type == "block-comment");
}
