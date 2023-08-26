#include <algorithm>
#include <iostream>
#include <map>
#include <string>
#include <tuple>
#include <vector>

#include <TMCompiler/compiler/front_end/bnf_parser.hpp>
#include <TMCompiler/compiler/front_end/earley_parser.hpp>
#include <TMCompiler/compiler/models/grammar_symbol.hpp>  // GrammarSymbol
#include <TMCompiler/compiler/models/rule.hpp>	// Rule
#include <TMCompiler/compiler/models/token.hpp>	  // Token
#include <TMCompiler/utils/logger/logger.hpp>

#include <TMCompiler/compiler/compiler.hpp>

std::vector<Rule> get_grammar_rules() {
	std::vector<Rule> grammar_rules;
	
	GrammarSymbol sum {"Sum", false};
	GrammarSymbol product {"Product", false};
	GrammarSymbol factor {"Factor", false};
	GrammarSymbol number {"Number", false};

	std::vector<GrammarSymbol> rhs;

	// 0: Sum -> Sum [+-] Product
	rhs.clear();
	rhs.push_back(sum);
	rhs.push_back(GrammarSymbol{"[+-]", true});
	rhs.push_back(product);

	Rule rule0 {sum, rhs};
	grammar_rules.push_back(rule0);

	// 1: Sum -> Product
	rhs.clear();
	rhs.push_back(product);

	Rule rule1 {sum, rhs};
	grammar_rules.push_back(rule1);

	// 2: Product -> Product [*/] Factor
	rhs.clear();
	rhs.push_back(product);
	rhs.push_back(GrammarSymbol{"[*/]", true});
	rhs.push_back(factor);

	Rule rule2 {product, rhs};
	grammar_rules.push_back(rule2);

	// 3: Product -> Factor
	rhs.clear();
	rhs.push_back(factor);

	Rule rule3 {product, rhs};
	grammar_rules.push_back(rule3);

	// 4: Factor -> '(' Sum ')'
	rhs.clear();
	rhs.push_back(GrammarSymbol{"\\(", true});
	rhs.push_back(sum);
	rhs.push_back(GrammarSymbol{"\\)", true});

	Rule rule4 {factor, rhs};
	grammar_rules.push_back(rule4);

	// 5: Factory -> Number
	rhs.clear();
	rhs.push_back(number);

	Rule rule5 {factor, rhs};
	grammar_rules.push_back(rule5);

	// 6: Number -> [0-9] Number
	rhs.clear();
	rhs.push_back(GrammarSymbol{"[0-9]", true});
	rhs.push_back(number);

	Rule rule6 {number, rhs};
	grammar_rules.push_back(rule6);

	// 7: Number -> [0-9]
	rhs.clear();
	rhs.push_back(GrammarSymbol{"[0-9]", true});

	Rule rule7 {number, rhs};
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

void printItem(std::vector<Rule> grammar_rules, EarleyItem item) {
	// std::cout << "{ rule = " << item.rule << ", start = " << item.start << ", next = " << item.next << "}" << std::endl;
	
	const std::size_t max_width = 55;
	std::size_t total_length = 0;
	
	Rule rule = grammar_rules[item.rule];
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

void print_tree(std::vector<Token> tokens, std::vector<SubParse> tree) {
	const std::size_t fence_length = 2;

	std::size_t size = tokens.size();
	std::size_t length = fence_length * size + 1;

	// write out indices
	for(std::size_t i = 0; i <= size; ++i) {
		std::cout << i % 10;
		for(std::size_t j = 1; j < fence_length; ++j) {
			std::cout << " ";
		}
	}
	std::cout << std::endl;

	// write out text on top
	for(std::size_t i = 0; i < size; ++i) {
		std::cout << tokens[i].value;
		for(std::size_t j = 1; j < fence_length; ++j) {
			std::cout << " ";
		}
	}
	std::cout << std::endl;

	for(std::size_t i = 0; i < length; ++i) {
		if(i % fence_length == 0) {
			std::cout << "|";
		}
		else {
			std::cout << "-";
		}
	}
	std::cout << std::endl;;

	std::vector<std::size_t> rule_nums;
	std::vector<char> line_chars(length, ' ');
	std::size_t index = 0;

	for(std::size_t i = 0; i < tree.size(); ++i) {
		SubParse what = tree[i];

		std::size_t start = what.start;
		std::size_t end = what.end;

		// std::cout << "setting " << start << " " << end << std::endl;
		// std::cout << "so go from [" << 4 * start << ", " << 4 * end << "]" << std::endl;

		if(fence_length * start < index) {
			// first print
			for(std::size_t j = 0; j < line_chars.size(); ++j) {
				std::cout << line_chars[j];
			}

			for(std::size_t j = 0; j < rule_nums.size(); ++j) {
				std::cout << " " << rule_nums[j];
			}
			std::cout << std::endl;

			line_chars.clear();
			rule_nums.clear();
			for(std::size_t j = 0; j < length; ++j) {
				line_chars.push_back(' ');
			}
			index = 0;
		}

		for(std::size_t j = fence_length * start; j <= fence_length * end; ++j) {
			line_chars[j] = '-';
		}

		line_chars[fence_length * start] = '|';
		line_chars[fence_length * end] = '|';

		rule_nums.push_back(what.rule);

		index = fence_length * end;
	}

	// if not printed
	if(index != 0) {
		for(std::size_t j = 0; j < line_chars.size(); ++j) {
			if(line_chars[j]) {
				if(j % fence_length == 0) {
					std::cout << "|";
				}
				else {
					std::cout << "-";
				}
			}
			else {
				std::cout << " ";
			}
		}

		std::cout << std::endl;
	}

	return;
}

void parse_actual_file() {
	std::ifstream input_bnf("TMCompiler/config/language_lexical_grammar.bnf");
	const std::vector<Rule> rules = BnfParser::parse_rules(input_bnf);

	std::cout << "Parsed BNF rules" << std::endl;

	std::vector<Rule> earley_rules = rules;

	std::cout << "generated Rules" << std::endl;

	std::cout << "num rules = " << earley_rules.size() << std::endl;
	for(std::size_t i = 0; i < earley_rules.size(); ++i) {
		const Rule rule = earley_rules[i];
		std::cout << i << ": " << rule.production.value << " -> ";
		for(std::size_t j = 0; j < rule.replacement.size(); ++j) {
			std::cout << rule.replacement[j].value << " ";
		}

		std::cout << std::endl;
	}

	std::cout << std::endl;

	// const std::string input{"int x = 5;"};
	// const std::string input{"for(int i = 5; i < 10; i+=1) { print(i); }"};
	// const std::string input{"int compute(int y) { return 0; }"};

	//                       01234567890123456789012345678901234567890123456789012345
	const std::string input{"void foo() {  for ( int i = 0; i < 10; i += 1) {  }  } "};

	std::vector<Token> tokens;
	for(std::size_t i = 0; i < input.size(); ++i) {
		Token k{"?", input.substr(i, 1), 0, i};
		tokens.push_back(k);
	}

	std::cout << "Generated tokens" << std::endl;

	std::cout << "TOKENS:" << std::endl;
	for(std::size_t i = 0; i < tokens.size(); ++i) {
		std::cout << "Token " << i << ": " << tokens[i].value << std::endl;
	}

	std::string default_start{"tokens"};

	std::vector<std::vector<EarleyItem> > items = build_earley_items(earley_rules, tokens, default_start);

	std::cout << "Finished building earley sets" << std::endl;

	std::cout << "How many Earley sets? " << items.size() << std::endl;

	// for(std::size_t i = 0; i < items.size(); ++i) {
		// std::cout << "Earley Set[" << i << "]" << std::endl;
		// for(std::size_t j = 0; j < items[i].size(); ++j) {
			// EarleyItem item = items[i][j];

			// printItem(earley_rules, item);
		// }

		// std::cout << std::endl;
	// }


	std::cout << "======================================================" << std::endl;

	std::vector<SubParse> tree = build_earley_parse_tree(items, earley_rules, tokens, default_start);

	for(std::size_t i = 0; i < tree.size(); ++i) {
		std::cout << i << ": " << "{ " << tree[i].rule << ", " << tree[i].end << "} " << " starting from " << tree[i].start << " parent= " << tree[i].parent << std::endl; 
	}

	print_tree(tokens, tree);

}

void attempt_parse() {
	std::vector<Rule> grammar_rules = get_grammar_rules();
	std::vector<Token> tokens = get_inputs();
	std::string default_start = "Sum";

	std::vector<std::vector<EarleyItem> > items = build_earley_items(grammar_rules, tokens, default_start);

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

	std::vector<SubParse> tree = build_earley_parse_tree(items, grammar_rules, tokens, default_start);

	for(std::size_t i = 0; i < tree.size(); ++i) {
		std::cout << i << ": " << "{ " << tree[i].rule << ", " << tree[i].end << "} " << " starting from " << tree[i].start << " parent= " << tree[i].parent << std::endl; 
	}

	std::cout << "TOKENS:" << std::endl;
	for(std::size_t i = 0; i < tokens.size(); ++i) {
		std::cout << tokens[i].value;
	}
	std::cout << std::endl;
	for(std::size_t i = 0; i < tokens.size(); ++i) {
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

void trial() {
	std::ifstream lexical_bnf("TMCompiler/config/language_lexical_grammar.bnf");
	std::ifstream syntax_bnf("TMCompiler/config/language_grammar.bnf");

	Compiler compiler(lexical_bnf, syntax_bnf);

	std::string program_text{"?"};
	program_text = "void foo() {}  void main() { foo(); }";

	compiler.compile_text(program_text);
	// compiler.compile("sample_program.cpp");
}

int main() {
	logger.set_level("DEBUG");

	LOG("INFO") << "BEGIN" << std::endl;
	// attempt_parse();
	// parse_actual_file();
	trial();
	LOG("INFO") << "DONE" << std::endl;

	return 0;
}
