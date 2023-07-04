#include "grammar.hpp"

#include <fstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>	// std::move
#include <vector>

#include <TMCompiler/compiler/front_end/bnf_parser.hpp>		// BnfParser
#include <TMCompiler/compiler/front_end/earley_parser.hpp>	// build_earley_items, build_earley_parse_tree, EarleyItem, SubParse
#include <TMCompiler/compiler/models/grammar_symbol.hpp>  // GrammarSymbol
#include <TMCompiler/compiler/models/token.hpp>			  // Token

Grammar::Grammar(std::ifstream& bnf_file, std::string default_start): default_start(std::move(default_start)) {
	if(!bnf_file.is_open()) {
		throw std::invalid_argument("Unable to open grammar file");
	}

	rules = BnfParser::parse_rules(bnf_file);
}

// LR parsing? Earley parsing?
// AbstractSyntaxTreeNode Grammar::parse_helper(const std::vector<char>& program,
											 // int cursor,
											 // GrammarSymbol symbol) {
// }

auto Grammar::parse(const std::vector<Token>& input_tokens) const -> std::vector<SubParse> {
	const std::vector<std::vector<EarleyItem> > earley_sets = build_earley_items(rules, input_tokens, default_start);
	return build_earley_parse_tree(earley_sets, rules, input_tokens, default_start);
}

auto Grammar::get_rules() const -> std::vector<Rule> {
	return rules;
}
