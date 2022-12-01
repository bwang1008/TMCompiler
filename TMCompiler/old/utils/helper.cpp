#include "helper.hpp"

#include <algorithm>		// std::reverse
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

/**
 * helper function: convert line number (non-negative)
 * to format of ipSize bits: 0 for non-negative, then
 * least-significant to most significant bits
 */
std::vector<std::string> lineNumToBits(const size_t lineNum, const size_t ipSize) {
	std::vector<std::string> bits;

	size_t val = lineNum;

	while(val > 0) {
		if(val % 2 == 0) {
			bits.push_back("0");
		}
		else {
			bits.push_back("1");
		}

		val /= 2;
	}

	while(bits.size() < ipSize) {
		bits.push_back("0");
	}

	std::reverse(bits.begin(), bits.end());

	return bits;
}

/**
 * calculate biginteger / 2 (floor integer division)
 * assume val is positive
 */
std::string divideIntegerBy2(const std::string &val) {
	if(val.size() == 0) {
		throw std::invalid_argument("Provided parameter is blank, so not an integer");
	}

	bool carry = false;

	std::string ans;
	for(size_t i = 0; i < val.size(); ++i) {
		int digit = (val[i] - '0');
		
		if(digit < 0 || digit >= 10) {
			throw std::invalid_argument("Val provided for divideIntegerBy2 not an integer: " + val);
		}

		if(carry) {
			digit += 10;
		}

		ans.push_back('0' + (digit / 2));
		carry = (digit % 2 == 1);
	}

	size_t firstNon0 = 0;
	while(firstNon0 < ans.size() && ans[firstNon0] == '0') {
		++firstNon0;
	}

	ans = ans.substr(firstNon0, ans.size() - firstNon0);
	
	if(ans.size() == 0) {
		ans.push_back('0');
	}

	return ans;
}

/**
 * calculate biginteger * 2
 * assume val is non-negative
 */
std::string multiplyIntegerBy2(const std::string &val) {
	if(val.size() == 0) {
		throw std::invalid_argument("Provided parameter is blank, so not an integer");
	}

	int carry = 0;

	std::string ans;
	for(int i = static_cast<int>(val.size()) - 1; i >= 0; --i) {
		int digit = (val[i] - '0');
		
		if(digit < 0 || digit >= 10) {
			throw std::invalid_argument("Val provided for divideIntegerBy2 not an integer: " + val);
		}

		int output = 2 * digit + carry;

		ans.push_back('0' + (output % 10));
		carry = (output / 10);
	}

	if(carry > 0) {
		ans.push_back('0' + carry);
	}

	std::reverse(ans.begin(), ans.end());
	
	return ans;
}

/**
 * calculate biginteger + 1
 * assume val is non-negative
 */
std::string incrementInteger(const std::string &val) {
	if(val.size() == 0) {
		throw std::invalid_argument("Provided parameter is blank, so not an integer");
	}

	int carry = 1;

	std::string ans;
	for(int i = static_cast<int>(val.size()) - 1; i >= 0; --i) {
		int digit = (val[i] - '0');
		
		if(digit < 0 || digit >= 10) {
			throw std::invalid_argument("Val provided for divideIntegerBy2 not an integer: " + val);
		}

		int output = digit + carry;

		ans.push_back('0' + (output % 10));
		carry = (output / 10);
	}

	if(carry > 0) {
		ans.push_back('0' + carry);
	}

	std::reverse(ans.begin(), ans.end());
	
	return ans;
}

/**
 * Convert string representing integer literal to bits
 * Use style of (1 if neg, 0 if non-negative), then bits from 
 * least-significant to most-significant. 0 is just "0"
 */
std::string convertIntegerToBits(std::string val) {
	if(val.size() == 0) {
		throw std::invalid_argument("Argument to convertIntegerToBits is blank, so not an integer");
	}

	std::string bits;

	if(val[0] == '-') {
		bits.push_back('1');
		val = val.substr(1, val.size() - 1);
	}
	else {
		bits.push_back('0');
	}

	while(val.size() != 1 || val[0] != '0') {
		const int mod2 = ((val.back() - '0') % 2);
		bits.push_back('0' + mod2);
		val = divideIntegerBy2(val);
	}

	return bits;
}

/**
 * Convert bit-string into a decimal integer.
 * Bit-string in form mentioned above: [sign-bit] then 
 * least-significant bit to most significant bit (no ending 0's)
 */
std::string convertBitStringToDecimalInteger(const std::string &bitString) {
	if(bitString.size() == 0) {
		throw std::invalid_argument("Parameter is blank, so not an integer");
	}

	bool isNeg = (bitString[0] == '1');

	std::string ans("0");
	for(size_t i = bitString.size() - 1; i >= 1; --i) {
		ans = multiplyIntegerBy2(ans);
		if(bitString[i] == '1') {
			ans = incrementInteger(ans);	
		}
	}

	if(isNeg) {
		ans = "-" + ans;
	}

	return ans;
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

	if(status == 0) {
		const std::string removeCommand = std::string("rm " + tempFileName);
		std::cout << "Execute: " << removeCommand << std::endl;
		const int removeStatus = std::system(removeCommand.c_str());
		if(removeStatus != 0) {
			std::cout << "There was a problem removing " << tempFileName << std::endl;
			std::cout << "You may have to manually remove this file" << std::endl;
		}
	}

	return status;
}

