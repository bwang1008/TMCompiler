#ifndef UTILS_H
#define UTILS_H

#include <algorithm>		// std::find_if
#include <cctype>			// std::isspace
#include <string>			// std::string
#include <vector>			// std::vector

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

bool isBlankString(const std::string &s);

bool isIntegerLiteral(const std::string &s);

std::string getAlphaNumericWord(const std::string &s, const int index);

std::string getNonBlankWord(const std::string &s, const int index);

std::string formSubstring(const std::vector<char> &s, const int start, const int end);

int findNext(const std::string &s, const char c, const int start = 0);

int findNext(const std::vector<char> &s, const char c, const int start = 0);

int findNext(const std::vector<std::string> &words, const std::string &word, const int start = 0);

int findPrev(const std::string &s, const char c, const int start = 0);

int findPrev(const std::vector<char> &s, const char c, const int start = 0);

int findNonBlank(const std::vector<char> &s, const int start = 0);

int findNonBlank(const std::string &s, const int start = 0);

int findBlank(const std::string &s, const int start = 0);

int findOpposite(const std::vector<char> &s, const int index);

int findOpposite(const std::string &s, const int index);

int findOpposite(const std::vector<std::string> &words, const int index);

int vimB(const std::string &s, int index);

void addIndents(std::string &s, const size_t numIndent);

void printProgram(const std::vector<std::string> &program, const bool showLines = true);

void printPrettyProgram(const std::vector<std::string> &program, const bool showLines = true);

std::string getLetters(const std::vector<std::string> &program);

std::vector<std::string> getWords(const std::string &letters) ;

size_t parseTapeNum(const std::string &word);

int checkCompilation(const std::string &fileName);

#endif
