#include <string>  // std::string

#include <TMCompiler/compiler/compiler.hpp>	 // Compiler

#include <catch2/catch_test_macros.hpp>

TEST_CASE("compiles") {
	Compiler compiler("TMCompiler/config/language.toml");
	std::string program_text;

	SECTION("program_text_void") {
		program_text = "void foo() {}";
	}
	SECTION("program_text_1") {
		program_text = "int compute(int y) { return 5; }";
	}
	SECTION("program_text_check_0") {
		program_text = "int compute(int y) { return 0; }";
	}
	SECTION("program_text_check_negative") {
		program_text = "int compute(int y) { return -3; }";
	}
	SECTION("program_text_check_negative_basic_statement") {
		program_text = "void foo() { int x = -1; }";
	}
	SECTION("program_two_functions") {
		program_text = "void foo() {} int compute(int y) { return 5; }";
	}
	SECTION("basic_statement") {
		program_text = "void foo() { int sum = 0; }";
	}
	SECTION("two_basic_statements") {
		program_text = "void foo() { int sum = 0; int extra = -25; }";
	}
	SECTION("compound_operator") {
		program_text = "void foo() { int sum = 0; sum += 5;}";
	}
	SECTION("for_loop") {
		program_text =
			"int foo() { int sum = 0; for(int i = 0; i < 10; i += 1) { sum += "
			"i; } return sum; }";
	}
	SECTION("if_statement") {
		program_text = "int foo() { if(true) { return 1; } return -1; }";
	}
	SECTION("if_statement_no_bracket") {
		program_text = "int foo() { if(true) return 1; return -1; }";
	}
	SECTION("if_else_statement") {
		program_text =
			"int foo() { if(true) { return 1; } else { return -1; } }";
	}
	SECTION("if_else_if_else_statement") {
		program_text =
			"bool check() { return true; } int foo() { if(check()) { return 1; "
			"} return -1; }";
	}
	SECTION("while_statement") {
		program_text = "int foo() { while(true) { return 1; }}";
	}
	SECTION("newline_and_tabs") {
		program_text = "int main() {\n\treturn 0;}";
	}
	SECTION("method_invocation") {
		program_text = "void foo() {}  void main() { foo(); }";
	}

	compiler.compile_text(program_text);
	SUCCEED("Compiled without errors");
}
