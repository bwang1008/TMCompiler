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

#include <iostream>
#include <optional>		  // std::optional
#include <regex>		  // std::regex
#include <stdexcept>	  // std::logic_error
#include <string>		  // std::string
#include <unordered_set>  // std::unordered_set
#include <utility>		  // std::pair
#include <vector>		  // std::vector

#include <TMCompiler/compiler/models/grammar_symbol.hpp>  // GrammarSymbol
#include <TMCompiler/compiler/models/rule.hpp>			  // Rule

#include <toml++/toml.hpp>	// toml namespace

/**
 * @brief Read in string attribute from top-level of TOML file
 *
 * For instance, given the following TOML section:
 * ------------
 * title = "Language Specification"
 * ------------
 * with parameter "key" := "title" should return "Language Specification"
 *
 * @param language_spec_table Table of entire TOML file
 * @param key Attribute in top-level of TOML (not under a [header])
 *		  that has a string right-hand-side.
 * @return std::string that is on right-hand-side of "="
 */
auto _read_top_level_string(const toml::table& language_spec_table,
							const std::string& key) -> std::string {
	const std::optional<std::string> rhs =
		language_spec_table[key].value<std::string>();
	if(!rhs.has_value()) {
		throw std::logic_error(key +
							   " attribute in top-level TOML either missing "
							   "or not parsable as string");
	}

	return rhs.value();
}

/**
 * @brief Read in [[token.regexes]] list from TOML file
 *
 * For instance, given the following TOML section:
 * ------------
 *  [[token.regexes]]
 *  name = "whitespace"
 *  production = '\s+'
 *  ignore = true
 *
 *  [[token.regexes]]
 *  name = "integer-constant"
 *  production = '\d+'
 * ------------
 *  should return (
 *		vector[
 *			("whitespace", regex("\s+")),
 *			("integer-constant", regex("\d+")),
 *		],
 *		unordered_set(["whitespace"]),
 *  )
 *
 * @param syntax_rules Array that is [[token.regexes]] in TOML format.
 *        Ex: language_spec_table["token"]["regexes"].as_array()
 * @return std::pair<std::vector<std::pair<std::string, std::regex>>,
 *					 std::unordered_set<std::string>>
 *		   : First element returned is all (name, production) pairs; the second
 * element is a set of all names that have "ignore = true" grammar Rules
 */
auto _read_token_regexes(const toml::array* token_regexes)
	-> std::pair<std::vector<std::pair<std::string, std::regex>>,
				 std::unordered_set<std::string>> {
	std::vector<std::pair<std::string, std::regex>> name_and_regexes;
	std::unordered_set<std::string> ignore_set;

	for(const toml::v3::node& token_regex_node : *token_regexes) {
		// regex_table contains {name: "whitespace", production = '\s+', ignore
		// = true}
		const toml::table* regex_table = token_regex_node.as_table();

		// name = "whitespace"
		const std::optional<std::string> name =
			(*regex_table)["name"].value<std::string>();
		if(!name.has_value()) {
			throw std::logic_error(
				"'name' attribute in [[token.regexes]] either missing or not "
				"parsable as string");
		}

		// regex_pattern = "\s+"
		const std::optional<std::string> regex_pattern =
			(*regex_table)["production"].value<std::string>();
		if(!regex_pattern.has_value()) {
			throw std::logic_error(
				"'production' attribute in [[token.regexes]] either missing or "
				"not parsable as string");
		}

		// ignore = true
		const std::optional<bool> is_ignore_opt =
			(*regex_table)["ignore"].value<bool>();
		const bool is_ignore =
			(is_ignore_opt.has_value() && is_ignore_opt.value());

		name_and_regexes.emplace_back(name.value(),
									  std::regex(regex_pattern.value()));

		if(is_ignore) {
			ignore_set.insert(name.value());
		}
	}

	return {name_and_regexes, ignore_set};
}

/**
 * @brief Read in array of arrays of key "production" under [syntax.rules]
 *
 * @param production Array under each "production" key in [[syntax.rules]]
 *        header, such as {production = [["<type>", "<identifier>"]]}
 * @return std::vector<std::vector<GrammarSymbol>> Convert 2D array to 2D vector
 */
auto _read_syntax_rule_production(const toml::array* production)
	-> std::vector<std::vector<GrammarSymbol>> {
	std::vector<std::vector<GrammarSymbol>> production_as_vec;

	for(const toml::v3::node& production_node : *production) {
		// production_node / rhs points to a RHS of a rule, like ["<type>",
		// "<identifier>"]
		const toml::array* rhs = production_node.as_array();
		std::vector<GrammarSymbol> rhs_vec;

		for(const toml::v3::node& symbol_node : *rhs) {
			// symbol_node points to a string like "<type>"
			const std::optional<std::string> symbol_string_opt =
				symbol_node.value<std::string>();

			if(!symbol_string_opt.has_value()) {
				throw std::logic_error(
					"Symbol in production cannot be parsed as a string");
			}

			const std::string& symbol_string = symbol_string_opt.value();

			// nonterminal symbols look like "<type>". That is, surrounded by
			// angle brackets. If so, remove surrounding angle brackets.
			const bool non_terminal =
				!symbol_string.empty() && (symbol_string[0] == '<') &&
				(symbol_string[symbol_string.size() - 1] == '>');

			if(non_terminal) {
				rhs_vec.push_back(GrammarSymbol{
					symbol_string.substr(1, symbol_string.size() - 2),
					!non_terminal});
			} else {
				rhs_vec.push_back(GrammarSymbol{symbol_string, !non_terminal});
			}
		}

		production_as_vec.push_back(rhs_vec);
	}

	return production_as_vec;
}

/**
 * @brief Read in [[syntax.rules]] list from TOML file
 *
 * For instance, given the following TOML section:
 * ------------
 *  [[syntax.rules]]
 *  name = "function-header"
 *  production = [
 *     ["<return-type>", "<identifier>", "(", ")"],
 *     ["<return-type>", "<identifier>", "(", "<formal-parameter-list>", ")"],
 *  ]
 * ------------
 *  should return vector[
 *		Rule("function-header", vector["return-type", "identifier", "(", ")"]),
 *		Rule("function-header", vector["return-type", "identifier", "(",
 *"formal-parameter-list", ")"]),
 *  ]
 *
 * @param syntax_rules Array that is [[syntax.rules]] in TOML format.
 *        Ex: language_spec_table["syntax"]["rules"].as_array()
 * @return std::vector<Rule> Convert (name, production) pairs to vector of
 *         grammar Rules
 */
auto _read_syntax_rules(const toml::array* syntax_rules) -> std::vector<Rule> {
	std::vector<Rule> parsed_syntax_rules;
	for(const toml::v3::node& syntax_rule_node : *syntax_rules) {
		// rule_table contains {name: "function-header", production =
		// [["<return-type>", "<identifier>", "(", ")"], ...]}
		const toml::table* rule_table = syntax_rule_node.as_table();

		// name = "function-header"
		const std::optional<std::string> name =
			(*rule_table)["name"].value<std::string>();
		if(!name.has_value()) {
			throw std::logic_error(
				"'name' attribute in [[syntax.rules]] either missing or not "
				"parsable as string");
		}
		const GrammarSymbol left_hand_side{name.value(), false};

		// production = [["<return-type>", "<identifier>", "(", ")"], ...]
		const toml::array* production_node =
			(*rule_table)["production"].as_array();
		const std::vector<std::vector<GrammarSymbol>> productions =
			_read_syntax_rule_production(production_node);

		for(const std::vector<GrammarSymbol>& production : productions) {
			// add Rule("function-header", vector["return-type", "identifier",
			// "(", ")"])
			parsed_syntax_rules.push_back(Rule{left_hand_side, production});
		}
	}

	return parsed_syntax_rules;
}

/**
 * @brief Read in "main" value under [syntax] header from TOML file
 *
 * @param syntax TOML node that points to the [syntax] header in the TOML
 * file. Ex: language_spec_table["syntax"].as_table()
 * @return string value of "main"
 */
auto _read_syntax_main(const toml::table* syntax) -> std::string {
	const std::optional<std::string> main_production =
		(*syntax)["main"].value<std::string>();
	if(!main_production.has_value()) {
		throw std::logic_error(
			"'main' attribute in [syntax] either missing or not parsable as "
			"string");
	}

	return main_production.value();
}

/**
 * @brief Read specification file and put information in struct
 * LanguageSpecification.
 *
 * @param language_specification_toml: TOML file path containing the
 * programming language specification, such as the regexes to parse tokens
 * and the BNF specifying the grammar. Ex: "TMCompiler/config/language.toml"
 * @return LanguageSpecification information within the TOML file as struct
 */
auto read_language_specification_toml(
	const std::string& language_specification_toml) -> LanguageSpecification {
	const toml::table language_spec_table =
		toml::parse_file(language_specification_toml);

	const std::string parsed_title =
		_read_top_level_string(language_spec_table, "title");
	const std::string parsed_description =
		_read_top_level_string(language_spec_table, "description");
	const std::string parsed_version =
		_read_top_level_string(language_spec_table, "version");

	const std::pair<std::vector<std::pair<std::string, std::regex>>,
					std::unordered_set<std::string>>
		name_regex_and_ignore_set = _read_token_regexes(
			language_spec_table["token"]["regexes"].as_array());

	const std::vector<std::pair<std::string, std::regex>> parsed_token_regexes =
		name_regex_and_ignore_set.first;

	const std::unordered_set<std::string> parsed_token_regexes_ignore =
		name_regex_and_ignore_set.second;

	const std::vector<Rule> parsed_syntax_rules =
		_read_syntax_rules(language_spec_table["syntax"]["rules"].as_array());

	const std::string parsed_syntax_main =
		_read_syntax_main(language_spec_table["syntax"].as_table());

	return LanguageSpecification{
		language_specification_toml,
		parsed_title,
		parsed_description,
		parsed_version,
		parsed_token_regexes,
		parsed_token_regexes_ignore,
		parsed_syntax_main,
		parsed_syntax_rules,
	};
}

auto main() -> int {
	try {
		LanguageSpecification ls =
			read_language_specification_toml("TMCompiler/config/language.toml");
		std::cout << "Parsing from " << ls.spec_file_name << std::endl;

		std::cout << "title = " << ls.title << std::endl;
		std::cout << "description = " << ls.description << std::endl;
		std::cout << "version = " << ls.version << std::endl;

		std::cout << "len of token_regexes: " << ls.token_regexes.size()
				  << std::endl;
		for(const auto& x : ls.token_regexes) {
			std::cout << "\t" << x.first << std::endl;
		}

		std::cout << "len of token_regexes_ignore: "
				  << ls.token_regexes_ignore.size() << std::endl;
		for(const auto& x : ls.token_regexes_ignore) {
			std::cout << "\t" << x << std::endl;
		}

		std::cout << "syntax_main = " << ls.syntax_main << std::endl;

		std::cout << "len of syntax_rules = " << ls.syntax_rules.size()
				  << std::endl;

		const std::size_t max_rules_to_show = 20;
		for(std::size_t i = 0; i < max_rules_to_show; ++i) {
			Rule rule = ls.syntax_rules[i];
			std::cout << "\t" << rule.production.value << ": [";
			for(const GrammarSymbol& gs : rule.replacement) {
				std::cout << "\"" << gs.value << "\", ";
			}
			std::cout << "]" << std::endl;
		}
	} catch(const std::logic_error& e) {
		std::cout << "Logic error parsing TOML file" << std::endl;
		std::cout << e.what() << std::endl;
		return 1;
	} catch(const std::exception& e) {
		std::cout << "UNKNOWN EXCEPTION" << std::endl;
		std::cout << e.what() << std::endl;
		return -1;
	}

	return 0;
}
