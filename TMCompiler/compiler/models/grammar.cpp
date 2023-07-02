#include "grammar.hpp"

#include <fstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <TMCompiler/compiler/models/grammar_symbol.hpp>  // GrammarSymbol
#include <TMCompiler/compiler/models/token.hpp>			  // Token

Grammar::Grammar(std::ifstream& bnf_file) : default_start("") {
	if(!bnf_file.is_open()) {
		throw std::invalid_argument("Unable to open grammar file");
	}

	rules = BnfParser::parse_rules(bnf_file);
}

// LR parsing? Earley parsing?
AbstractSyntaxTreeNode Grammar::parse_helper(const std::vector<char>& program,
											 int cursor,
											 GrammarSymbol symbol) {
}

auto Grammar::parse(const std::vector<char>& program) -> std::vector<SubParse> {



}
