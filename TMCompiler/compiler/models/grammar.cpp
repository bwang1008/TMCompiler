#include "grammar.hpp"

#include <set>		// std::set
#include <string>	// std::string
#include <utility>	// std::move
#include <vector>	// std::vector

#include <TMCompiler/compiler/models/grammar_symbol.hpp>  // GrammarSymbol
#include <TMCompiler/compiler/models/rule.hpp>			  // Rule
#include <TMCompiler/compiler/models/token.hpp>			  // Token
#include <TMCompiler/compiler/parser/earley_parser.hpp>	 // build_earley_items, build_earley_parse_tree, EarleyItem, SubParse

Grammar::Grammar(std::vector<Rule> _rules, std::string _default_start)
	: rules(std::move(_rules)), default_start(std::move(_default_start)) {
}

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
auto Grammar::mark_special_symbols_as_terminal(
	const std::set<std::string>& special_tokens) -> void {
	for(Rule& rule : rules) {
		if(special_tokens.find(rule.production.value) != special_tokens.end()) {
			rule.production.terminal = true;
		}

		for(GrammarSymbol& symbol : rule.replacement) {
			if(special_tokens.find(symbol.value) != special_tokens.end()) {
				symbol.terminal = true;
			}
		}
	}
}
