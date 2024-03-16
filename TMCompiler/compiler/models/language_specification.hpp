#include <regex>		  // std::regex
#include <string>		  // std::string
#include <unordered_set>  // std::unorderedset
#include <utility>		  // std::pair
#include <vector>		  // std::vector

#include <TMCompiler/compiler/models/rule.hpp>	// Rule

namespace LanguageSpecification {
extern std::string title;
extern std::string description;
extern std::string version;

// pairs of token names and their corresponding regex pattern
// found in language.toml [[token.regexes]] tables
// ex: [("whitespace", "\s+"), ("integer-constant", "\d+"), ...]
extern std::vector<std::pair<std::string, std::regex> > token_regexes;

// name of tokens that should be ignored when parsing tokens into the
// grammar found in TOML file under [[token.regexes.ignore]]
extern std::unordered_set<std::string> token_regexes_ignore;

// value of [syntax.main]
extern std::string syntax_main;

// list of rules in [[syntax.rules]]
extern std::vector<Rule> syntax_rules;

auto read_language_specification_toml(
	const std::string& language_specification_toml) -> void;
};	// namespace LanguageSpecification
