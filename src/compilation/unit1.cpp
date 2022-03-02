/*
Convert C++-like source-code into my own assembly language
Function names must be unique
Variable names must be distinct from function names
No (user) global variables allowed
Global infinite int[] array called MEM: 0-indexed
No ++ or -- operators
only allowed types are int and bool
*/

#include "unit1.h"

#include <algorithm>		// std::find_if, std::reverse
#include <cctype>			// std::isspace
#include <cstdlib>			// std::system
#include <fstream>			// std::ifstream
#include <iomanip>			// std::setw
#include <iostream>			// std::cerr, std::cout, std::endl
#include <map>				// std::map
#include <set>				// std::set
#include <string>			// std::string, std::getline, std::to_string, std::insert, std::replace
#include <tuple>			// std::tuple, std::make_tuple, std::get<>
#include <typeinfo>			// typeid
#include <unordered_map>	// std::unordered_map
#include <unordered_set>	// std::unordered_set
#include <map>				// std::map
#include <vector>			// std::vector

int tempCount;				// counter for number of temp variables used

// HELPER FUNCTIONS

// next 3 functions found on : https://stackoverflow.com/questions/216823/how-to-trim-a-stdstring
// trim from start (in place)
static inline void ltrim(std::string &s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
				return !std::isspace(ch);
				}));
}

// trim from end (in place)
static inline void rtrim(std::string &s) {
	s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
				return !std::isspace(ch);
				}).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(std::string &s) {
	ltrim(s);
	rtrim(s);
}

bool isBlankString(const std::string &s) {
	if(s.size() == 0) {
		return true;
	}

	for(size_t i = 0; i < s.size(); ++i) {
		if(!std::isspace(s[i])) {
			return false;
		}
	}

	return true;
}

bool isIntegerLiteral(const std::string &s) {
	if(isBlankString(s)) {
		return false;
	}
	
	if(s[0] == '-') {
		if(s.size() == 1) {
			return false;
		}
		for(int i = 1; i < s.size(); ++i) {
			if(s[i] < '0' || '9' < s[i]) {
				return false;
			}
		}
		
		return true;
	}
	
	for(int i = 0; i < s.size(); ++i) {
		if(s[i] < '0' || '9' < s[i]) {
			return false;
		}
	}
	
	return true;
}

std::string getAlphaNumericWord(const std::string &s, int index) {
	int left = index;
	while(0 <= left && (('0' <= s[left] && s[left] <= '9') || ('a' <= s[left] && s[left] <= 'z') || ('A' <= s[left] && s[left] <= 'Z'))) {
		--left;
	}
	
	int right = index;
	while(right < s.size() && (('0' <= s[right] && s[right] <= '9') || ('a' <= s[right] && s[right] <= 'z') || ('A' <= s[right] && s[right] <= 'Z'))) {
		++right;
	}
	
	return s.substr(1 + left, right - left - 1);
}

std::string getNonBlankWord(const std::string &s, int index) {
	int left = index;
	while(0 <= left && !std::isspace(s[left])) {
		--left;
	}
		
	int right = index;
	while(right < s.size() && !std::isspace(s[right])) {
		++right;
	}
	
	return s.substr(1 + left, right - left - 1);
}

std::string formSubstring(std::vector<char> &s, int start, int end) {
	std::string ans = "";
	for(int i = start; i < end; ++i) {
		ans.push_back(s[i]);
	}

	return ans;
}

int findNext(std::string &s, char c, int start = 0) {
	int index = start;
	while(index < s.size() && s[index] != c) {
		++index;	
	}

	if(index == s.size()) {
		return -1;
	}

	return index;
}

int findNext(std::vector<char> &s, char c, int start = 0) {
	std::string s2 = formSubstring(s, 0, s.size());
	return findNext(s2, c, start);
}

int findNext(std::vector<std::string> &words, const std::string &word, int start = 0) {
	int index = start;
	while(index < words.size() && words[index] != word) {
		++index;
	}
	
	if(index == words.size()) {
		return -1;
	}
	
	return index;
}

int findPrev(std::string &s, char c, int start = 0) {
	int index = start;
	while(index >= 0 && s[index] != c) {
		--index;
	}

	// i know i can just "return index", but just to make it clear
	if(index == -1) {
		return -1;	
	}

	return index;
}

int findPrev(std::vector<char> &s, char c, int start = 0) {
	std::string s2 = formSubstring(s, 0, s.size());
	return findPrev(s2, c, start);
}

int findNonBlank(std::vector<char> &s, int start = 0) {
	int index = start;
	while(index < s.size() && std::isspace(s[index])) {
		++index;
	}

	if(index == s.size()) {
		return -1;
	}

	return index;
}

int findNonBlank(std::string &s, int start = 0) {
	std::vector<char> v(s.begin(), s.end());
	return findNonBlank(v, start);
}

int findBlank(std::string &s, int start = 0) {
	int index = start;
	while(index < s.size() && !std::isspace(s[index])) {
		++index;
	}
	
	if(index == s.size()) {
		index = -1;
	}
	
	return index;
}

int findOpposite(std::vector<char> &s, int index) {
	char c = s[index];
	char op = c;
	bool findBack = false;

	if(c == '(') {
		op = ')';
	}
	else if(c == ')') {
		op = '(';
		findBack = true;
	}
	else if(c == '[') {
		op = ']';
	}
	else if(c == ']') {
		op = '[';
		findBack = true;
	}
	else if(c == '{') {
		op = '}';
	}
	else if(c == '}') {
		op = '{';
		findBack = true;
	}

	int stack = 1;
	if(findBack) {
		for(int i = index - 1; i >= 0; --i) {
			if(s[i] == c) {
				++stack;
			}
			else if(s[i] == op) {
				--stack;
				if(stack == 0) {
					return i;
				}
			}
		}
	}
	else {
		for(int i = index + 1; i < (int) s.size(); ++i) {
			if(s[i] == c) {
				++stack;
			}
			else if(s[i] == op) {
				--stack;
				if(stack == 0) {
					return i;
				}
			}
		}
	}

	return -1;
}

int findOpposite(std::string &s, int index) {
	std::vector<char> v(s.begin(), s.end());
	return findOpposite(v, index);
}

int findOpposite(std::vector<std::string> &words, int index) {
	std::string word = words[index];
	std::string op = word;
	bool findBack = false;

	if(word == "(") {
		op = ")";
	}
	else if(word == ")") {
		op = "(" ;
		findBack = true;
	}
	else if(word == "[") {
		op = "]";
	}
	else if(word == "]") {
		op = "["; 
		findBack = true;
	}
	else if(word == "{") {
		op = "}";
	}
	else if(word == "}") {
		op = "{"; 
		findBack = true;
	}

	int stack = 1;
	if(findBack) {
		for(int i = index - 1; i >= 0; --i) {
			if(words[i] == word) {
				++stack;
			}
			else if(words[i] == op) {
				--stack;
				if(stack == 0) {
					return i;
				}
			}
		}
	}
	else {
		for(int i = index + 1; i < (int) words.size(); ++i) {
			if(words[i] == word) {
				++stack;
			}
			else if(words[i] == op) {
				--stack;
				if(stack == 0) {
					return i;
				}
			}
		}
	}

	return -1;
}

int vimB(std::string &s, int index) {
	if(s.size() == 0) {
		return -1;
	}
	
	if(index == 0) {
		return -1;
	}
	
	--index;
	while(index >= 1 && !(std::isspace(s[index - 1]) && !std::isspace(s[index]))) {
		--index;	
	}
	
	if(index < 1) {
		return -1;
	}
	
	return index;
}

void addIndents(std::string &s, int numIndent) {
	for(int i = 0; i < numIndent; ++i) {
		s.push_back('\t');
	}
}

void printProgram(std::vector<std::string> &program, bool showLines = true) {
	if(program.size() == 0) {
		std::cout << "Program is empty" << std::endl;
		return;
	}

	size_t numDigits = std::to_string(program.size() - 1).size();
	for(size_t i = 0; i < program.size(); ++i) {
		if(showLines) {
			std::cout << std::setw(numDigits) << i << ": ";
		}
		std::cout << program[i] << std::endl;
	}
}

void printPrettyProgram(std::vector<std::string> &program, bool showLines = true) {
	if(program.size() == 0) {
		std::cout << "Program is empty" << std::endl;
	}

	size_t numDigits = std::to_string(program.size() - 1).size();
	for(size_t i = 0; i < program.size(); ++i) {
		if(showLines) {
			std::cout << std::setw(numDigits) << i << ": ";
		}
		
		std::string line = program[i];
		
		size_t start = 0;
		while(start < line.size()) {
			if(line.substr(start, 10) == "!VAR_USER_") {
				start += 10;
				continue;
			}
			else if(line.substr(start, 10) == "!VAR_TEMP_") {
				start += 10;
				continue;
			}
			else if(line.substr(start, 11) == "!FUNC_USER_") {
				start += 11;
				continue;
			}
			else if(line.substr(start, 10) == "!FUNC_LIB_") {
				start += 10;
				continue;
			}
			else if(line.substr(start, 9) == "!VAR_LIB_") {
				start += 9;
				continue;
			}

			std::cout << line[start]; 
			++start;
		}

		std::cout << std::endl;
	}
}

std::string getLetters(std::vector<std::string> &program) {
	std::string ans;
	for(size_t i = 0; i < program.size(); ++i) {
		for(size_t j = 0; j < program[i].size(); ++j) {
			ans.push_back(program[i][j]);
		}
		ans.push_back('\n');
	}
	
	return ans;
}

std::vector<std::string> getWords(std::string &letters)  {
	std::vector<std::string> words;
	
	int startSearch = findNonBlank(letters, 0);
	while(startSearch < letters.size()) {
		std::string word = getNonBlankWord(letters, startSearch);
		words.push_back(word);

		startSearch = findBlank(letters, startSearch);
		if(startSearch < 0 || startSearch >= letters.size()) {
			break;
		}
		startSearch = findNonBlank(letters, startSearch);
	}
 
	return words;
}

// END HELPER FUNCTIONS

int checkCompilation(std::string &fileName) {
	int periodIndex = findPrev(fileName, '.', fileName.size() - 1);

	std::string prefix = "";

	if(periodIndex == -1) {
		prefix = fileName;
	}
	else {
		prefix = fileName.substr(0, periodIndex);
	}

	std::string tempFileName = prefix + "_temp.cpp";
	
	/*
	std::string command = std::string("cp ") + fileName + " " + tempFileName;
	std::cout << "Execute: " << command << std::endl;
	std::system(command.c_str());
	*/

	std::ofstream file;
	file.open(tempFileName);
	file << "int MEM[5];" << std::endl;
	file << "int nextInt();" << std::endl;
	file << "void printInt(int x);" << std::endl;
	file << "void printSpace();" << std::endl;

	std::ifstream origFile;
	origFile.open(fileName);
	std::string line;
	while(std::getline(origFile, line)) {
		file << line << std::endl;
	}

	origFile.close();
	file.close();

	std::string command = std::string("g++ -fsyntax-only ") + tempFileName;
	std::cout << "Execute: " << command << std::endl;
	int status = std::system(command.c_str());

	return status;
}

/**
 * Remove all user comments: line comments of the form //
 * or block comments (potentially multiple lines) of the form / * ... * /
 *
 */
std::vector<std::string> removeComments(std::vector<std::string> &program) {
	std::vector<std::string> ans;
	
	std::vector<char> letters;
	for(size_t i = 0; i < program.size(); ++i) {
		std::string line = program[i];
		for(size_t j = 0; j < line.size(); ++j) {
			letters.push_back(line[j]);
		}

		letters.push_back('\n');
	}

	std::vector<bool> isComment(letters.size(), false);

	size_t numLines = (letters.size() > 0 && letters[0] == '\n') ? 1 : 0;
	size_t start = 1;
	while(start < letters.size()) {
		if(letters[start - 1] == '/') {
			// line comment
			if(letters[start] == '/') {
				isComment[start - 1] = isComment[start] = true;
				// find end of line
				int end = start;
				while(end < letters.size() && letters[end] != '\n') {
					if(letters[end] == '\n') {
						++numLines;
					}
					isComment[end] = true;
					++end;
				}

				start = end + 1;
			}
			/* block comment */
			else if(letters[start] == '*') {
				isComment[start - 1] = isComment[start] = true;
				// find end of block comment
				int end = start + 2;
				while(end < letters.size() && (letters[end - 1] != '*' || letters[end] != '/')) {
					if(letters[end] == '\n') {
						++numLines;
					}
					isComment[end] = true;
					++end;
				}

				if(end == letters.size()) {
					// error
					std::cerr << "Error: Could not find end of block comment start at line " << numLines << std::endl;
					return ans;
				}

				isComment[end] = true;
				start = end + 1;
			}
		}
		else if(letters[start] == '\n') {
			++numLines;
		}	

		++start;
	}

	std::string line = "";
	for(size_t i = 0; i < letters.size(); ++i) {
		char c = letters[i];
		if(!isComment[i]) {
			if(c == '\n') {
				if(line.size() > 0 && !isBlankString(line)) {
					ans.push_back(line);
				}

				line = "";
			}
			else {
				line.push_back(c);
			}
		}
	}

	if(line.size() > 0) {
		ans.push_back(line);
	}	

	return ans;
}

/**
 * Add spaces around any operators/separators, like =,*, +=, (, {, }, ...
 *
 */
std::vector<std::string> addSpaceAroundOperators(std::vector<std::string> &program) {
	// all assignments must have spaces around the equal sign: "x = 5"
	
	std::vector<std::string> ans;

	// ";" and "{" and "}" get slightly different rules: need new line after, not space
	std::set<std::string> oper1 {"+", "-", "*", "/", "%", "!", "=", "(", ")", ",", "^", ">", "<", "[", "]", ";", "{", "}"};
	std::set<std::string> oper2 {"+=", "-=", "*=", "/=", "%=", "!=", "|=", "&=", "^=", "&&", "||", "==", "<=", ">="};

	std::string letters;
	for(size_t i = 0; i < program.size(); ++i) {
		// deliberately don't add in newlines
		for(size_t j = 0; j < program[i].size(); ++j) {
			char c = program[i][j];
			if(c != '\n') {
				letters.push_back(program[i][j]);
			}


			// add spaces around operators
			if(letters.size() >= 2) {
				std::string op = "";
				op.push_back(letters[letters.size() - 2]);
				op.push_back(letters[letters.size() - 1]);
				
				if(oper2.find(op) != oper2.end()) {
					letters.pop_back();
					letters.pop_back();
					letters.push_back(' ');
					letters.append(op);
					letters.push_back(' ');
				}
			}

			if(letters.size() >= 1) {
				std::string op = "";
				op.push_back(letters[letters.size() - 1]);

				if(oper1.find(op) != oper1.end()) {
					// TODO
					if(false && (op == ";" || op == "{" || op == "}")) {
						letters.pop_back();
						letters.push_back(' ');
						letters.append(op);
						letters.push_back('\n');
					}
					else {
						if(letters.size() >= 3 && letters[letters.size() - 2] == ' ') {
							std::string op2 = "";
							op2.push_back(letters[letters.size() - 3]);
							op2.push_back(letters[letters.size() - 1]);
							
							if(oper2.find(op2) != oper2.end()) {
								// oops, accidentally split a 2-char operator
								// what about 3-char operators?? so far illegal but still...
								char relevant = letters[letters.size() - 1];
								letters.pop_back();
								letters.pop_back();
								letters.push_back(relevant);
								letters.push_back(' ');
								continue;
							}
						}

						letters.pop_back();
						letters.push_back(' ');
						letters.append(op);
						letters.push_back(' ');
					}
				}
			}
		}
	}
	
	std::string line = "";
	for(char c : letters) {
		if(c == '\n') {
			ans.push_back(line);
			line = "";
		}
		else {
			line.push_back(c);
		}
	}
	
	if(line.size() > 0) {
		ans.push_back(line);
	}

	return ans;
}

/**
 * Add back in correct indentation and newlines
 */
std::vector<std::string> formatProgram(std::vector<std::string> &program) {
	// all for loops (even single line ones) need {...}	
	
	std::vector<std::string> ans;
	
	std::string letters;
	for(size_t i = 0; i < program.size(); ++i) {
		for(size_t j = 0; j < program[i].size(); ++j) {
			letters.push_back(program[i][j]);
		}
	}

	std::vector<std::string> words;
	int startSearch = findNonBlank(letters, 0);
	
	while(0 <= startSearch && startSearch < letters.size()) {
		std::string word = getNonBlankWord(letters, startSearch);
		words.push_back(word);

		startSearch = findBlank(letters, startSearch);

		if(startSearch == -1 || startSearch == letters.size()) {
			break;
		}

		startSearch = findNonBlank(letters, startSearch);
	}

	std::string letters2;
	
	// kinds of single-lines:
	// 1) declaration: int x = ...;  , with potentially function calls. only int / bool allowed
	// 2) assignment: x =  ...;
	// 3) special assignment: x += ...;, -=, *=, /=, %= (for ints), and &=, |=, ^= (for bools)  . Notable, no shifts, or bit-wise operations
	// 4) function call: my_func(3, 2);
	// 5) function declaration: func int my_func(int x, int y) {
	// 6) loops: for(...) {  / while(...) {
	// 7) conditionals: if(...) {,  else if(...) {, else {...
	// 8) just "}"

	int numIndent = 0;
	bool isFor = false;
	bool newLine = true;
	for(std::string word : words) {
		if(newLine) {
			addIndents(letters2, numIndent);
			newLine = false;
		}

		if(word == "}") {
			letters2.pop_back();
			letters2.append("}\n");	
			--numIndent;
			newLine = true;
		}
		else if(word == ";" && !isFor) {
			letters2.append(";\n");
			newLine = true;
		}
		else if(word == "{") {
			letters2.append("{\n");
			++numIndent;
			newLine = true;
			isFor = false;
		}
		else {
			letters2.append(word);
			letters2.push_back(' ');

			if(word == "for") {
				isFor = true;
			}
		}
	}


	/*
	int startSearch = findNonBlank(letters, 0);
	while(startSearch < (int) letters.size()) {

		if(std::isblank(letters[startSearch])) {
			startSearch = findNonBlank(letters, startSearch);
			continue;
		}


		if(letters[startSearch] == '}') {
			--numIndent;

			addIndents(letters2, numIndent);
			letters2.push_back('}');
			letters2.push_back('\n');

			++startSearch;
			continue;
		}

		// remaining: 1,2,3,4,5,6,7
		// letter must be alpha-numerical

		std::string w = getAlphaNumericWord(letters, startSearch);

		if(w == "if") {
			startSearch = findNext(letters, '(', startSearch);
			
			int endParen = findOpposite(letters, startSearch);

			addIndents(letters2, numIndent);
			letters2.append("if(");
			for(int j = startSearch + 1; j < endParen; ++j) {
				char c = letters[j];
				if(!std::isblank(c) && c != '\n') {
					letters2.push_back(c);
				}
			}
			letters2.append(") {\n");
			++numIndent;

			startSearch = findNext(letters, '{', endParen) + 1;
			continue;
		}

		if(w == "else") {

			// 4 cuz "else" is 4 letters long"
			for(int j = startSearch + 4; j < letters.size(); ++j) {
				if(!(std::isblank(letters[j]))) {
					startSearch = j;
					break;
				}
			}

			std::string w2 = getAlphaNumericWord(letters, startSearch);
			if(w2 == "if") {
				startSearch = findNex(letters, '(', startSearch);
				int endParen = findOpposite(letters, startSearch);

				addIndents(letters2, numIndent);
				letters2.append("else if(");
				for(int j = startSearch + 1; j < endParen; ++j) {
					char c = letters[j];
					if(!std::isblank(c) && c != '\n') {
						letters2.push_back(c);
					}
				}
				letters2.append(") {\n");

				startSearch = findNext(letters, '{', endParen + 1);
			}
			else {
				// is "{"
				addIndents(letters2, numIndent);
				letters2.append("else {\n");
				
				startSearch = startSearch + 1;
			}

			++numIndent;
		}


		// remaining: 1,2,3,4,5,6


	}
	*/

	std::string line = "";
	for(char c : letters2) {
		if(c == '\n') {
			ans.push_back(line);
			line = "";
		}
		else {
			line.push_back(c);
		}
	}

	if(line.size() > 0) {
		ans.push_back(line);
	}

	return ans;
}

/**
 * Identify variable names (declarations, as well as where they are used), 
 * and replace them with "!VAR_USER_<orig_variable_name>"
 * Ex: "int i" -> "int !VAR_USER_i"
 * This is done so I can search more easily, to avoid identifying like "int while2" as a while loop
 *
 * Assumption: cannot have a variable name == function name ... difficult to tell the difference
 */
std::vector<std::string> renameUserVariables(std::vector<std::string> &program) {
	std::vector<std::string> ans;
	
	std::unordered_set<std::string> validTypes {"int", "bool", "void"};

	std::unordered_set<std::string> origFuncNames;
	std::unordered_set<std::string> origVarNames;

	std::vector<std::string> words;
	for(size_t i = 0; i < program.size(); ++i) {
		std::string line = program[i];
		
		int index = findNonBlank(line, 0);
		while(0 <= index && index < line.size()) {
			std::string word = getNonBlankWord(line, index);
			words.push_back(word);

			index = findBlank(line, index);
			if(index < 0 || index >= line.size()) {
				break;
			}

			index = findNonBlank(line, index);
		}
	}

	for(int i = 1; i < (int) words.size(); ++i) {
		std::string prev = words[i - 1];
		if(validTypes.find(prev) != validTypes.end()) {
			if(i < words.size() - 1 && words[i + 1] == "(") {
				origFuncNames.insert(words[i]);
			}
			else {
				origVarNames.insert(words[i]);
			}
		}
	}

	std::cout << "origFuncNames: " << std::endl;
	for(std::string w : origFuncNames) {
		std::cout << w << ", ";
	}
	std::cout << "]" << std::endl;
	std::cout << "origVarNames: " << std::endl;
	for(std::string w : origVarNames) {
		std::cout << w << ", ";
	}
	std::cout << "]" << std::endl;

	std::string letters;
	for(size_t i = 0; i < program.size(); ++i) {
		for(size_t j = 0; j < program[i].size(); ++j) {
			letters.push_back(program[i][j]);
		}

		letters.push_back('\n');
	}

	// now replace function call, with prefix
	std::string funcPrefix = "!FUNC_USER_";
	std::string varPrefix = "!VAR_USER_";

	int startSearch = letters.size() - 1;
	for(int startSearch = letters.size() - 1; startSearch >= 0; startSearch = vimB(letters, startSearch)) {
		if(std::isblank(letters[startSearch])) {
			continue;
		}
		
		std::string word = getNonBlankWord(letters, startSearch);

		if(origFuncNames.find(word) != origFuncNames.end()) {
			letters.insert(startSearch, funcPrefix);
		}
		else if(origVarNames.find(word) != origVarNames.end()) {
			letters.insert(startSearch, varPrefix);
		}
	}

	std::string line;
	for(char c : letters) {
		if(c == '\n') {
			ans.push_back(line);
			line = "";
		}
		else {
			line.push_back(c);
		}
	}

	if(line.size() > 0) {
		ans.push_back(line);
	}	

	return ans;
}

/**
 * Identify built-in stuff: namely,
 * MEM (memory), nextInt(), printInt(), printSpace()
 */
std::vector<std::string> renameBuiltInVariables(std::vector<std::string> &program) {
	std::string letters;
	for(size_t i = 0; i < program.size(); ++i) {
		for(size_t j = 0; j < program[i].size(); ++j) {
			letters.push_back(program[i][j]);
		}
		letters.push_back('\n');
	}

	std::string varPrefix = "!VAR_LIB_";
	std::string funcPrefix = "!FUNC_LIB_";

	std::vector<std::string> words;
	
	int startSearch = findNonBlank(letters, 0);
	while(startSearch < letters.size()) {
		std::string word = getNonBlankWord(letters, startSearch);
		
		if(word == "MEM") {
			word = varPrefix + word;
		}
		else if(word == "nextInt" || word == "printInt" || word == "printSpace") {
			word = funcPrefix + word;
		}
		
		words.push_back(word);

		startSearch = findBlank(letters, startSearch);
		if(startSearch < 0 || startSearch >= letters.size()) {
			break;
		}
		startSearch = findNonBlank(letters, startSearch);
	}

	std::vector<std::string> ans;
	std::string line;
	for(std::string word : words) {
		line.append(word);
		line.push_back('\n');
	}

	ans.push_back(line);

	return ans;
}

/**
 * Convert for-loops to while-loops. Be careful with "continue".
 */
std::vector<std::string> forToWhile(std::vector<std::string> &program) {
	std::vector<std::string> ans;

	std::string letters;
	for(size_t i = 0; i < program.size(); ++i) {
		std::string line = program[i];
		for(size_t j = 0; j < line.size(); ++j) {
			letters.push_back(line[j]);
		}
		letters.push_back('\n');
	}

	// asume program is formatted already: for loop declaration on a single line in source code
	// assume for loop is standard for loop: no for-each loop

	// set recording each for/while loop: for == 0 / while == 1, index of f of "for" or w of "while", openParen, both semicolons, closeParen, and open/close Braces
	std::set<std::tuple<int, int, int, int, int, int, int, int> > allLoops;
	std::map<int, int> curlyBraces;

	std::vector<int> loopChanges(letters.size(), 0);

	int startSearch = 0;
	int lineNum = 0; 

	while(startSearch < (int) letters.size()) {
		if(letters[startSearch] == '\n') {
			++lineNum;
			++startSearch;
			continue;
		}

		bool isLoop = false;
		bool isWhileLoop = false;
		int startLoop = startSearch;

		if(startSearch >= 2 && letters.substr(startSearch - 2, 3) == "for") {
			isLoop = true;
			isWhileLoop = false;
			startLoop = startSearch - 2;
		}
		else if(startSearch >= 4 && letters.substr(startSearch - 4, 5) == "while") {
			isLoop = true;
			isWhileLoop = true;
			startLoop = startSearch - 4;
		}

		if(isLoop) {
 			int openParen = findNext(letters, '(', startSearch + 1);
			int semicolon1 = findNext(letters, ';', openParen + 1);
			int semicolon2 = findNext(letters, ';', semicolon1 + 1);
			int closeParen = findNext(letters, ')', semicolon2 + 1);
			int openBrace = findNext(letters, '{', closeParen + 1);
			int closeBrace = findOpposite(letters, openBrace);
			
			int nextLineBreak1 = findNext(letters, '\n', startLoop);
			int nextLineBreak2 = findNext(letters, '\n', closeParen);

			if(openParen == -1 || semicolon1 == -1 || semicolon2 == -1 || closeParen == -1 || nextLineBreak1 != nextLineBreak2) {
				//std::cerr << "Error: For loop syntax incorrect. Needs \"for(...;...;...) {...}" << std::endl;
				//return ans;

				// not a for loop: like what if variable name was "'for'est" or sth
				++startSearch;
				continue;
			}

			//std::cout << "yea found " << letters.substr(startSearch - 2, closeParen - startSearch + 2 + 1) << std::endl;
			//std::cout << ": " << letters.substr(openBrace, closeBrace - openBrace + 1) << std::endl;

			allLoops.insert(std::make_tuple(isWhileLoop ? 1 : 0, startLoop, openParen, semicolon1, semicolon2, closeParen, openBrace, closeBrace));
			curlyBraces[openBrace] = closeBrace;

			++loopChanges[openBrace + 1];
			--loopChanges[closeBrace];
		}

		++startSearch;
	}

	// for each for loop, do a surface-level check (no inner loops) through letters to find substring "continue"
	// map from original "continue" index, to <insert (if 0) or replace(if > 0), update-rule>
	std::map<int, std::pair<int, std::string> > loopUpdate;
	for(std::tuple<int, int, int, int, int, int, int, int> loopInfo : allLoops) {
		int loopType  = std::get<0>(loopInfo);

		// if it's a for loop
		if(loopType == 0) {
			
			int loopStart = std::get<1>(loopInfo);
			int openParen = std::get<2>(loopInfo);
			int semicolon1 = std::get<3>(loopInfo);
			int semicolon2 = std::get<4>(loopInfo);
			int closeParen = std::get<5>(loopInfo);
			int openBrace = std::get<6>(loopInfo);
			int closeBrace = std::get<7>(loopInfo);
			
			std::string initialization = letters.substr(openParen + 1, semicolon1 - openParen - 1);
			std::string check = letters.substr(semicolon1 + 1, semicolon2 - semicolon1 - 1);
			std::string update = letters.substr(semicolon2 + 1, closeParen - semicolon2 - 1);

			trim(initialization);
			trim(check);
			trim(update);

			// if empty check (aka for(;;)), then replace check with true
			if(isBlankString(check)) {
				check = "true";
			}

			//std::cout << "init: " << initialization << " check: " << check << " update: " << update << std::endl;
			
			// keep track of indentation
			std::string padding = "";
			// even if on first line: findPrev returns -1, so firstBlankIndex == 0, so no need to error handle
			int firstBlankIndex = findPrev(letters, '\n', loopStart) + 1;
			for(int i = firstBlankIndex; i < loopStart; ++i) {
				padding.push_back(letters[i]);
			}

			// replace all "surface-level" "continue" with "update; continue"
			// surface-level meaning don't bother with changing the "continue" in an inner for loop or while loop
			// (but if statements are ok)	
			for(startSearch = openBrace + 1; startSearch < closeBrace;) {
				// if found another loop (aka an inner loop), skip it
				if(curlyBraces.find(startSearch) != curlyBraces.end()) {
					int innerCloseBrace = curlyBraces[startSearch];
					startSearch = innerCloseBrace + 1;
					continue;
				}

				// "continue" has length 8; also make sure don't find variables like "continue2" or sth
				if(startSearch >= 7 && letters.substr(startSearch - 7, 8) == "continue") {
					// check if update is not blank: could have for-loop like "for(;;) {}"
					if(!isBlankString(update)) {
						int lineStart = findPrev(letters, '\n', startSearch - 7);
						std::string currentLine = letters.substr(lineStart, startSearch + 1 - lineStart + 1);
						trim(currentLine);
						if(currentLine == "continue") {
							std::string padding2 = letters.substr(lineStart, startSearch - 7 - lineStart);
							loopUpdate[lineStart] = std::make_pair(0, padding2 + update + " ;");
						}
					}
				}

				++startSearch;
			}
	
			// before for loop, insert initialization: 
			// before: "for(<init>;<check>;<update>) {...}
			// after:  "<init>;\n while(check) {... <update>;}
			
			// replace for with while
			loopUpdate[firstBlankIndex] = std::make_pair(closeParen + 1 - firstBlankIndex, padding + initialization + " ;\n" + padding + "while ( " + check + " ) ");

			// before the last '}', insert in <update>
			if(!isBlankString(update)) {
				int lineStart = findPrev(letters, '\n', closeBrace);
				std::string padding2 = letters.substr(lineStart, closeBrace - lineStart);
				loopUpdate[lineStart] = std::make_pair(0, padding2 + "\t" + update + " ;");
			}
		}
	}

	// do all replacements
	// traverse in reverse so don't have to deal with problem of indices shifting up
	for(std::map<int, std::pair<int, std::string> >::reverse_iterator it = loopUpdate.rbegin(); it != loopUpdate.rend(); ++it) {
		
		int index = it->first;
		std::pair<int, std::string> temp = it->second;
		int len = temp.first;
		std::string text = temp.second;
		
		if(len > 0) {
			letters.replace(index, len, text);			
		}
		else {
			letters.insert(index, text);
		}
		
	}

	// turn letters,back into vector<string>
	std::string line;
	for(size_t i = 0; i < letters.size(); ++i) {
		char c = letters[i];
		if(c == '\n') {
			ans.push_back(line);
			line = "";
		}
		else {
			line.push_back(c);
		}
	}

	if(line.size() > 0) {
		ans.push_back(line);
	}

	return ans;
}

/**
 * Convert else-if statements to just if + else statements
 */
std::vector<std::string> convertElif(std::vector<std::string> &program) {
	std::vector<std::string> ans;

	std::string letters;
	for(size_t i = 0; i < program.size(); ++i) {
		for(size_t j = 0; j < program[i].size(); ++j) {
			letters.push_back(program[i][j]);
		}
		letters.push_back('\n');
	}
	
	std::vector<std::string> words;

	int startSearch = findNonBlank(letters, 0);
	while(startSearch < letters.size()) {
		std::string word = getNonBlankWord(letters, startSearch);
		words.push_back(word);

		startSearch = findBlank(letters, startSearch);
		startSearch = findNonBlank(letters, startSearch);	
	}
	
	std::vector<std::string> words2;
	
	bool hasChanged = true;

	while(hasChanged) {
		hasChanged = false;

		int startSearch = 0;
		while(startSearch < words.size()) {	
			std::string word = words[startSearch];
			if(word != "else") {
				words2.push_back(word);
				++startSearch;
				continue;
			}

			int numClose = 0;

			while(words[startSearch] == "else") {
				bool b = (startSearch + 1 < words.size() && words[startSearch + 1] == "if");

				int openBrace = findNext(words, "{", startSearch);
				int closeBrace = findOpposite(words, openBrace);

				words2.push_back("else");
				
				if(b) {
					words2.push_back("{");		// now "else {" instead of "else if"
					++numClose;
					hasChanged = true;
				}

				// now put in everything from "if(...) {...}"

				++startSearch;			// now move from "else" to "if" (if "else if"), otherwise move from "else" to "{"
				while(startSearch <= closeBrace) {
					words2.push_back(words[startSearch]);
					++startSearch;
				}
			}
			
			for(int i = 0; i < numClose; ++i) {
				words2.push_back("}");
			}
		}

		words = words2;
		words2.clear();
	}
	
	for(size_t i = 0; i < words.size(); ++i) {
		std::string word = words[i];
		ans.push_back(word);
		if(i + 1 < words.size()) {
			ans.push_back(" ");
		}
	}
	
	return ans;
}

/**
 * Convert while(<expression>) to 
 * while(true) {
 * 	  if(!(expression)) {
 *		break;
 */
std::vector<std::string> changeWhileLoops(std::vector<std::string> &program) {
	std::vector<std::vector<std::string> > allWords;
	for(size_t i = 0; i < program.size(); ++i) {
		allWords.push_back(getWords(program[i]));
	}
	
	for(int i = (int) program.size() - 1; i >= 0; --i) {
		std::vector<std::string> words = allWords[i];
		if(words[0] == "while") {
			int openParen = findNext(words, "(", 0);
			int closeParen = findOpposite(words, openParen);
			
			std::vector<std::string> expression(words.begin() + openParen + 1, words.begin() + closeParen);
			words.erase(words.begin() + openParen + 1, words.begin() + closeParen);
			words.insert(words.begin() + openParen + 1, "true");

			allWords[i] = words;	

			std::vector<std::string> temp1 {"if", "(", "!", "("};
			temp1.insert(temp1.end(), expression.begin(), expression.end());
			temp1.push_back(")");
			temp1.push_back(")");
			temp1.push_back("{");
			allWords.insert(allWords.begin() + i + 1, temp1);

			std::vector<std::string> temp2 {"break", ";"};
			allWords.insert(allWords.begin() + i + 2, temp2);

			std::vector<std::string> temp3 {"}"};
			allWords.insert(allWords.begin() + i + 3, temp3);
		}
	}
	
	std::vector<std::string> ans;
	std::string temp;
	for(size_t i = 0; i < allWords.size(); ++i) {
		for(size_t j = 0; j < allWords[i].size(); ++j) {
			temp.append(allWords[i][j]);
			temp.append(" ");
		}
	}
	
	ans.push_back(temp);
	return ans;
}

/**
 * Add return to the end of void functions
 */
std::vector<std::string> voidReturns(std::vector<std::string> &program) {
	std::string letters = getLetters(program);
	std::vector<std::string> words = getWords(letters);

	for(size_t i = 0; i < words.size(); ++i) {
		if(words[i] == "void" && words[i+1].substr(0, 5) == "!FUNC") {
			int openBrace = findNext(words, "{", i);
			int closeBrace = findOpposite(words, openBrace);
				
			if(words[closeBrace - 1] != "return") {
				words.insert(words.begin() + closeBrace, ";");
				words.insert(words.begin() + closeBrace, "return");
			}

			i = closeBrace - 1;
		}
	}
	
	std::vector<std::string> ans;
	std::string line;
	for(std::string word : words) {
		line.append(word);
		line.push_back(' ');
	}
	ans.push_back(line);
	
	return ans;
}

/**
 * Move declarations to the top of the function:
 * change int x = 5;   ->   int x;  x = 5;
 */
std::vector<std::string> moveUpDeclaration(std::vector<std::string> &program) {
	std::vector<std::vector<std::string> > allWords;

	std::vector<int> funcLines;
	for(size_t i = 0; i < program.size(); ++i) {
		std::string line = program[i];
		std::vector<std::string> words = getWords(line);
	
		allWords.push_back(words);

		std::unordered_set<std::string> validTypes {"void", "int", "bool"};
		if(validTypes.find(words[0]) != validTypes.end() && words[1].substr(0, 5) == "!FUNC") {
			// function declaration found
			funcLines.push_back((int) i);
		}
	}

	// find variable declarations: "int !VAR = "
	// as well as "int x;"
	// no multiple declarations in a single line: no "int x, y;"
	
	// map from function declaration line (which function variable lives in), to all declared variables
	std::map<int, std::vector<std::string> > intVars;
	std::map<int, std::vector<std::string> > boolVars;

	for(int i = (int) program.size() - 1; i >= 0; --i) {
		std::vector<std::string> words = allWords[i]; 

		std::unordered_map<std::string, std::string> validTypes {{"int", "0"}, {"bool", "false"}};
		if(validTypes.find(words[0]) != validTypes.end() && words[1].substr(0, 4) == "!VAR") {
			int funcLine = funcLines[funcLines.size() - 1];
			if(funcLine > i) {
				funcLines.pop_back();
				funcLine = funcLines[funcLines.size() - 1];
			}
			
			std::string typeName = words[0];
			std::string varName = words[1];
			if(words[2] == ";") {
				words.clear();
				words.push_back(varName);
				words.push_back("=");
				words.push_back(validTypes[typeName]);

			}
			else {
				words.erase(words.begin());
			}

			allWords[i] = words;

			if(typeName == "int") {
				intVars[funcLine].push_back(varName);
			}
			else {
				boolVars[funcLine].push_back(varName);
			}
		}
	}
	std::map<int, std::vector<std::string> >::reverse_iterator it1 = intVars.rbegin();
	std::map<int, std::vector<std::string> >::reverse_iterator it2 = boolVars.rbegin();

	while(it1 != intVars.rend() && it2 != boolVars.rend()) {
		if(it1->first >= it2->first) {
			std::vector<std::string> varNames = it1->second;
			std::unordered_set<std::string> used;
			for(std::string varName : varNames) {
				// can only initialize once: if use for(int i = 0)... multiple times...
				if(used.find(varName) == used.end()) {
					used.insert(varName);
					std::vector<std::string> temp {"int", varName, ";"};
					allWords.insert(allWords.begin() + it1->first + 1, temp);
				}
			}
			++it1;
		}
		else {
			std::vector<std::string> varNames = it2->second;
			std::unordered_set<std::string> used;
			for(std::string varName : varNames) {
				std::vector<std::string> temp {"bool", varName, ";"};
				allWords.insert(allWords.begin() + it2->first + 1, temp);
			}
			++it2;
		}
	}
	
	while(it1 != intVars.rend()) {
		std::vector<std::string> varNames = it1->second;
		for(std::string varName : varNames) {
			std::vector<std::string> temp {"int", varName, ";"};
			allWords.insert(allWords.begin() + it1->first + 1, temp);
		}
		++it1;
	}
		
	while(it2 != boolVars.rend()) {
		std::vector<std::string> varNames = it2->second;
		for(std::string varName : varNames) {
			std::vector<std::string> temp {"int", varName, ";"};
			allWords.insert(allWords.begin() + it2->first + 1, temp);
		}
		++it2;
	}

	std::vector<std::string> ans;
	std::string temp;
	for(size_t i = 0; i < allWords.size(); ++i) {
		for(size_t j = 0; j < allWords[i].size(); ++j) {
			temp.append(allWords[i][j]);
			temp.append(" ");
		}
	}
		
	ans.push_back(temp);
	return ans;
}

/**
 * Given formatted program and renamed variables, get all functions in a vector of:
 * name of function, then list of parameters, then return type
 */
std::vector<std::tuple<std::string, std::vector<std::string>, std::string> > getFunctions(std::vector<std::string> &program) {
	// get all functions in program: function consists of <name, parameters, return type>
	std::vector<std::tuple<std::string, std::vector<std::string>, std::string> > funcs;
	
	funcs.push_back(std::make_tuple("!", std::vector<std::string>{"bool"}, "bool"));
	funcs.push_back(std::make_tuple("u-", std::vector<std::string>{"int"}, "int"));				// unary minus
	funcs.push_back(std::make_tuple("+", std::vector<std::string>{"int", "int"}, "int"));
	funcs.push_back(std::make_tuple("-", std::vector<std::string>{"int", "int"}, "int"));
	funcs.push_back(std::make_tuple("*", std::vector<std::string>{"int", "int"}, "int"));
	funcs.push_back(std::make_tuple("/", std::vector<std::string>{"int", "int"}, "int"));
	funcs.push_back(std::make_tuple("%", std::vector<std::string>{"int", "int"}, "int"));
	funcs.push_back(std::make_tuple("<", std::vector<std::string>{"int", "int"}, "bool"));
	funcs.push_back(std::make_tuple(">", std::vector<std::string>{"int", "int"}, "bool"));
	funcs.push_back(std::make_tuple("<=", std::vector<std::string>{"int", "int"}, "bool"));
	funcs.push_back(std::make_tuple(">=", std::vector<std::string>{"int", "int"}, "bool"));
	funcs.push_back(std::make_tuple("==", std::vector<std::string>{"bool", "bool"}, "bool")); //  what about the == for ints?
	funcs.push_back(std::make_tuple("!=", std::vector<std::string>{"bool", "bool"}, "bool")); // is technically overloaded for bools too
	funcs.push_back(std::make_tuple("&&", std::vector<std::string>{"bool", "bool"}, "bool"));
	funcs.push_back(std::make_tuple("||", std::vector<std::string>{"bool", "bool"}, "bool"));
	funcs.push_back(std::make_tuple("^", std::vector<std::string>{"bool", "bool"}, "bool"));
	funcs.push_back(std::make_tuple("=", std::vector<std::string>{"int", "int"}, "int"));		// and for bool's too
	funcs.push_back(std::make_tuple("+=", std::vector<std::string>{"int", "int"}, "int"));
	funcs.push_back(std::make_tuple("-=", std::vector<std::string>{"int", "int"}, "int"));
	funcs.push_back(std::make_tuple("*=", std::vector<std::string>{"int", "int"}, "int"));
	funcs.push_back(std::make_tuple("/=", std::vector<std::string>{"int", "int"}, "int"));
	funcs.push_back(std::make_tuple("%=", std::vector<std::string>{"int", "int"}, "int"));
	funcs.push_back(std::make_tuple("&=", std::vector<std::string>{"bool", "bool"}, "bool"));
	funcs.push_back(std::make_tuple("|=", std::vector<std::string>{"bool", "bool"}, "bool"));
	funcs.push_back(std::make_tuple("^=", std::vector<std::string>{"bool", "bool"}, "bool"));
	
	funcs.push_back(std::make_tuple("!FUNC_LIB_printInt", std::vector<std::string> {"int"}, "void"));
	funcs.push_back(std::make_tuple("!FUNC_LIB_printSpace", std::vector<std::string>(), "void"));
	funcs.push_back(std::make_tuple("!FUNC_LIB_nextInt", std::vector<std::string>(), "int"));

	std::unordered_set<std::string> validTypes {"void", "int", "bool"};

	for(size_t i = 0; i < program.size(); ++i) {
		std::vector<std::string> words = getWords(program[i]);
		if(validTypes.find(words[0]) != validTypes.end() && words[1].substr(0, 5) == "!FUNC") {
			// function declaration
			std::string name = words[1];
			std::string retType = words[0];
			std::vector<std::string> params;
			int openParen = findNext(words, "(", 1);
			int closeParen = findOpposite(words, openParen);
			for(size_t j = openParen + 1; j < closeParen; ++j) {
				if(validTypes.find(words[j]) != validTypes.end()) {
					params.push_back(words[j]);
				}
			}

			std::tuple<std::string, std::vector<std::string>, std::string> func = std::make_tuple(name, params, retType);
			funcs.push_back(func);
		}
	}
	
	return funcs;
}

/**
 * Given a statement that is either a) function call , or b) assignment (no special assignment like +=), or while loop / if statement without open curly brace
 * And no literals, like true/false/-35
 * Use Dijkstra's shunting-yard algorithm to convert to post-fix notation
 */
std::vector<std::string> shuntingYard(std::vector<std::string> &words) {
	// operator precedence and left-to-right stuff
	// only thing right-to-left associative is ! (NOT) and = (assignment / special assignment)

	// https://introcs.cs.princeton.edu/java/11precedence/
	std::unordered_map<std::string, int> precedence {{"!", 14}, {"u-", 14}, {"*", 12}, {"/", 12}, {"%", 12}, {"+", 11}, {"-", 11}, {"<", 9}, {"<=", 9}, {">", 9}, {">=", 9}, {"==", 8}, {"!=", 8}, {"^", 6}, {"&&", 4}, {"||", 3}, {"=", 1}, {"+=", 1}, {"-=", 1}, {"*=", 1}, {"/=", 1}, {"%=", 1}, {"&=", 1}, {"|=", 1}, {"^=", 1}};

	std::vector<std::string> output;
	std::vector<std::string> stack;

	for(size_t i = 0; i < words.size(); ++i) {
		std::string word = words[i];
		if(word == "!VAR_LIB_MEM") {
			stack.push_back(word);
		}
		else if(word.substr(0, 4) == "!VAR") {
			output.push_back(word);
			
			// check for unary minus: looked like -number
			if(stack.size() >= 1 && stack[stack.size() - 1] == "u-") {
				output.push_back(stack[stack.size() - 1]);
				stack.pop_back();
			}
		}
		else if(word == ")" || word == "]") {
			std::string oppo = (word == ")") ? "(" : "[";
			while(stack[stack.size() - 1] != oppo) {
				std::string op = stack[stack.size() - 1];
				output.push_back(op);
				stack.pop_back();
			}

			stack.pop_back();		// remove "(" / "["
				
			// check for unary minus: original expression looked like -(number)
			if(stack.size() >= 1 && stack[stack.size() - 1] == "u-") {
				output.push_back(stack[stack.size() - 1]);
				stack.pop_back();
			}

			if(stack.size() > 0 && (stack[stack.size() - 1].substr(0, 5) == "!FUNC" || stack[stack.size() - 1] == "!VAR_LIB_MEM" || stack[stack.size() - 1] == "while" || stack[stack.size() - 1] == "if" || stack[stack.size() - 1] == "return")) {
				std::string func = stack[stack.size() - 1];
				output.push_back(func);
				stack.pop_back();
			}
			
			// check for unary minus again: expression looked like -func(...)
			if(stack.size() >= 1 && stack[stack.size() - 1] == "u-") {
				output.push_back(stack[stack.size() - 1]);
				stack.pop_back();
			}
		}
		else if(precedence.find(word) != precedence.end()) {

			// if minus sign, determine if unary; unable to determine once expression is in postfix: wcipeg.com shunting-yard algo page has example 1 2 - 3 + 
			// stuff inside !(...) 
			if(word == "-" && !(i > 0 && (words[i-1] == "]" || words[i-1] == ")" || words[i-1].substr(0, 4) == "!VAR"))) {
				// must be unary minus
				word = "u-";
			}

			int level = precedence[word];

			while(stack.size() > 0 && precedence.find(stack[stack.size() - 1]) != precedence.end() && ((precedence[stack[stack.size() - 1]] > level) || (precedence[stack[stack.size() - 1]] == level && word != "!" && word != "=" && level != 1))) {
				std::string temp = stack[stack.size() - 1];
				output.push_back(temp);
				stack.pop_back();
			}

			stack.push_back(word);
		}
		else if(word == "[" || word == "(" || word.substr(0, 5) == "!FUNC" || word == "while" || word == "if" || word == "return") {
			stack.push_back(word);
		}

		// leave out commas
	}

	while(!stack.empty()) {
		output.push_back(stack[stack.size() - 1]);
		stack.pop_back();
	}

	return output;
}

/**
 * Helper function to simplifyExpressions(); 
 * takes in one line of source code, and returns a bunch of lines (so now simplified)
 * Is recursive
 * tempCount is incremented whenever new temp variable is created (to avoid duplicate declarations)
 */
std::vector<std::string> simplifyLine(std::string &line, std::vector<std::tuple<std::string, std::vector<std::string>, std::string> > funcs) {
	std::vector<std::string> ans;
	std::vector<std::string> words = getWords(line);	

	std::unordered_set<std::string> validTypes {"int", "bool", "void"};
	//std::unordered_map<std::string, int> numParams {{"!FUNC_LIB_nextInt", 0}, {"!FUNC_LIB_printInt", 1}, {"!FUNC_LIB_printSpace", 0}, {"!", 1}, {"if", 1}, {"while", 1}, {"return", 1}, {"*", 2}, {"/", 2}, {"%", 2}, {"+", 2}, {"-", 2}, {"&&", 2}, {"||", 2}, {"^", 2}, {"!=", 2}, {"==", 2}, {"<", 2}, {"<=", 2}, {">", 2}, {">=", 2}, {"=", 2}, {"+=", 2}, {"-=", 2}, {"*=", 2}, {"/=", 2}, {"%=", 2}, {"&=", 2}, {"|=", 2}, {"^=", 2}};
	// the above doesn't account for user-defined functions; wouldn't recognize user-defined function as an operator/function
	
	std::unordered_map<std::string, int> numParams; 
	for(size_t i = 0; i < funcs.size(); ++i) {
		std::tuple<std::string, std::vector<std::string>, std::string> func = funcs[i];
		numParams[std::get<0>(func)] = std::get<1>(func).size();
	}

	// check what kind of line it is
	
	// empty line
	if(words.size() == 0) {
		ans.push_back(line);
		return ans;
	}

	// function declaration?
	if(words.size() >= 3 && validTypes.find(words[0]) != validTypes.end() && words[1].substr(0, 5) == "!FUNC" && words[2] == "(") {
		int numCommas = 0;
		for(size_t i = 0; i < words.size(); ++i) {
			if(words[i] == ",") {
				++numCommas;
			}
		}
		
		std::string func = words[1];
		numParams[func] = 1 + numCommas;

		ans.push_back(line);
		return ans;
	}
	
	if(words[0] == "continue" || words[0] == "break" || words[0] == "else" || words[0] == "}" || words[0] == "while") {
		ans.push_back(line);
		return ans;
	}

	// just a declaration?
	if(words.size() == 3 && validTypes.find(words[0]) != validTypes.end() && words[1].substr(0, 4) == "!VAR" && words[2] == ";") {
		ans.push_back(line);
		return ans;
	}

	//std::vector<std::string> decInt;
	//std::vector<std::string> decBoolean;

	// has declaration?
	/*
	if(words.size() >= 2 && validTypes.find(words[0]) != validTypes.end()) {
		std::string var = words[1];
		if(words[0] == "int") {
			decInt.push_back(var);
		}
		else if(words[0] == "bool") {
			decBoolean.push_back(var);
		}

		words.erase(words.begin()); // remove first word: the declaration
	}
	*/

	// now turn int literals (like -35 and 0) and boolean literals (like true/false) 
	// into their own variables
	for(size_t i = 0; i < words.size(); ++i) {
		std::string word = words[i];
		if(word == "true" || word == "false") {
			std::string var = std::string("!VAR_TEMP_temp") + std::to_string(tempCount);
			//decBoolean.push_back(var);
			words[i] = var;
			
			ans.push_back("bool " + var + " ; ");	
			ans.push_back(var + " " + word + " = ; ");		// have statement like "temp1 = true" prepended
			++tempCount;
		}
		else if(isIntegerLiteral(word)) {
			std::string var = std::string("!VAR_TEMP_temp") + std::to_string(tempCount);
			//decInt.push_back(var);
			words[i] = var;
			
			ans.push_back("int " + var + " ; ");	
			ans.push_back(var + " " + word + " = ; ");		// have statement like "temp2 = 20" prepended
			++tempCount;
		}
	}

	/*
	for(size_t i = 0; i < decInt.size(); ++i) {
		std::string var = decInt[i];
		ans.push_back("int " + var + " ; ");
	}
	
	for(size_t i = 0; i < decBoolean.size(); ++i) {
		std::string var = decBoolean[i];
		ans.push_back("bool " + var + " ; ");
	}
	*/
		
	// special assignment?
	// turn "x += 5 ; " to "x = x + (5) ;"
	
	/*
	// TODO  ??? What. are you doing. += doesn't have to be words[1]
	if(words.size() >= 2) {
		std::string op = words[1];
		std::unordered_map<std::string, std::string> special {{"+=", "+"}, {"-=", "-"}, {"*=", "*"}, {"/=", "/"}, {"%=", "%"}, {"&=", "&&"}, {"|=", "||"}, {"^=", "^"}};
		if(special.find(op) != special.end()) {
			std::string op2 = special[op];
			std::string var = words[0];

			std::vector<std::string> words2;
			words2.push_back(var);
			words2.push_back("=");
			words2.push_back(var);
			words2.push_back(op2);
			words2.push_back("(");
			for(size_t i = 2; i < words.size() - 1; ++i) {
				words2.push_back(words[i]);
			}
			words2.push_back(")");
			words2.push_back(words[words.size() - 1]);
			
			words = words2;
		}
	}
	*/

	// funcion call?
	// regular assignment? EDIT: nah, can have special assignment
	// while loops / if statements...?

	bool hasOpenBrace = words[words.size() - 1] == "{";
	bool hasComma = words[words.size() - 1] == ";";
	if(hasOpenBrace || hasComma) {
		words.pop_back();
	}

	std::vector<std::string> postfix = shuntingYard(words);

	// TODO
	// what to do about hasOpenBrace and hasComma?
	// MEM is incorrect...

	/*
	std::cout << "orig: " << std::endl;
	for(std::string word : words) {
		std::cout << word << " ";
	}
	std::cout << std::endl;

	std::cout << "post: " << std::endl;
	for(std::string word : postfix) {
		std::cout << word << " ";
	}
	std::cout << std::endl << std::endl;
	*/

	int numOp = 0;
	for(size_t i = 0; i < postfix.size(); ++i) {
		std::string word = postfix[i];
		if(numParams.find(word) != numParams.end()) {
			++numOp;
		}
	}

	//std::cout << "numOp = " << numOp << std::endl;
	while(numOp > 1) {
		// find first op from the left
		int op = 0;
		while(numParams.find(postfix[op]) == numParams.end()) {
			++op;
		}
		
		--numOp;

		int numParam = numParams[postfix[op]];

		int countParam = 0;
		int paramIndex = op;
		while(countParam < numParam) {
			--paramIndex;
			// "index MEM" takes up two words, but only one parameter
			if(postfix[paramIndex] == "!VAR_LIB_MEM") {
				--paramIndex;
			}
			++countParam;
		}
		// then the part from [paramIndex, op] is one sub-expression; take out and replace with temp variable
		std::string subexpression;
		for(size_t i = paramIndex; i <= op; ++i) {
			subexpression.append(postfix[i]);
			subexpression.append(" ");
		}
		std::string tempVar = "!VAR_TEMP_temp" + std::to_string(tempCount);
		subexpression.append(" = " + tempVar + " ; ");

		// find return type of the operator
		std::string opName = postfix[op];
		std::string retType = ""; 
		
		for(size_t i = 0; i < funcs.size(); ++i) {
			if(std::get<0>(funcs[i]) == opName) {
				retType = std::get<2>(funcs[i]);
			}
		}

		// now that you found the type of the result, declare the temp variable, then call subexpression
		//std::cout << "found ret type of " << tempVar << " = [" << retType << "]" << " of  op = " << opName << std::endl;
		ans.push_back(retType + " " + tempVar + " ; ");
		ans.push_back(subexpression);
			
		postfix.erase(postfix.begin() + paramIndex, postfix.begin() + op + 1);
		postfix.insert(postfix.begin() + paramIndex, "!VAR_TEMP_temp" + std::to_string(tempCount));
		++tempCount;
	}

	// 1 operator left
	std::string subexpression;
	for(size_t i = 0; i < postfix.size(); ++i) {
		subexpression.append(postfix[i]);
		subexpression.append(" ");
	}
	
	if(hasOpenBrace) {
		subexpression.append(" { ");
	}
	else if(hasComma) {
		subexpression.append(" ; ");
	}
	
	ans.push_back(subexpression);

	return ans;
}

/**
 * Turn complex expressions (like 2 + 3 * 5) into multiple lines of simple expressions (like
 * int temp = 3 * 5;
 * int ans = 2 + temp;
 * This applies to stuff like (int ans = 2 + 3 * 5), boolean conditions for if/while statements (result should always be if(<boolean-name>)), and passing parameters
 * 
 * Order of operations: https://introcs.cs.princeton.edu/java/11precedence/
 *
 * Literals, like "true" and "-35" get their own line: int x = -35 ;
 */
std::vector<std::string> simplifyExpressions(std::vector<std::string> &program) {
	// get all functions in program: function consists of <name, parameters, return type>
	std::vector<std::tuple<std::string, std::vector<std::string>, std::string> > funcs = getFunctions(program);
	
	std::vector<std::string> ans;

	for(size_t i = 0; i < program.size(); ++i) {
		std::string line = program[i];
		std::vector<std::string> lines = simplifyLine(line, funcs);

		//std::cout << "line = " << line << std::endl;
		for(size_t j = 0; j < lines.size(); ++j) {
			ans.push_back(lines[j]);
			//std::cout << ": " << lines[j] << std::endl;
		}
		//std::cout << std::endl;
	}

	std::vector<std::string> ans2;
	std::string temp;
	for(size_t i = 0; i < ans.size(); ++i) {
		std::string line = ans[i];
		std::vector<std::string> words = getWords(line);
		for(size_t j = 0; j < words.size(); ++j) {
			temp.append(words[j]);
			temp.append(" ");
		}
	}
	
	ans2.push_back(temp);
	return ans2;
}

/**
 * Turn special assigment (like postfix "a b +=" -> "int temp; a b + = temp; a temp = ;"
 * Meaning: turn "a += b" to "int temp; temp = a + b; a = temp;"
 * Other possible expression: "a b += = temp1" -> "int temp; a b + = temp; a temp =; temp1 a =;"
 * Expression cannot start with declaration: consider
 *
 * EDIT: what about: MEM[ MEM[2] += 3 ] += 5?
 * Turns to MEM[ MEM[2] += 3 ] = MEM[ MEM[2] += 3] + 5
 * So MEM[2] actually increments by 6...
 * EDIT: I think handled
 */
std::vector<std::string> convertSpecialAssignment(std::vector<std::string> &program) {
	std::vector<std::string> ans;
	std::unordered_map<std::string, std::string> special {{"+=", "+"}, {"-=", "-"}, {"*=", "*"}, {"/=", "/"}, {"%=", "%"}, {"|=", "||"}, {"&=", "&&"}, {"^=", "^"}};

	for(int i = (int) program.size() - 1; i >= 0; --i) {
		std::string line = program[i];
		std::vector<std::string> words = getWords(line);
		
		int desiredOp = -1;
		for(size_t j = 0; j < words.size(); ++j) {
			if(special.find(words[j]) != special.end()) {
				desiredOp = (int) j;
				break;
			}
		}

		if(desiredOp == -1) {
			ans.push_back(line);
		}
		else {
			//ans.push_back(line);
			std::string tempVar = "temp" + std::to_string(tempCount);
			++tempCount;

			std::string desiredType = "int";
			if(words[desiredOp] == "|=" || words[desiredOp] == "&=" || words[desiredOp] == "^=") {
				desiredType = "bool";
			}

			int index = desiredOp - 1;
			std::string B;
			if(words[index] == "!VAR_LIB_MEM") {
				--index;
				B = words[index] + " " + words[index + 1];
			}
			else {
				B = words[index];
			}
				
			std::string A;
			--index;
			if(words[index] == "!VAR_LIB_MEM") {
				--index;
				A = words[index] + " " + words[index + 1];
			}
			else {
				A = words[index];
			}

			std::vector<std::string> tempLines;
			tempLines.push_back(desiredType + " " + tempVar + " ; ");
			tempLines.push_back(A + " " + B + " " + special[words[desiredOp]] + " = " + tempVar + " ; ");
			tempLines.push_back(A + " " + tempVar + " = ; ");

			if(words.size() >= 3 && words[words.size() - 3] == "=") {
				tempLines.push_back(words[words.size() - 2] + " " + A + " = ; ");
			}

			std::reverse(tempLines.begin(), tempLines.end());
			ans.insert(ans.end(), tempLines.begin(), tempLines.end());
		}
	}

	// but now it's reversed, since you processed from last line to first
	//std::reverse(ans.begin(), ans.end());

	std::vector<std::string> ans2;
	std::string line;
	for(int i = (int) ans.size() - 1; i >= 0; --i) {
		std::vector<std::string> words = getWords(ans[i]);
		for(size_t j = 0; j < words.size(); ++j) {
			line.append(words[j]);
			line.append(" ");
		}
	}

	ans2.push_back(line);

	return ans2;
}

/**
 * Change every function's parameters to one of temp: 
 * Motivation: avoid sth like "x x + = temp1"  (meaning temp1 = x + x)
 * Otherwise would lead to reading from same tape...
 * just not the assignment-operator: "="
 * Not necessary for functions/operators with at most one parameter
 */
std::vector<std::string> paramsToTemp(std::vector<std::string> &program) {
	std::vector<std::tuple<std::string, std::vector<std::string>, std::string> > funcs = getFunctions(program);
		
	std::unordered_map<std::string, int> numParams;
	for(size_t i = 0; i < funcs.size(); ++i) {
		std::tuple<std::string, std::vector<std::string>, std::string> func = funcs[i];
		numParams[std::get<0>(func)] = std::get<1>(func).size();
	}
	
	// have to know variable declarations (process as you go to avoid "int x"in one function, then "bool x"in another function
	std::unordered_map<std::string, std::string> varType;

	std::vector<std::string> ans;

	for(size_t i = 0; i < program.size(); ++i) {
		std::string line = program[i];
		std::vector<std::string> words = getWords(line);
		
		int opIndex = -1;
		for(size_t j = 0; j < words.size(); ++j) {
			std::string word = words[j];
			if(word != "=" && numParams.find(word) != numParams.end() && numParams[word] > 1) {
				opIndex = j;
				break;
			}
		}
		
		if(words[words.size() - 1] == "{" || opIndex == -1) {
			// ^ is so function declarations aren't counted
			ans.push_back(line);

			// is it simple declaration?
			std::unordered_set<std::string> validTypes {"int", "bool"};
			if(validTypes.find(words[0]) != validTypes.end()) {
				std::string varName = words[1];
				varType[varName] = words[0];
			}
		}
		else {
			int numParam = numParams[words[opIndex]];
			std::vector<std::string> params;
			int index = opIndex - 1;
			for(int j = 0; j < numParam; ++j) {
				std::string param = words[index];
				--index;
				if(param == "!VAR_LIB_MEM") {
					param = words[index] + " " + words[index + 1];
					--index;
				}
				params.push_back(param);
			}

			std::reverse(params.begin(), params.end());
			// now have a list of parameters to function: for any param that is not temp, add temp variable and replace
			
			std::vector<std::string> fixedParams;
			
			for(size_t j = 0; j < params.size(); ++j) {
				std::string param = params[j];
				if(param.substr(0, 9) != "!VAR_TEMP" || param.substr(param.size() - 13, 13) == " !VAR_LIB_MEM") {
					std::string tempVar = "!VAR_TEMP_temp" + std::to_string(tempCount);
					++tempCount;

					fixedParams.push_back(tempVar);
						
					std::string desiredType = varType[param];
					ans.push_back(desiredType + " " + tempVar + " ; "); 		// equiv to: int/bool tempVar;
					ans.push_back(tempVar + " " + param + " = ; ");				// tempVar = param;
				}
				else {
					fixedParams.push_back(param);
				}
			}
			
			std::string tempLine;
			for(int j = 0; j < index; ++j) {
				tempLine.append(words[j]);
				tempLine.append(" ");
			}
			
			// append params
			for(size_t j = 0; j < fixedParams.size(); ++j) {
				tempLine.append(fixedParams[j]);
				tempLine.append(" ");
			}

			// append operator and rest of words
			for(int j = opIndex; j < words.size(); ++j) {
				tempLine.append(words[j]);
				tempLine.append(" ");
			}
			ans.push_back(tempLine);
			//ans.push_back(line);
		}
	}	

	std::string tempLine;
	for(size_t i = 0; i < ans.size(); ++i) {
		std::vector<std::string> words = getWords(ans[i]);
		for(size_t j = 0; j < words.size(); ++j) {
			tempLine.append(words[j]);
			tempLine.append(" ");
		}
	}

	std::vector<std::string> ans2;
	ans2.push_back(tempLine);

	return ans2;
}

/**
 * Change memory access to functions
 * !VAR_LIB_MEM[0] = x  ->  !FUNC_LIB_memset(0, x)
 * x = !VAR_LIB_MEM[0]  ->  x = !FUNC_LIB_memget(0)
 */
std::vector<std::string> convertMemoryAccess(std::vector<std::string> &program) {
	// first find and replace setting memory: 
	// anywhere it looks like MEM [ ... ] =  or += or ...

	std::string letters = getLetters(program);
	std::vector<std::string> words = getWords(letters);
	
	for(size_t i = 0; i < words.size(); ++i) {
		if(words[i] == "!VAR_LIB_MEM") {
			int openBracket = i + 1;
			int closeBrace = findOpposite(words, openBracket);
			std::string op = words[closeBrace + 1];
			if(op[op.size() - 1] == '=') {
				
				// search for end of expression
				int startSearch = closeBrace + 2;
				while(words[startSearch] != ";" && (words[startSearch] != ")" || findOpposite(words, startSearch) > openBracket) && (words[startSearch] != "]" || findOpposite(words, startSearch) > openBracket)) {
					++startSearch;		
				}

				std::vector<std::string> temp;
				for(size_t j = closeBrace + 2; j < startSearch; ++j) {
					temp.push_back(words[j]);
				}

				std::vector<std::string> index;
				for(size_t j = openBracket + 1; j < closeBrace; ++j) {
					index.push_back(words[j]);
				}

				// create new copy with changes
				std::vector<std::string> words2;
				for(size_t j = 0; j < i; ++j) {
					words2.push_back(words[j]);
				}
				words2.push_back("!FUNC_LIB_MEMSET");
				words2.push_back("(");
				for(std::string word : index) {
					words2.push_back(word);
				}
				words2.push_back(",");
				for(std::string word : temp) {
					words2.push_back(word);
				}
				words2.push_back(")");
				for(size_t j = startSearch; j < words.size(); ++j) {
					words2.push_back(words[j]);
				}

				// now swap new content into original words
				words = words2;
			}
		}
	}
	
	std::vector<std::string> ans;
	for(size_t i = 0; i < words.size(); ++i) {
		std::string word = words[i];
		ans.push_back(word);
		if(i + 1 < words.size()) {
			ans.push_back(" ");
		}
	}
	
	return ans;
}

std::vector<std::string> midToLow(std::vector<std::string> &program) {
	std::vector<std::string> modifiedProgram = program;

	modifiedProgram = removeComments(modifiedProgram);
	modifiedProgram = addSpaceAroundOperators(modifiedProgram);
	modifiedProgram = formatProgram(modifiedProgram);

	modifiedProgram = renameUserVariables(modifiedProgram);
	modifiedProgram = renameBuiltInVariables(modifiedProgram);
	modifiedProgram = formatProgram(modifiedProgram);

	modifiedProgram = forToWhile(modifiedProgram);
	modifiedProgram = convertElif(modifiedProgram);
	modifiedProgram = formatProgram(modifiedProgram);
	
	modifiedProgram = changeWhileLoops(modifiedProgram);
	modifiedProgram = formatProgram(modifiedProgram);

	modifiedProgram = voidReturns(modifiedProgram);
	modifiedProgram = formatProgram(modifiedProgram);
	
	modifiedProgram = moveUpDeclaration(modifiedProgram);
	modifiedProgram = formatProgram(modifiedProgram);

	modifiedProgram = simplifyExpressions(modifiedProgram);
	modifiedProgram = formatProgram(modifiedProgram);
	
	modifiedProgram = convertSpecialAssignment(modifiedProgram);
	modifiedProgram = formatProgram(modifiedProgram); 
	
	modifiedProgram = paramsToTemp(modifiedProgram);
	modifiedProgram = formatProgram(modifiedProgram);

	//modifiedProgram = convertMemoryAccess(modifiedProgram);
	//modifiedProgram = formatProgram(modifiedProgram);
	
	return modifiedProgram;
}

int main() {
	tempCount = 0;
	std::string fileName = "example.source";
	std::ifstream file(fileName);	

	if(!file.is_open()) {
		std::cout << "Unable to open file " << fileName << std::endl;
		return -1;
	}

	int status = checkCompilation(fileName);
	if(status != 0) {
		std::cout << "Error compiling source code: " << std::endl;
		return status;
	}
	else {
		std::cout << "Compilation OK" << std::endl;
	}

	std::vector<std::string> program;
	std::string line;
	while(std::getline(file, line)) {
		program.push_back(line);
	}
	
	file.close();
	
	std::vector<std::string> transformedProgram = midToLow(program);
	printPrettyProgram(transformedProgram);

	return 0;
}
