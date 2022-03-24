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

std::string getAlphaNumericWord(const std::string &s, int index);

std::string getNonBlankWord(const std::string &s, int index);


std::string formSubstring(std::vector<char> &s, int start, int end);

int findNext(std::string &s, char c, int start = 0);

int findNext(std::vector<char> &s, char c, int start = 0);

int findNext(std::vector<std::string> &words, const std::string &word, int start = 0);

int findPrev(std::string &s, char c, int start = 0);

int findPrev(std::vector<char> &s, char c, int start = 0);

int findNonBlank(std::vector<char> &s, int start = 0);

int findNonBlank(std::string &s, int start = 0);

int findBlank(std::string &s, int start = 0);

int findOpposite(std::vector<char> &s, int index);

int findOpposite(std::string &s, int index);

int findOpposite(std::vector<std::string> &words, int index);

int vimB(std::string &s, int index);

void addIndents(std::string &s, int numIndent);

void printProgram(std::vector<std::string> &program, bool showLines = true);

void printPrettyProgram(std::vector<std::string> &program, bool showLines = true);

std::string getLetters(std::vector<std::string> &program);

std::vector<std::string> getWords(std::string &letters) ;

int checkCompilation(std::string &fileName);

#endif
