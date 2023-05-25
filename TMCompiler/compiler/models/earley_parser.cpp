/**
 * Implement Earley Parsing that parses an input text using grammar rules.
 * See https://loup-vaillant.fr/tutorials/earley-parsing/recogniser
 */
#include "earley_parser.hpp"

#include <iostream>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include <TMCompiler/compiler/models/bnf_parser.hpp>  // Symbol
#include <TMCompiler/compiler/models/tokenizer.hpp>	  // Token
#include <TMCompiler/utils/logger/logger.hpp>		  // Logger

auto rule_to_string(const EarleyRule& rule) -> std::string {
	std::stringstream ss;
	ss << "Rule[" << rule.production.value << " -> ";
	for(std::size_t k = 0; k < rule.replacement.size(); ++k) {
		ss << rule.replacement[k].value;
		if(k + 1 < rule.replacement.size()) {
			ss << " ";
		}
	}
	ss << "]";

	return ss.str();
}

auto item_to_string(const FlippedEarleyItem item) -> std::string {
	std::stringstream ss;
	ss << "FItem(";
	ss << "rule=";
	ss << item.rule;
	ss << ", ";
	ss << "end=";
	ss << item.end;
	ss << ", ";
	ss << "next=";
	ss << item.next;
	ss << ")";

	return ss.str();
}

/**
 * Equality of EarleyItem: used to ensure uniqueness in Earley state sets
 * @param item1: left operand to compare with
 * @param item2: right operand to compare with
 * @return true iff both items have the same contents
 */
auto equals(const EarleyItem item1, const EarleyItem item2) -> bool {
	return item1.rule == item2.rule && item1.start == item2.start &&
		   item1.next == item2.next;
}

/**
 * Check if a given input symbol matches a symbol that a rule predicts.
 * For instance, is "103" really a "Number" symbol?
 * We use regex to determine if the input token is a valid symbol
 * @param predicted: Symbol that grammar rule predicts is next
 * @param actual: Token input that is scanned next
 * return true iff actual is an instance of predicted
 */
auto matches(const Symbol& predicted, const Token& actual) -> bool {
	// return predicted.value == actual.get_value();
	return std::regex_match(actual.get_value(), std::regex(predicted.value));
}

/**
 * Add an element to a set, maintaining the property that an element
 * appears at most once.
 * @param earley_set: list of elements
 * @param item: element to add to the list
 */
void add_earley_item_to_set(std::vector<EarleyItem>& earley_set,
							EarleyItem item) {
	// if duplicate found in set, do nothing
	for(EarleyItem element : earley_set) {
		if(equals(element, item)) {
			return;
		}
	}

	earley_set.push_back(item);
}

/**
 * Completion step in Earley parsing. When a rule is finished, a previous
 * rule that generated the finished rule must be moved forward a step.
 * @param earley_sets: global EarleyItems at each iteration / input token
 * @param current_earley_set_index: index of token we are currently parsing
 * @param grammar_rules: global set of grammar rules that is being used
 * to parse the input
 * @param item: Earley item that is finished. Use to find prev rule
 */
void complete(std::vector<std::vector<EarleyItem> >& earley_sets,
			  std::size_t current_earley_set_index,
			  std::vector<EarleyRule> grammar_rules, EarleyItem item) {
	EarleyRule finished_rule = grammar_rules[item.rule];
	Symbol finished_production = finished_rule.production;

	// find who generated this finished_rule. That previous rule has made a step
	// forward
	std::vector<EarleyItem> prev_earley_set = earley_sets[item.start];

	for(EarleyItem candidate : prev_earley_set) {
		// find rules that have <finished> production next to their dot
		EarleyRule candidate_rule = grammar_rules[candidate.rule];

		Symbol actual = candidate_rule.replacement[candidate.next];

		if(actual.value == finished_production.value &&
		   actual.terminal == finished_production.terminal) {
			EarleyItem next_item{candidate.rule, candidate.start,
								 1 + candidate.next};
			add_earley_item_to_set(earley_sets[current_earley_set_index],
								   next_item);
		}
	}
}

/**
 * Scan step of Earley parsing. When the rule's next symbol is a terminal
 * symbol, check if the next input token matches that terminal symbol.
 * @param earley_sets: global EarleyItems at each iteration / input token
 * @param current_earley_set_index: index of token we are currently parsing
 * @param item: Earley item whose next symbol in rule is a terminal symbol
 * @param predicted: next symbol in rule
 * @param actual: input token to match with predicted symbol
 */
void scan(std::vector<std::vector<EarleyItem> >& earley_sets,
		  std::size_t current_earley_set_index, EarleyItem item,
		  const Symbol& predicted, const Token& actual) {
	if(matches(predicted, actual)) {
		EarleyItem next_item{item.rule, item.start, 1 + item.next};
		add_earley_item_to_set(earley_sets[1 + current_earley_set_index],
							   next_item);
	}
}

/**
 * Predict step of Earley parsing. When the rule's next symbol is a
 * non-terminal symbol, add on another Earley Item to the current
 * Earley set, to signify that we later want to "recurse" down
 * the rule to check if the subrule / next Symbol holds
 * @param earley_sets: global EarleyItems at each iteration / input token
 * @param current_earley_set_index: index of token we are currently parsing
 * @param grammar_rules: global set of grammar rules that is being used
 * @param production: the current rule's next symbol (non-terminal).
 * We want to "recurse" down the current rule, to see if the input here
 * matches this production rule
 */
void predict(std::vector<std::vector<EarleyItem> >& earley_sets,
			 std::size_t current_earley_set_index,
			 std::vector<EarleyRule> grammar_rules, const Symbol& production) {
	for(std::size_t i = 0; i < grammar_rules.size(); ++i) {
		if(grammar_rules[i].production.value == production.value) {
			EarleyItem item{i, current_earley_set_index, 0};
			add_earley_item_to_set(earley_sets[current_earley_set_index], item);
		}
	}
}

/**
 * Build up the entire Earley state sets from a given input and set of
 * grammar rules. From it, backtrack from the end to find the parse of
 * the entire input program.
 * @param grammar_rules: list of production symbols to replacement rules
 * @param inputs: the "words" of the program / input
 * @param default_start: the top symbol of the parse; which production
 * rule in grammar_rules should start parsing the input
 * @return list of Earley state sets, of size inputs.size() + 1.
 * state_set[i] refers to the valid possible parses, before reading
 * token[i]. The last state set that has a finished rule and starts from
 * the beginning, is a valid grammar parse of the input tokens.
 */
auto build_earley_items(std::vector<EarleyRule> grammar_rules,
						std::vector<Token> inputs,
						const std::string& default_start)
	-> std::vector<std::vector<EarleyItem> > {
	std::vector<std::vector<EarleyItem> > earley_sets(1 + inputs.size());

	// initialize first state
	for(std::size_t i = 0; i < grammar_rules.size(); ++i) {
		if(grammar_rules[i].production.value == default_start) {
			EarleyItem item{i, 0, 0};
			add_earley_item_to_set(earley_sets[0], item);
		}
	}

	// create the remaining state sets, while traversing the input

	// for each earley_set, starting from 0 and working up, parse all
	// earley_items
	for(std::size_t i = 0; i < earley_sets.size(); ++i) {
		for(std::size_t j = 0; j < earley_sets[i].size(); ++j) {
			EarleyItem item = earley_sets[i][j];
			EarleyRule rule = grammar_rules[item.rule];

			// if EarleyRule ends in dot, COMPLETE
			if(item.next == rule.replacement.size()) {
				complete(earley_sets, i, grammar_rules, item);
				continue;
			}

			Symbol next_symbol = rule.replacement[item.next];
			// if next token after dot is terminal, SCAN
			if(next_symbol.terminal) {
				scan(earley_sets, i, item, next_symbol, inputs[i]);
			}
			// if next token after dot is non-terminal, PREDICT
			else {
				predict(earley_sets, i, grammar_rules, next_symbol);
			}
		}
	}

	return earley_sets;
}

/**
 * Change the meaning of Earley state sets: instead of storing the end position
 * explicitly, store the start position explicitly. This allows parsing from
 * the beginning of input, instead from the end.
 * In addition, filter out uncompleted parses.
 * @param earley_sets: Earley state sets to swap start and end positions
 * @return same Earley sets, just stored in a different way
 */
auto flip_earley_sets(const std::vector<std::vector<EarleyItem> >& earley_sets,
					  std::vector<EarleyRule> grammar_rules)
	-> std::vector<std::vector<FlippedEarleyItem> > {
	std::vector<std::vector<FlippedEarleyItem> > swapped(earley_sets.size());
	for(std::size_t i = 0; i < earley_sets.size(); ++i) {
		for(EarleyItem item : earley_sets[i]) {
			EarleyRule rule = grammar_rules[item.rule];
			if(rule.replacement.size() == item.next) {
				FlippedEarleyItem same_item{item.rule, i, item.next};
				swapped[item.start].push_back(same_item);
			}
		}
	}

	return swapped;
}

/**
 * Once the Earley state sets have been created, find the item that corresponds
 * to the highest-level rule that applies to the input tokens
 * @param earley_sets: created Earley state sets
 * @param grammar_rules: global set of grammar rules that is being used
 * @param default_start: first production rule that applies to input
 * @return FlippedEarleyItem that corresponds to highest-level rule
 */
auto find_top_item(std::vector<std::vector<FlippedEarleyItem> > earley_sets,
				   std::vector<EarleyRule> grammar_rules,
				   const std::string& default_start) -> FlippedEarleyItem {
	if(earley_sets.empty()) {
		throw std::invalid_argument("Earley state sets cannot be empty");
	}

	for(FlippedEarleyItem item : earley_sets.front()) {
		EarleyRule rule = grammar_rules[item.rule];
		if(item.end + 1 == earley_sets.size() &&
		   rule.production.value == default_start) {
			return item;
		}
	}

	throw std::logic_error("No successful parse of tokens");
}

/**
 * Given a current EarleyItem, recursively find its next child. "Next" referring
 * to the same rule, but with the dot advanced one step.
 * @param earley_sets: created Earley state sets
 * @param grammar_rules: global set of grammar rules that is being used
 * @param path: past history that led to the current node
 * @param curr_node: current EarleyItem to search its children
 * @return true iff there is a path from curr_node to its last child
 */
auto dfs(const std::vector<std::vector<FlippedEarleyItem> >& earley_sets,
		 const std::vector<EarleyRule>& grammar_rules,
		 const std::vector<Token>& input_tokens, const EarleyRule& parent_rule,
		 const std::size_t parent_rule_dot, const std::size_t token_location,
		 std::vector<std::pair<FlippedEarleyItem, std::size_t> >& path)
	-> bool {
	std::stringstream ss;
	ss << "Call DFS(parent_rule=" << rule_to_string(parent_rule)
	   << ", parent_rule_dot=" << parent_rule_dot
	   << ", token_location=" << token_location
	   << ", path history size=" << path.size() << ")";
	LOG("DEBUG", ss.str());

	// finished if dot at right-most of parent_rule
	if(parent_rule_dot == parent_rule.replacement.size()) {
		LOG("DEBUG",
			"Parent rule dot has moved to right most of parent_rule! Huzzah!");
		return true;
	}

	Symbol next_rule_symbol = parent_rule.replacement[parent_rule_dot];

	// if next part of rule is a terminal, check if it matches the token
	if(next_rule_symbol.terminal) {
		// if no more tokens, bad parse
		if(token_location >= input_tokens.size()) {
			return false;
		}

		bool match_result =
			matches(next_rule_symbol, input_tokens[token_location]);

		if(!match_result) {
			return false;
		}

		return dfs(earley_sets, grammar_rules, input_tokens, parent_rule,
				   1 + parent_rule_dot, 1 + token_location, path);
	}

	// next part of rule is non-terminal:
	// check all possible children where the rule's dot
	// is located

	for(FlippedEarleyItem possible_child : earley_sets[token_location]) {
		EarleyRule possible_child_rule = grammar_rules[possible_child.rule];

		if(possible_child_rule.production.value == next_rule_symbol.value &&
		   possible_child_rule.production.terminal ==
			   next_rule_symbol.terminal) {
			path.emplace_back(possible_child, token_location);

			bool child_ret =
				dfs(earley_sets, grammar_rules, input_tokens, parent_rule,
					1 + parent_rule_dot, possible_child.end, path);

			if(child_ret) {
				return true;
			}

			path.pop_back();
		}
	}

	return false;
}

/**
 * Wrapper function for dfs.
 * @param earley_sets: created Earley state sets
 * @param grammar_rules: global set of grammar rules that is being used
 * @param item: EarleyItem to find its path from start to finish, as dot
 * advances from beginning of rule to end of rule
 * @return list of item's path / children
 */
auto find_rule_steps(
	const std::vector<std::vector<FlippedEarleyItem> >& earley_sets,
	const std::vector<EarleyRule>& grammar_rules,
	const std::vector<Token>& input_tokens, FlippedEarleyItem item,
	std::size_t item_start)
	-> std::vector<std::pair<FlippedEarleyItem, std::size_t> > {
	EarleyRule parent_rule = grammar_rules[item.rule];
	std::vector<std::pair<FlippedEarleyItem, std::size_t> > children_path;
	bool search_result = dfs(earley_sets, grammar_rules, input_tokens,
							 parent_rule, 0, item_start, children_path);

	if(!search_result) {
		LOG("CRITICAL", "No match found in dfs");
	}

	// dfs returns parent + children. Return only children
	// children_path.erase(children_path.begin());

	return children_path;
}

/**
 * Build the parse tree given the Earley state sets.
 * @param earley_sets: created Earley state sets
 * @param grammar_rules: global set of grammar rules that is being used
 * @param default_start: the top symbol of the parse; which production
 * rule in grammar_rules should start parsing the input
 * @return list of EarleyItems, each with an index pointing to its parent
 */
auto build_earley_parse_tree(
	const std::vector<std::vector<EarleyItem> >& earley_sets,
	const std::vector<EarleyRule>& grammar_rules,
	const std::vector<Token>& input_tokens, const std::string& default_start)
	-> std::vector<std::tuple<FlippedEarleyItem, std::size_t, std::size_t> > {
	Logger::set_level("INFO");
	LOG("INFO", "Construct Parse Tree");

	std::vector<std::vector<FlippedEarleyItem> > flipped_earley_sets =
		flip_earley_sets(earley_sets, grammar_rules);

	LOG("DEBUG", "flipped_earley_sets = ");
	for(std::size_t i = 0; i < flipped_earley_sets.size(); ++i) {
		LOG("DEBUG", "\nstarts with " + std::to_string(i));
		for(FlippedEarleyItem item : flipped_earley_sets[i]) {
			LOG("DEBUG", item_to_string(item));
		}
	}
	LOG("DEBUG", "-------------------------------");

	FlippedEarleyItem top =
		find_top_item(flipped_earley_sets, grammar_rules, default_start);

	std::vector<std::tuple<FlippedEarleyItem, std::size_t, std::size_t> > tree;
	tree.emplace_back(top, 0, 0);

	for(std::size_t location = 0; location < tree.size(); ++location) {
		FlippedEarleyItem item = std::get<0>(tree[location]);

		LOG("DEBUG",
			std::string("Finding rule steps for item ") + item_to_string(item));

		std::vector<std::pair<FlippedEarleyItem, std::size_t> > children =
			find_rule_steps(flipped_earley_sets, grammar_rules, input_tokens,
							item, std::get<1>(tree[location]));

		for(std::pair<FlippedEarleyItem, std::size_t> child : children) {
			tree.emplace_back(child.first, child.second, location);
		}
	}

	return tree;
}
