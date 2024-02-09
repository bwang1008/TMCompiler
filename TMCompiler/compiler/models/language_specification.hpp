#include <regex>		  // std::regex
#include <string>		  // std::string
#include <unordered_set>  // std::unorderedset
#include <utility>		  // std::pair
#include <vector>		  // std::vector

#include <TMCompiler/compiler/models/rule.hpp>	// Rule

struct LanguageSpecification {
	// pairs of token names and their corresponding regex pattern
	// found in language.toml [[token.regexes]] tables
	// ex: [("whitespace", "\s+"), ("integer-constant", "\d+"), ...]
	std::vector<std::pair<std::string, std::regex> > token_regexes;

	// name of tokens that should be ignored when parsing tokens into the
	// grammar found in TOML file under [[token.regexes.ignore]]
	std::unordered_set<std::string> token_regexes_ignore;

	// value of [syntax.main]
	std::string syntax_main;

	// list of rules in [[syntax.rules]]
	std::vector<Rule> syntax_rules;

	std::string version;
};

auto read_language_specification_toml(
	const std::string& language_specification_toml) -> LanguageSpecification;
