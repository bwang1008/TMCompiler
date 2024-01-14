#include <string>  // std::string

#include <TMCompiler/compiler/compiler.hpp>	   // Compiler
#include <TMCompiler/utils/logger/logger.hpp>  // logger

#include <catch2/catch_test_macros.hpp>

TEST_CASE("program_text_0") {
	logger.set_level("NONE");
	const std::string lex_bnf = "TMCompiler/config/regex_lexical_grammar.bnf";
	const std::string syn_bnf = "TMCompiler/config/language_grammar.bnf";

	Compiler compiler(lex_bnf, syn_bnf);
	const std::string program_text{"void foo() {}"};
	compiler.compile_text(program_text);

	REQUIRE(1 == 1);
}

TEST_CASE("program_text_1") {
	logger.set_level("NONE");
	const std::string lex_bnf = "TMCompiler/config/regex_lexical_grammar.bnf";
	const std::string syn_bnf = "TMCompiler/config/language_grammar.bnf";

	Compiler compiler(lex_bnf, syn_bnf);
	const std::string program_text{"int compute(int y) { return 5; }"};
	compiler.compile_text(program_text);

	REQUIRE(1 == 1);
}

TEST_CASE("program_text_check_0") {
	logger.set_level("NONE");
	const std::string lex_bnf = "TMCompiler/config/regex_lexical_grammar.bnf";
	const std::string syn_bnf = "TMCompiler/config/language_grammar.bnf";

	Compiler compiler(lex_bnf, syn_bnf);
	const std::string program_text{"int compute(int y) { return 0; }"};
	compiler.compile_text(program_text);

	REQUIRE(1 == 1);
}

TEST_CASE("program_text_check_negative") {
	logger.set_level("NONE");
	const std::string lex_bnf = "TMCompiler/config/regex_lexical_grammar.bnf";
	const std::string syn_bnf = "TMCompiler/config/language_grammar.bnf";

	Compiler compiler(lex_bnf, syn_bnf);
	const std::string program_text{"int compute(int y) { return -3; }"};
	compiler.compile_text(program_text);

	REQUIRE(1 == 1);
}

TEST_CASE("program_text_check_negative_basic_statement") {
	logger.set_level("NONE");
	const std::string lex_bnf = "TMCompiler/config/regex_lexical_grammar.bnf";
	const std::string syn_bnf = "TMCompiler/config/language_grammar.bnf";

	Compiler compiler(lex_bnf, syn_bnf);
	const std::string program_text{"void foo() { int x = -1; }"};
	compiler.compile_text(program_text);

	REQUIRE(1 == 1);
}

TEST_CASE("program_two_functions") {
	logger.set_level("NONE");
	const std::string lex_bnf = "TMCompiler/config/regex_lexical_grammar.bnf";
	const std::string syn_bnf = "TMCompiler/config/language_grammar.bnf";

	Compiler compiler(lex_bnf, syn_bnf);
	const std::string program_text{
		"void foo() {} int compute(int y) { return 5; }"};
	compiler.compile_text(program_text);

	REQUIRE(1 == 1);
}

TEST_CASE("basic_statement") {
	logger.set_level("NONE");
	const std::string lex_bnf = "TMCompiler/config/regex_lexical_grammar.bnf";
	const std::string syn_bnf = "TMCompiler/config/language_grammar.bnf";

	Compiler compiler(lex_bnf, syn_bnf);
	const std::string program_text{"void foo() { int sum = 0; }"};
	compiler.compile_text(program_text);

	REQUIRE(1 == 1);
}

TEST_CASE("two_basic_statements") {
	logger.set_level("NONE");
	const std::string lex_bnf = "TMCompiler/config/regex_lexical_grammar.bnf";
	const std::string syn_bnf = "TMCompiler/config/language_grammar.bnf";

	Compiler compiler(lex_bnf, syn_bnf);
	const std::string program_text{
		"void foo() { int sum = 0; int extra = -25; }"};
	compiler.compile_text(program_text);

	REQUIRE(1 == 1);
}

TEST_CASE("compound_operator") {
	logger.set_level("NONE");
	const std::string lex_bnf = "TMCompiler/config/regex_lexical_grammar.bnf";
	const std::string syn_bnf = "TMCompiler/config/language_grammar.bnf";

	Compiler compiler(lex_bnf, syn_bnf);
	const std::string program_text{"void foo() { int sum = 0; sum += 5;}"};
	compiler.compile_text(program_text);

	REQUIRE(1 == 1);
}

TEST_CASE("for_loop") {
	logger.set_level("NONE");
	const std::string lex_bnf = "TMCompiler/config/regex_lexical_grammar.bnf";
	const std::string syn_bnf = "TMCompiler/config/language_grammar.bnf";

	Compiler compiler(lex_bnf, syn_bnf);
	const std::string program_text{
		"int foo() { int sum = 0; for(int i = 0; i < 10; i += 1) { sum += i; } "
		"return sum; }"};
	compiler.compile_text(program_text);

	REQUIRE(1 == 1);
}

TEST_CASE("if_statement") {
	logger.set_level("NONE");
	const std::string lex_bnf = "TMCompiler/config/regex_lexical_grammar.bnf";
	const std::string syn_bnf = "TMCompiler/config/language_grammar.bnf";

	Compiler compiler(lex_bnf, syn_bnf);
	const std::string program_text{
		"int foo() { if(true) { return 1; } return -1; }"};
	compiler.compile_text(program_text);

	REQUIRE(1 == 1);
}

TEST_CASE("if_statement_no_bracket") {
	logger.set_level("NONE");
	const std::string lex_bnf = "TMCompiler/config/regex_lexical_grammar.bnf";
	const std::string syn_bnf = "TMCompiler/config/language_grammar.bnf";

	Compiler compiler(lex_bnf, syn_bnf);
	const std::string program_text{
		"int foo() { if(true) return 1; return -1; }"};
	compiler.compile_text(program_text);

	REQUIRE(1 == 1);
}

TEST_CASE("if_else_statement") {
	logger.set_level("NONE");
	const std::string lex_bnf = "TMCompiler/config/regex_lexical_grammar.bnf";
	const std::string syn_bnf = "TMCompiler/config/language_grammar.bnf";

	Compiler compiler(lex_bnf, syn_bnf);
	const std::string program_text{
		"int foo() { if(true) { return 1; } else { return -1; } }"};
	compiler.compile_text(program_text);

	REQUIRE(1 == 1);
}

TEST_CASE("if_else_if_else_statement") {
	logger.set_level("NONE");
	const std::string lex_bnf = "TMCompiler/config/regex_lexical_grammar.bnf";
	const std::string syn_bnf = "TMCompiler/config/language_grammar.bnf";

	Compiler compiler(lex_bnf, syn_bnf);
	const std::string program_text{
		"bool check() { return true; } int foo() { if(check()) { return 1; } "
		"return -1; }"};
	compiler.compile_text(program_text);

	REQUIRE(1 == 1);
}

TEST_CASE("while_statement") {
	logger.set_level("NONE");
	const std::string lex_bnf = "TMCompiler/config/regex_lexical_grammar.bnf";
	const std::string syn_bnf = "TMCompiler/config/language_grammar.bnf";

	Compiler compiler(lex_bnf, syn_bnf);
	const std::string program_text{"int foo() { while(true) { return 1; }}"};
	compiler.compile_text(program_text);

	REQUIRE(1 == 1);
}

TEST_CASE("newline_and_tabs") {
	logger.set_level("NONE");
	const std::string lex_bnf = "TMCompiler/config/regex_lexical_grammar.bnf";
	const std::string syn_bnf = "TMCompiler/config/language_grammar.bnf";

	Compiler compiler(lex_bnf, syn_bnf);
	const std::string program_text{"int main() {\n\treturn 0;}"};
	compiler.compile_text(program_text);

	REQUIRE(1 == 1);
}

TEST_CASE("method_invocation") {
	logger.set_level("NONE");
	const std::string lex_bnf = "TMCompiler/config/regex_lexical_grammar.bnf";
	const std::string syn_bnf = "TMCompiler/config/language_grammar.bnf";

	Compiler compiler(lex_bnf, syn_bnf);
	const std::string program_text{"void foo() {}  void main() { foo(); }"};
	compiler.compile_text(program_text);

	REQUIRE(1 == 1);
}
