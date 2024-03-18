#include <string>  // std::string

#include <TMCompiler/compiler/models/language_specification.hpp>  // LanguageSpecification

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Reads TOML file") {
	const std::string config_file = "TMCompiler/config/language.toml";
	LanguageSpecification spec =
		LanguageSpecification::read_language_specification_toml(config_file);

	REQUIRE(spec.spec_file_name == config_file);
	REQUIRE(spec.title == "Language Specification");
	REQUIRE(!spec.description.empty());
	REQUIRE(!spec.version.empty());

	REQUIRE(!spec.token_regexes.empty());
	REQUIRE(!spec.token_regexes_ignore.empty());

	REQUIRE(!spec.syntax_main.empty());
	REQUIRE(!spec.syntax_rules.empty());
}
