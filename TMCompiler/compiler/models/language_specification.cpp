/**
 * @file language_specification.cpp
 * @author Brian Wang (bwang1008@gmail.com)
 * @brief Parse programming language specification from TOML file into struct
 * @date 2024-01-28
 *
 * @copyright Copyright (c) 2024
 *
 * The programming language that the compiler reads and translates can be
 * specified by how to parse individual tokens (words like identifiers,
 * keywords, operators, ...) and how to group tokens together into logical
 * groups (the grammar). See TMCompiler/config/language.toml.
 *
 * This file implements the function read_language_specification_toml() that
 * reads from this TOML file and stores the information in the
 * LanguageSpecification struct, letting other pieces of code to use the
 * information in the TOML file.
 *
 * Adding new parsing:
 * 1) Modify the TOML file with the additional information
 * 2) Modify the LanguageSpecification struct to hold this information
 * 3) Create a new function in this file that looks like
 *      _read_<toml-path-to-new-info>(toml::node)
 * 4) Call this new function from read_language_specification_toml and store
 *      the information in the struct
 */

#include "language_specification.hpp"

#include <optional>	  // std::optional
#include <regex>	  // std::regex
#include <stdexcept>  // std::logic_error
#include <string>	  // std::string
#include <utility>	  // std::pair
#include <vector>	  // std::vector

#include <TMCompiler/compiler/models/grammar_symbol.hpp>  // GrammarSymbol
#include <toml++/toml.hpp>								  // toml namespace

/**
 * @brief Read in [[syntax.rules]] list from TOML file
 *
 * @param syntax_rules Array that is [[syntax.rules]] in TOML format.
 *        Ex: language_spec_table["syntax"]["rules"].as_array()
 * @return std::vector<Rule> Convert (name, production) pairs to vector of
 *         grammar Rules
 */
auto _read_syntax_rules(const toml::array* syntax_rules) -> std::vector<Rule> {
	std::vector<Rule> parsed_syntax_rules;

	syntax_rules->for_each([](auto&& element) {
		// rule_struct looks like {name = "...", production = [[...], [...],
		// ...]}
		const toml::table rule_struct = *element.as_table();

		const std::optional<std::string> name =
			rule_struct["name"].value<std::string>();
		if(!name.has_value()) {
			throw std::logic_error("Rule contains no 'name' attribute");
		}

		const toml::array* rhs_productions =
			rule_struct["production"].as_array();

		rhs_productions->for_each([](auto&& rhs_production) {
			// rhs_production looks like ["(", "<expression>", ")"]
			const toml::array* symbols = rhs_production.as_array();
			std::vector<GrammarSymbol> replacement;

			symbols->for_each(
				[replacement](toml::value<std::string>& wrapped_symbol) {
					// symbol looks like "<expression>"
					// symbol is non-terminal if enclosed in <>
					const std::string symbol = wrapped_symbol.get();
					// const bool non_terminal = !symbol.empty() && symbol[0] ==
					// '<' && symbol[symbol_str.size() - 1] == '>';
					// replacement.push_back(GrammarSymbol{symbol,
					// !non_terminal});
					replacement.push_back(GrammarSymbol{symbol, false});
				});

			parsed_syntax_rules.push_back(Rule{name.value(), replacement});
		});
	});

	return parsed_syntax_rules;
}

/**
 * @brief Read in "main" value under [syntax] header from TOML file
 *
 * @param syntax TOML node that points to the [syntax] header in the TOML file.
 *        Ex: language_spec_table["syntax"]
 * @return string value of "main"
 */
auto _read_syntax_main(const toml::array* syntax) -> std::string {
	const std::optional<std::string> main_production =
		syntax["main"].value<std::string>();
	if(!main_production.has_value()) {
		throw std::logic_error("No 'main' attribute in [syntax]");
	}

	return main_production.value();
}

/**
 * @brief Read specification file and put information in struct
 * LanguageSpecification.
 *
 * @param language_specification_toml: TOML file path containing the programming
 *        language specification, such as the regexes to parse tokens and the
 *        BNF specifying the grammar. Ex: "TMCompiler/config/language.toml"
 * @return LanguageSpecification information within the TOML file as struct
 */
auto read_language_specification_toml(
	const std::string& language_specification_toml) -> LanguageSpecification {
	const toml::table language_spec_table =
		toml::parse_file(language_specification_toml);

	const toml::array* token_regexes =
		language_spec_table["token"]["regexes"].as_array();
	std::vector<std::pair<std::string, std::regex> > parsed_token_regexes;

	token_regexes->for_each([](auto&& element) {
		// regex_struct looks like {name = "integer-constant", production =
		// '\d+'}
		const toml::table regex_struct = *element.as_table();

		const std::optional<std::string> name =
			regex_struct["name"].value<std::string>();
		const std::optional<std::string> production =
			regex_struct["production"].value<std::string>();
		const std::optional<bool> ignore = regex_struct["ignore"].value<bool>();

		if(!name.has_value()) {
			throw std::logic_error("Regex contains no 'name' attribute");
		}

		// ADD HERE ...?

		if(!production.has_value()) {
		}
	});

	const std::vector<Rule> parsed_syntax_rules =
		_read_syntax_rules(language_spec_table["syntax"]["rules"].as_array());

	const std::string main_production =
		language_spec_table["syntax"]["main"].value_or("");
}
