#include "grammar.hpp"

#include <fstream>
#include <set>		// std::set
#include <stdexcept>
#include <string>
#include <utility>	// std::move
#include <vector>

#include <TMCompiler/compiler/front_end/bnf_parser.hpp>	 // BnfParser
#include <TMCompiler/compiler/front_end/earley_parser.hpp>	// build_earley_items, build_earley_parse_tree, EarleyItem, SubParse
#include <TMCompiler/compiler/models/grammar_symbol.hpp>  // GrammarSymbol
#include <TMCompiler/compiler/models/token.hpp>			  // Token

Grammar::Grammar(std::ifstream& bnf_file, std::string top_symbol)
	: default_start(std::move(top_symbol)) {
	if(!bnf_file.is_open()) {
		throw std::invalid_argument("Unable to open grammar file");
	}

	rules = BnfParser::parse_rules(bnf_file);
}

// LR parsing? Earley parsing?
// AbstractSyntaxTreeNode Grammar::parse_helper(const std::vector<char>&
// program, int cursor, GrammarSymbol symbol) {
// }

auto Grammar::parse(const std::vector<Token>& input_tokens) const
	-> std::vector<SubParse> {
	const std::vector<std::vector<EarleyItem> > earley_sets =
		build_earley_items(rules, input_tokens, default_start);
	return build_earley_parse_tree(
		earley_sets, rules, input_tokens, default_start);
}

auto Grammar::get_rules() const -> std::vector<Rule> {
	return rules;
}

/**
 * Mark some special rule symbols from default non-terminal to terminal.
 *
 * By default, rules like <a> ::= <b> "c" will mark symbols in angle-brackets
 * as non-terminal, in that it expects the BNF file to have an expansion
 * of the non-terminal, like <b>. However, some symbols like <identifier>
 * are actually terminal, since the parsing for it was done in the
 * first round of parsing, the lexical parsing. So instead, we treat
 * <identifier> as terminal.
 *
 * @param special_tokens: a list of strings to change in current rules
 */
auto Grammar::mark_special_symbols_as_terminal(const std::set<std::string> special_tokens) -> void {
	for(std::size_t i = 0; i < rules.size(); ++i) {
		if(special_tokens.find(rules[i].production.value) != special_tokens.end()) {
			rules[i].production.terminal = true;
		}

		for(GrammarSymbol& symbol: rules[i].replacement) {
			if(special_tokens.find(symbol.value) != special_tokens.end()) {
				symbol.terminal = true;
			}
		}
	}
}
