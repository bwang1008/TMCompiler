#include <iostream>
#include <tuple>

#include <TMCompiler/compiler/models/bnf_parser.hpp>  // Symbol
#include <TMCompiler/compiler/models/tokenizer.hpp>	  // Token
#include <TMCompiler/compiler/models/earley_parser.hpp>

std::vector<EarleyRule> get_grammar_rules() {
	std::vector<EarleyRule> grammar_rules;
	
	Symbol sum {"Sum", false};
	Symbol product {"Product", false};
	Symbol factor {"Factor", false};
	Symbol number {"Number", false};

	std::vector<Symbol> rhs;

	// 0: Sum -> Sum [+-] Product
	rhs.clear();
	rhs.push_back(sum);
	rhs.push_back(Symbol{"[+-]", true});
	rhs.push_back(product);

	EarleyRule rule0 {sum, rhs};
	grammar_rules.push_back(rule0);

	// 1: Sum -> Product
	rhs.clear();
	rhs.push_back(product);

	EarleyRule rule1 {sum, rhs};
	grammar_rules.push_back(rule1);

	// 2: Product -> Product [*/] Factor
	rhs.clear();
	rhs.push_back(product);
	rhs.push_back(Symbol{"[*/]", true});
	rhs.push_back(factor);

	EarleyRule rule2 {product, rhs};
	grammar_rules.push_back(rule2);

	// 3: Product -> Factor
	rhs.clear();
	rhs.push_back(factor);

	EarleyRule rule3 {product, rhs};
	grammar_rules.push_back(rule3);

	// 4: Factor -> '(' Sum ')'
	rhs.clear();
	rhs.push_back(Symbol{"\\(", true});
	rhs.push_back(sum);
	rhs.push_back(Symbol{"\\)", true});

	EarleyRule rule4 {factor, rhs};
	grammar_rules.push_back(rule4);

	// 5: Factory -> Number
	rhs.clear();
	rhs.push_back(number);

	EarleyRule rule5 {factor, rhs};
	grammar_rules.push_back(rule5);

	// 6: Number -> [0-9] Number
	rhs.clear();
	rhs.push_back(Symbol{"[0-9]", true});
	rhs.push_back(number);

	EarleyRule rule6 {number, rhs};
	grammar_rules.push_back(rule6);

	// 7: Number -> [0-9]
	rhs.clear();
	rhs.push_back(Symbol{"[0-9]", true});

	EarleyRule rule7 {number, rhs};
	grammar_rules.push_back(rule7);
	
	return grammar_rules;
}

std::vector<Token> get_inputs() {
	std::vector<Token> inputs;

	inputs.push_back(Token{"?", "1", 0, 0});
	inputs.push_back(Token{"?", "+", 0, 1});
	inputs.push_back(Token{"?", "(", 0, 2});
	inputs.push_back(Token{"?", "2", 0, 3});
	inputs.push_back(Token{"?", "*", 0, 4});
	inputs.push_back(Token{"?", "3", 0, 5});
	inputs.push_back(Token{"?", "-", 0, 6});
	inputs.push_back(Token{"?", "4", 0, 7});
	inputs.push_back(Token{"?", ")", 0, 8});

	return inputs;
}

void printItem(std::vector<EarleyRule> grammar_rules, EarleyItem item) {
	// std::cout << "{ rule = " << item.rule << ", start = " << item.start << ", next = " << item.next << "}" << std::endl;
	
	int max_width = 55;
	int total_length = 0;
	
	EarleyRule rule = grammar_rules[item.rule];
	std::cout << rule.production.value << " -> ";
	total_length += (rule.production.value.size()) + 4;

	for(std::size_t i = 0; i < item.next; ++i) {
		std::cout << rule.replacement[i].value << " ";
		total_length += rule.replacement[i].value.size() + 1;
	}

	std::cout << "*** ";
	total_length += 4;

	for(std::size_t i = item.next; i < rule.replacement.size(); ++i) {
		std::cout << rule.replacement[i].value << " ";
		total_length += rule.replacement[i].value.size() + 1;
	}


	while(total_length < max_width) {
		std::cout << " ";
		total_length += 1;
	}

	std::cout << "(" << item.start << ")";

	std::cout << std::endl;
}

void attempt_parse() {
	std::vector<EarleyRule> grammar_rules = get_grammar_rules();
	std::vector<Token> inputs = get_inputs();
	std::string default_start = "Sum";

	std::vector<std::vector<EarleyItem> > items = build_earley_items(grammar_rules, inputs, default_start);

	std::cout << "Finished building earley sets" << std::endl;

	std::cout << "How many Earley sets? " << items.size() << std::endl;

	for(std::size_t i = 0; i < items.size(); ++i) {
		std::cout << "Earley Set[" << i << "]" << std::endl;
		for(std::size_t j = 0; j < items[i].size(); ++j) {
			EarleyItem item = items[i][j];

			printItem(grammar_rules, item);
		}

		std::cout << std::endl;
	}


	std::cout << "======================================================" << std::endl;

	std::vector<std::tuple<FlippedEarleyItem, std::size_t, std::size_t> > tree = build_earley_parse_tree(items, grammar_rules, inputs, default_start);

	for(std::size_t i = 0; i < tree.size(); ++i) {
		std::cout << i << ": " << "{ " << std::get<0>(tree[i]).rule << ", " << std::get<0>(tree[i]).end << ", " << std::get<0>(tree[i]).next << "} " << " starting from " << std::get<1>(tree[i]) << " parent " << std::get<2>(tree[i]) << std::endl; 
	}

	std::cout << "TOKENS:" << std::endl;
	for(std::size_t i = 0; i < inputs.size(); ++i) {
		std::cout << inputs[i].value;
	}
	std::cout << std::endl;
	for(std::size_t i = 0; i < inputs.size(); ++i) {
		std::cout << (i % 10);
	}
	std::cout << std::endl;

	std::cout << "RULES:" << std::endl;
	std::cout << "0: Sum -> Sum [+-] Product" << std::endl;
	std::cout << "1: Sum -> Product" << std::endl;
	std::cout << "2: Product -> Product [*/] Factor" << std::endl;
	std::cout << "3: Product -> Factor" << std::endl;
	std::cout << "4: Factor -> '(' Sum ')'" << std::endl;
	std::cout << "5: Factory -> Number" << std::endl;
	std::cout << "6: Number -> [0-9] Number" << std::endl;
	std::cout << "7: Number -> [0-9]" << std::endl;


	
	
}

int main() {
	std::cout << "BEGIN" << std::endl;
	attempt_parse();
	std::cout << "DONE" << std::endl;

	return 0;
}
