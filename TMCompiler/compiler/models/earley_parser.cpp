/**
 * Implement Earley Parsing that parses an input text using grammar rules.
 * See https://loup-vaillant.fr/tutorials/earley-parsing/recogniser
 */
#include "earley_parser.hpp"

#include <regex>
#include <stdexcept>
#include <string>
#include <vector>

#include <TMCompiler/compiler/models/bnf_parser.hpp>  // Symbol
#include <TMCompiler/compiler/models/tokenizer.hpp>	  // Token

struct FlippedEarleyItem {
	std::size_t rule;
	std::size_t end;
	std::size_t next;
};

/**
 * Equality of EarleyItem: used to ensure uniqueness in Earley state sets
 * @param item1: left operand to compare with
 * @param item2: right operand to compare with
 * @return true iff both items have the same contents
 */
auto equals(EarleyItem item1, EarleyItem item2) -> bool {
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
 * @param earley_sets: Earley state sets to swap start and end positions
 * @return same Earley sets, just stored in a different way
 */
auto flip_earley_sets(const std::vector<std::vector<EarleyItem> >& earley_sets) -> std::vector<std::vector<FlippedEarleyItem> > {
	std::vector<std::vector<FlippedEarleyItem> > swapped(earley_sets.size());
	for(std::size_t i = 0; i < earley_sets.size(); ++i) {
		for(EarleyItem item : earley_sets[i]) {
			FlippedEarleyItem same_item { item.rule, i, item.next };
			swapped[item.start].push_back(same_item);
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
auto find_top_item(std::vector<std::vector<FlippedEarleyItem> > earley_sets, std::vector<EarleyRule> grammar_rules, const std::string& default_start) -> FlippedEarleyItem {
	if(earley_sets.empty()) {
		throw std::invalid_argument("Earley state sets cannot be empty");
	}

	for(FlippedEarleyItem item : earley_sets.front()) {
		EarleyRule rule = grammar_rules[item.rule];
		if(item.end + 1 == earley_sets.size() && rule.production.value == default_start) {
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
auto dfs(std::vector<std::vector<FlippedEarleyItem> > earley_sets, std::vector<EarleyRule> grammar_rules, std::vector<FlippedEarleyItem>& path, FlippedEarleyItem curr_node) -> bool {
	path.push_back(curr_node);

	EarleyRule rule = grammar_rules[curr_node.rule];
	if(curr_node.next == rule.replacement.size()) {
		return true;
	}

	// now that you accepted the rule, and the dot in the rule is not at the end,
	// move the dot one step forward in the rule: find all next EarleyItems 
	// with the same rule, but the dot is one more forward, and recurse on DFS
	for(FlippedEarleyItem potential_child : earley_sets[curr_node.end]) {
		if(potential_child.rule == curr_node.rule && potential_child.next == 1 + curr_node.next) {
			bool child_search_result = dfs(earley_sets, grammar_rules, path, potential_child);
			if(child_search_result) {
				return true;
			}
		}
	}

	// didn't have any valid children. Thus, this current node is not valid either
	path.pop_back();
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
auto find_rule_steps(const std::vector<std::vector<FlippedEarleyItem> >& earley_sets, const std::vector<EarleyRule>& grammar_rules, FlippedEarleyItem item) -> std::vector<FlippedEarleyItem> {
	std::vector<FlippedEarleyItem> children_path;
	dfs(earley_sets, grammar_rules, children_path, item);

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
auto build_earley_parse_tree(const std::vector<std::vector<EarleyItem> >& earley_sets, const std::vector<EarleyRule>& grammar_rules, const std::string& default_start) -> std::vector<std::pair<FlippedEarleyItem, std::size_t> > {
	std::vector<std::vector<FlippedEarleyItem> > flipped_earley_sets = flip_earley_sets(earley_sets);
	FlippedEarleyItem top = find_top_item(flipped_earley_sets, grammar_rules, default_start);

	std::vector<std::pair<FlippedEarleyItem, std::size_t> > tree;
	tree.emplace_back(top, 0);

	for(std::size_t location = 0; location < tree.size(); ++location) {
		FlippedEarleyItem item = tree[location].first;
		std::vector<FlippedEarleyItem> children = find_rule_steps(flipped_earley_sets, grammar_rules, item);

		for(FlippedEarleyItem child : children) {
			tree.emplace_back(child, location);
		}
	}

	return tree;
}
