#include "helper.hpp"

#include <algorithm>		// std::find_if
#include <cctype>			// std::isspace
#include <cstdlib>			// std::system
#include <fstream>			// std::ifstream, std::ofstream
#include <iomanip>			// std::setw
#include <iostream>			// std::cout, std::endl
#include <string>			// std::string, std::getline, std::to_string
#include <vector>			// std::vector

/**
 * Implementation of helper functions, mainly for TMCompiler/compilation/unit2.cpp
 */

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
	
	size_t start = 0;
	if(s[0] == '-') {
		if(s.size() == 1) {
			return false;
		}

		start = 1;
	}
	
	for(size_t i = start; i < s.size(); ++i) {
		if(s[i] < '0' || '9' < s[i]) {
			return false;
		}
	}
	
	return true;
}

std::string getAlphaNumericWord(const std::string &s, const size_t index) {
	int left = static_cast<int>(index);
	while(0 <= left && (('0' <= s[left] && s[left] <= '9') || ('a' <= s[left] && s[left] <= 'z') || ('A' <= s[left] && s[left] <= 'Z'))) {
		--left;
	}
	
	size_t right = index;
	while(right < s.size() && (('0' <= s[right] && s[right] <= '9') || ('a' <= s[right] && s[right] <= 'z') || ('A' <= s[right] && s[right] <= 'Z'))) {
		++right;
	}
	
	return s.substr(1 + left, right - left - 1);
}

std::string getNonBlankWord(const std::string &s, const int index) {
	int left = index;
	while(0 <= left && !std::isspace(s[left])) {
		--left;
	}
		
	int right = index;
	while(right < static_cast<int>(s.size()) && !std::isspace(s[right])) {
		++right;
	}
	
	return s.substr(1 + left, right - left - 1);
}

std::string formSubstring(const std::vector<char> &s, const int start, const int end) {
	std::string ans = "";
	for(int i = start; i < end; ++i) {
		ans.push_back(s[i]);
	}

	return ans;
}

int findNext(const std::string &s, const char c, const int start) {
	int index = start;
	while(static_cast<size_t>(index) < s.size() && s[index] != c) {
		++index;	
	}

	if(static_cast<size_t>(index) == s.size()) {
		return -1;
	}

	return index;
}

int findNext(const std::vector<char> &s, const char c, const int start) {
	const std::string s2 = formSubstring(s, 0, s.size());
	return findNext(s2, c, start);
}

int findNext(const std::vector<std::string> &words, const std::string &word, const int start) {
	int index = start;
	while(static_cast<size_t>(index) < words.size() && words[index] != word) {
		++index;
	}
	
	if(index == static_cast<int>(words.size())) {
		return -1;
	}
	
	return index;
}

int findPrev(const std::string &s, const char c, const int start) {
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

int findPrev(const std::vector<char> &s, const char c, const int start) {
	std::string s2 = formSubstring(s, 0, s.size());
	return findPrev(s2, c, start);
}

int findNonBlank(const std::vector<char> &s, const int start) {
	int index = start;
	while(static_cast<size_t>(index) < s.size() && std::isspace(s[index])) {
		++index;
	}

	if(static_cast<size_t>(index) == s.size()) {
		return -1;
	}

	return index;
}

int findNonBlank(const std::string &s, const int start) {
	const std::vector<char> v(s.begin(), s.end());
	return findNonBlank(v, start);
}

int findBlank(const std::string &s, const int start) {
	int index = start;
	while(static_cast<size_t>(index) < s.size() && !std::isspace(s[index])) {
		++index;
	}
	
	if(index == static_cast<int>(s.size())) {
		index = -1;
	}
	
	return index;
}

int findOpposite(const std::vector<char> &s, const int index) {
	const char c = s[index];
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

int findOpposite(const std::string &s, const int index) {
	const std::vector<char> v(s.begin(), s.end());
	return findOpposite(v, index);
}

int findOpposite(const std::vector<std::string> &words, const int index) {
	const std::string word = words[index];
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

int vimB(const std::string &s, int index) {
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

void addIndents(std::string &s, const size_t numIndent) {
	for(size_t i = 0; i < numIndent; ++i) {
		s.push_back('\t');
	}
}

void printProgram(const std::vector<std::string> &program, const bool showLines) {
	if(program.size() == 0) {
		std::cout << "Program is empty" << std::endl;
		return;
	}

	const size_t numDigits = std::to_string(program.size() - 1).size();
	for(size_t i = 0; i < program.size(); ++i) {
		if(showLines) {
			std::cout << std::setw(numDigits) << i << ": ";
		}
		std::cout << program[i] << std::endl;
	}
}

void printPrettyProgram(const std::vector<std::string> &program, const bool showLines) {
	if(program.size() == 0) {
		std::cout << "Program is empty" << std::endl;
	}

	const size_t numDigits = std::to_string(program.size() - 1).size();
	for(size_t i = 0; i < program.size(); ++i) {
		if(showLines) {
			std::cout << std::setw(numDigits) << i << ": ";
		}
		
		const std::string line = program[i];
		
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

std::string getLetters(const std::vector<std::string> &program) {
	std::string ans;
	for(size_t i = 0; i < program.size(); ++i) {
		for(size_t j = 0; j < program[i].size(); ++j) {
			ans.push_back(program[i][j]);
		}
		ans.push_back('\n');
	}
	
	return ans;
}

std::vector<std::string> getWords(const std::string &letters)  {
	std::vector<std::string> words;
	
	int startSearch = findNonBlank(letters, 0);
	if(startSearch < 0) {
		return words;
	}

	while(static_cast<size_t>(startSearch) < letters.size()) {
		const std::string word = getNonBlankWord(letters, startSearch);
		words.push_back(word);

		startSearch = findBlank(letters, startSearch);
		if(startSearch < 0) {
			break;
		}
		
		startSearch = findNonBlank(letters, startSearch);
	}
 
	return words;
}

/**
 * Turns sth like "!TAPE_tape5" into 5
 */
size_t parseTapeNum(const std::string &word) {
	if(word.size() < 10 || word.substr(0, 10) != "!TAPE_tape") {
		throw std::invalid_argument("word must start with !TAPE_tape");
	}

	const std::string suffix = word.substr(10, word.size() - 10);

	return std::stoi(suffix);
}

int checkCompilation(const std::string &fileName) {
	const int periodIndex = findPrev(fileName, '.', fileName.size() - 1);

	std::string prefix = "";

	if(periodIndex == -1) {
		prefix = fileName;
	}
	else {
		prefix = fileName.substr(0, periodIndex);
	}

	const std::string tempFileName = prefix + "_temp.cpp";
	
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

	const std::string command = std::string("g++ -fsyntax-only ") + tempFileName;
	std::cout << "Execute: " << command << std::endl;
	const int status = std::system(command.c_str());

	return status;
}

