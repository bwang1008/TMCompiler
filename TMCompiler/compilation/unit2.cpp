/*
Convert C++-like source-code into my own assembly language
Function names must be unique
Variable names must be distinct from function names
No (user) global variables allowed
Global infinite int[] array called MEM: 0-indexed
No ++ or -- operators, nor bit-shifts
only allowed types are int and bool
*/

#include "unit2.hpp"

#include <algorithm>		// std::reverse
#include <fstream>			// std::ifstream
#include <iostream>			// std::cout, std::endl
#include <map>				// std::map
#include <set>				// std::set
#include <string>			// std::string, std::getline, std::to_string
#include <tuple>			// std::tuple, std::make_tuple, std::get<>
#include <unordered_map>	// std::unordered_map
#include <unordered_set>	// std::unordered_set
#include <vector>			// std::vector

#include "TMCompiler/utils/helper.hpp"			// helper functions; basic algorithms

#define SINGLE_FILE 0

int tempCount = 0;			// counter for number of temp variables used

/**
 * create a new identifier for temporary variable
 */
std::string createTemp() {
	const std::string varName = "!VAR_TEMP_temp" + std::to_string(tempCount);
	++tempCount;
	return varName;
}

/**
 * Remove all user comments: line comments of the form //
 * or block comments (potentially multiple lines) of the form / * ... * /
 *
 */
std::vector<std::string> removeComments(const std::vector<std::string> &program) {
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
				size_t end = start;
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
				size_t end = start + 2;
				while(end < letters.size() && (letters[end - 1] != '*' || letters[end] != '/')) {
					if(letters[end] == '\n') {
						++numLines;
					}
					isComment[end - 1] = isComment[end] = true;
					++end;
				}

				if(end == letters.size()) {
					// error
					std::cerr << "Error: Could not find end of block comment start at line " << numLines << std::endl;
					return ans;
				}

				isComment[end - 1] = isComment[end] = true;
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
						
						// if just a one-letter op, put space, by first popping op, then add space + op + space
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
	
	while(0 <= startSearch && static_cast<size_t>(startSearch) < letters.size()) {
		std::string word = getNonBlankWord(letters, startSearch);
		words.push_back(word);

		startSearch = findBlank(letters, startSearch);

		if(startSearch == -1 || static_cast<size_t>(startSearch) == letters.size()) {
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
std::vector<std::string> renameUserVariables(std::vector<std::string> &program, std::string funcPrefix = "!FUNC_USER_") {
	std::vector<std::string> ans;
	
	std::unordered_set<std::string> validTypes {"int", "bool", "void"};

	std::unordered_set<std::string> origFuncNames;
	std::unordered_set<std::string> origVarNames;

	std::vector<std::string> words;
	for(size_t i = 0; i < program.size(); ++i) {
		std::string line = program[i];
		
		int index = findNonBlank(line, 0);
		while(0 <= index && static_cast<size_t>(index) < line.size()) {
			std::string word = getNonBlankWord(line, index);
			words.push_back(word);

			index = findBlank(line, index);
			if(index < 0 || static_cast<size_t>(index) >= line.size()) {
				break;
			}

			index = findNonBlank(line, index);
		}
	}

	for(size_t i = 1; i < words.size(); ++i) {
		std::string prev = words[i - 1];
		if(validTypes.find(prev) != validTypes.end()) {
			if(i + 1 < words.size() && words[i + 1] == "(") { // && words[i].substr(0, 10) != "!FUNC_LIB_") {
				// // if function declaration, but is not a !FUNC_LIB_ function
				origFuncNames.insert(words[i]);
			}
			else {
				origVarNames.insert(words[i]);
			}
		}
	}

	/*
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
	*/

	std::string letters;
	for(size_t i = 0; i < program.size(); ++i) {
		for(size_t j = 0; j < program[i].size(); ++j) {
			letters.push_back(program[i][j]);
		}

		letters.push_back('\n');
	}

	// now replace function call, with prefix
	//std::string funcPrefix = "!FUNC_USER_";
	std::string varPrefix = "!VAR_USER_";

	for(int startSearch = static_cast<int>(letters.size()) - 1; startSearch >= 0; startSearch = vimB(letters, startSearch)) {
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

	std::unordered_set<std::string> reservedForTM {"nextInt", "printInt", "printSpace", "isZero", "isNeg", "isPos", "basic_add", "basic_sub", "basic_xor", "basic_eq", "basic_lt", "basic_neg", "basic_mul2", "basic_div2", "isEven", "isOdd", "getMemBitIndex", "setMemBitIndex", "moveMemHeadRight", "moveMemHeadLeft", "setMemBitZero", "setMemBitOne", "setMemBitBlank", "memBitIsZero", "memBitIsOne", "memBitIsBlank"};

	std::vector<std::string> words;
	
	int startSearch = findNonBlank(letters, 0);
	while(0 <= startSearch && startSearch < static_cast<int>(letters.size())) {
		std::string word = getNonBlankWord(letters, startSearch);
		
		if(word == "MEM") {
			word = varPrefix + word;
		}
		else if(reservedForTM.find(word) != reservedForTM.end()) {
			word = funcPrefix + word;
		}
		
		words.push_back(word);

		startSearch = findBlank(letters, startSearch);
		if(startSearch < 0 || static_cast<size_t>(startSearch) >= letters.size()) {
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
 *  Add in library functions (like division/modulo/memset/memget/nextInt/printInt/printSpace)
 *  Question mark about nextInt/printInt/printSpace
 */
std::vector<std::string> addLibraryFunctions(std::vector<std::string> &program) {
	std::vector<std::string> ans;

	std::string libFileName = "lib.cpp";
	std::ifstream libFile(libFileName);
	
	if(!libFile.is_open()) {
		std::cout << "Library file " << libFileName << " not found" << std::endl;
		return program;
	}
	else {
		std::cout << "Library file found" << std::endl;
	}

	std::string line;
	while(std::getline(libFile, line)) {
		ans.push_back(line);
	}
	
	// go through same transformations; just prefix functions differently
	ans = removeComments(ans);
	ans = addSpaceAroundOperators(ans);
	ans = formatProgram(ans);

	ans = renameUserVariables(ans, "!FUNC_LIB_");
	ans = renameBuiltInVariables(ans);
	ans = formatProgram(ans);

	for(size_t i = 0; i < program.size(); ++i) {
		ans.push_back(program[i]);
	}

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
	std::vector<std::string> allWords;
	for(size_t i = 0; i < program.size(); ++i) {
		std::vector<std::string> words = getWords(program[i]);
		allWords.insert(allWords.end(), words.begin(), words.end());
	}

	for(size_t i = 0; i < allWords.size(); ++i) {
		if(i + 1 < allWords.size() && allWords[i] == "else" && allWords[i + 1] == "if") {
			// every else-if, put a open brace between else + if, and a close brace after the last close brace of entire if-elseif-else statement
			int openBrace = findNext(allWords, "{", i + 1);
			int closeBrace = findOpposite(allWords, openBrace);

			while(static_cast<size_t>(closeBrace + 1) < allWords.size() && allWords[closeBrace + 1] == "else") {
				openBrace = findNext(allWords, "{", closeBrace + 1);
				closeBrace = findOpposite(allWords, openBrace);
			}

			int lastBrace = closeBrace;

			allWords.insert(allWords.begin() + lastBrace + 1, "}");
			allWords.insert(allWords.begin() + i + 1, "{");
		}
	}

	std::string tempLine;
	for(size_t i = 0; i < allWords.size(); ++i) {
		tempLine.append(allWords[i]);
		tempLine.append(" ");
	}

	std::vector<std::string> ans {tempLine};
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
 * Add parenthesis around what is returned. Like return x; -> return (x);
 * Necessary to prevent some incorrect stuff with shunting-yard, sth like return (sth) || (sth2)
 */
std::vector<std::string> parenthesisReturn(std::vector<std::string> &program) {
	std::vector<std::string> allWords;
	for(size_t i = 0; i < program.size(); ++i) {
		std::vector<std::string> words = getWords(program[i]);
		allWords.insert(allWords.end(), words.begin(), words.end());
	}

	std::vector<std::string> allWords2;
	for(size_t i = 0; i < allWords.size(); ++i) {
		if(allWords[i] == "return" && allWords[i + 1] != ";") {
			allWords2.push_back("return");
			allWords2.push_back("(");
			size_t j;
			for(j = i + 1; allWords[j] != ";"; ++j) {
				allWords2.push_back(allWords[j]);
			}

			allWords2.push_back(")");
			
			i = j - 1;
		}
		else {
			allWords2.push_back(allWords[i]);
		}
	}

	std::string tempLine;
	for(size_t i = 0; i < allWords2.size(); ++i) {
		tempLine.append(allWords2[i]);
		tempLine.append(" ");
	}

	std::vector<std::string> ans {tempLine};
	return ans;
}

/**
 * Change int x = 5; -> int x; x = 5;
 * and int x; -> int x; -> x = 0;
 */
std::vector<std::string> assignDefaultToDeclaration(std::vector<std::string> &program) {
	std::vector<std::vector<std::string> > allWords;

	for(size_t i = 0; i < program.size(); ++i) {
		std::string line = program[i];
		std::vector<std::string> words = getWords(line);
		allWords.push_back(words);
	}

	// find variable declarations: "int !VAR = "
	// as well as "int x;"
	// no multiple declarations in a single line: no "int x, y;"
	
	// map from function declaration line (which function variable lives in), to all declared variables

	for(int i = (int) program.size() - 1; i >= 0; --i) {
		std::vector<std::string> words = allWords[i]; 

		std::unordered_map<std::string, std::string> validTypes {{"int", "0"}, {"bool", "false"}};
		if(validTypes.find(words[0]) != validTypes.end() && words[1].substr(0, 4) == "!VAR") {	
			std::string typeName = words[0];
			std::string varName = words[1];
			if(words[2] == ";") {
				words.clear();
				
				std::vector<std::string> temp {typeName, varName, ";"};
				for(std::string word : temp) {
					words.push_back(word);
				}
				
				words.push_back(varName);
				words.push_back("=");
				words.push_back(validTypes[typeName]);
				words.push_back(";");
			}
			else {
				std::vector<std::string> words2 {typeName, varName, ";"};
				for(size_t j = 1; j < words.size(); ++j) {
					words2.push_back(words[j]);
				}
				words = words2;
			}

			allWords[i] = words;
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
 * Move declarations to the top of the function:
 */
std::vector<std::string> moveUpDeclaration(std::vector<std::string> &program) {
	std::vector<std::vector<std::string> > allWords;

	std::unordered_set<std::string> validTypes {"void", "int", "bool"};

	std::vector<int> funcLines;
	for(size_t i = 0; i < program.size(); ++i) {
		std::string line = program[i];
		std::vector<std::string> words = getWords(line);
	
		allWords.push_back(words);

		if(validTypes.find(words[0]) != validTypes.end() && words[1].substr(0, 5) == "!FUNC") {
			// function declaration found
			funcLines.push_back((int) i);
		}
	}

	// find variable declarations:as well as "int x;"
	// no multiple declarations in a single line: no "int x, y;"
	
	// map from function declaration line (which function variable lives in), to all declared variables
	//std::map<int, std::unordered_set<std::string> > intVars;
	//std::map<int, std::unordered_set<std::string> > boolVars;

	// since we reuse tapes, and a tape can hold an int or bool, type doesn't actually matter here
	// so just map from function declaration line, to the declared variables in it (whether int or bool)
	std::map<int, std::unordered_set<std::string> > vars;

	for(int i = (int) program.size() - 1; i >= 0; --i) {
		std::vector<std::string> words = allWords[i]; 

		std::unordered_set<std::string> validTypes {"int", "bool"};
		if(validTypes.find(words[0]) != validTypes.end() && words[1].substr(0, 4) == "!VAR") {
			int funcLine = funcLines[funcLines.size() - 1];
			while(funcLine > i) {
				funcLines.pop_back();
				funcLine = funcLines[funcLines.size() - 1];
			}
			
			std::string typeName = words[0];
			std::string varName = words[1];
			if(words[2] == ";") { 		// change "int x ;" into "nop"; putting declaration earlier
				words.clear();
				words.push_back("nop");
			}

			allWords[i] = words;

			/*
			if(typeName == "int") {
				intVars[funcLine].insert(varName);
			}
			else {
				boolVars[funcLine].insert(varName);
			}
			*/

			vars[funcLine].insert(varName);
		}
	}

	/*
	std::map<int, std::unordered_set<std::string> >::reverse_iterator it1 = intVars.rbegin();
	std::map<int, std::unordered_set<std::string> >::reverse_iterator it2 = boolVars.rbegin();

	while(it1 != intVars.rend() && it2 != boolVars.rend()) {
		if(it1->first >= it2->first) {
			std::unordered_set<std::string> varNames = it1->second;
			for(std::string varName : varNames) {
				std::vector<std::string> temp {"int", varName, ";"};
				allWords.insert(allWords.begin() + it1->first + 1, temp);
			}
			++it1;
		}
		else {
			std::unordered_set<std::string> varNames = it2->second;
			for(std::string varName : varNames) {
				std::vector<std::string> temp {"bool", varName, ";"};
				allWords.insert(allWords.begin() + it2->first + 1, temp);
			}
			++it2;
		}
	}
	
	while(it1 != intVars.rend()) {
		std::unordered_set<std::string> varNames = it1->second;
		for(std::string varName : varNames) {
			std::vector<std::string> temp {"int", varName, ";"};
			allWords.insert(allWords.begin() + it1->first + 1, temp);
		}
		++it1;
	}
		
	while(it2 != boolVars.rend()) {
		std::unordered_set<std::string> varNames = it2->second;
		for(std::string varName : varNames) {
			std::vector<std::string> temp {"bool", varName, ";"};
			allWords.insert(allWords.begin() + it2->first + 1, temp);
		}
		++it2;
	}
	*/

	// we need to have a declare line on every function, even those where you don't declare any variables
	
	// get function declaration lines for which variables already exist(again);
	std::vector<int> funcLines2;
	for(std::map<int, std::unordered_set<std::string> >::reverse_iterator it = vars.rbegin(); it != vars.rend(); ++it) {
		std::unordered_set<std::string> varNames = it->second;

		funcLines2.push_back(it->first);

		std::vector<std::string> tempWords {"declare"};
		for(std::string varName : varNames) {
			tempWords.push_back(varName);	
		}

		tempWords.push_back(";");
		allWords.insert(allWords.begin() + it->first + 1, tempWords);
	}

	// since you went from front to back, by inserting into the lower lines, the upper lines are off. compensate!
	for(size_t i = 0; i < funcLines2.size(); ++i) {
		funcLines2[funcLines2.size() - i - 1] += i;	
	}

	// get function declaration lines (again); (after the changes to allWords above)
	std::set<int> funcLines3;
	for(size_t i = 0; i < allWords.size(); ++i) {
		std::vector<std::string> words = allWords[i];
		if(validTypes.find(words[0]) != validTypes.end() && words[words.size() - 1] == "{") {
			funcLines3.insert((int) i);		
		}
	}

	// remove funcLines2 values from funcLines3: get function declarations, for which variables had not existed, and therefore need a declare line explicitly added
	for(int val : funcLines2) {
		funcLines3.erase(val);
	}

	// for the rest of the functions, add in a "declare" line, right after function declaration (traverse from high to low line numbers)
	for(std::set<int>::reverse_iterator it = funcLines3.rbegin(); it != funcLines3.rend(); ++it) {
		int funcLine = *it;
		
		std::vector<std::string> words {"declare", ";"};
		allWords.insert(allWords.begin() + funcLine + 1, words);
	}

	// reformat
	std::vector<std::string> ans;
	std::string temp;
	for(size_t i = 0; i < allWords.size(); ++i) {
		for(size_t j = 0; j < allWords[i].size(); ++j) {
			if(allWords[i][j] != "nop") {
				temp.append(allWords[i][j]);
				temp.append(" ");
			}
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
	
	// memset, memget aren't here cuz implemented in lib.cpp

	funcs.push_back(std::make_tuple("!FUNC_LIB_isZero", std::vector<std::string> {"int"}, "bool"));
	funcs.push_back(std::make_tuple("!FUNC_LIB_isNeg", std::vector<std::string> {"int"}, "bool"));
	funcs.push_back(std::make_tuple("!FUNC_LIB_isPos", std::vector<std::string> {"int"}, "bool"));
	funcs.push_back(std::make_tuple("!FUNC_LIB_basic_add", std::vector<std::string> {"int", "int"}, "int"));
	funcs.push_back(std::make_tuple("!FUNC_LIB_basic_sub", std::vector<std::string> {"int", "int"}, "int"));
	funcs.push_back(std::make_tuple("!FUNC_LIB_basic_xor", std::vector<std::string> {"int", "int"}, "int"));
	funcs.push_back(std::make_tuple("!FUNC_LIB_basic_eq", std::vector<std::string> {"int", "int"}, "bool"));
	funcs.push_back(std::make_tuple("!FUNC_LIB_basic_lt", std::vector<std::string> {"int", "int"}, "bool"));
	funcs.push_back(std::make_tuple("!FUNC_LIB_basic_neg", std::vector<std::string> {"int"}, "int"));
	funcs.push_back(std::make_tuple("!FUNC_LIB_basic_mul2", std::vector<std::string> {"int"}, "int"));
	funcs.push_back(std::make_tuple("!FUNC_LIB_basic_div2", std::vector<std::string> {"int"}, "int"));
	funcs.push_back(std::make_tuple("!FUNC_LIB_isEven", std::vector<std::string> {"int"}, "bool"));
	funcs.push_back(std::make_tuple("!FUNC_LIB_isOdd", std::vector<std::string> {"int"}, "bool"));
	
	funcs.push_back(std::make_tuple("!FUNC_LIB_getMemBitIndex", std::vector<std::string>(), "int"));
	funcs.push_back(std::make_tuple("!FUNC_LIB_setMemBitIndex", std::vector<std::string> {"int"}, "void"));
	funcs.push_back(std::make_tuple("!FUNC_LIB_moveMemHeadRight", std::vector<std::string>(), "void"));
	funcs.push_back(std::make_tuple("!FUNC_LIB_moveMemHeadLeft", std::vector<std::string>(), "void"));
	funcs.push_back(std::make_tuple("!FUNC_LIB_setMemBitZero", std::vector<std::string>(), "void"));
	funcs.push_back(std::make_tuple("!FUNC_LIB_setMemBitOne", std::vector<std::string>(), "void"));
	funcs.push_back(std::make_tuple("!FUNC_LIB_setMemBitBlank", std::vector<std::string>(), "void"));
	funcs.push_back(std::make_tuple("!FUNC_LIB_memBitIsZero", std::vector<std::string>(), "bool"));
	funcs.push_back(std::make_tuple("!FUNC_LIB_memBitIsOne", std::vector<std::string>(), "bool"));
	funcs.push_back(std::make_tuple("!FUNC_LIB_memBitIsBlank", std::vector<std::string>(), "bool"));

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
			for(int j = openParen + 1; j < closeParen; ++j) {
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

	// these aren't really functions per-se, but need to account for them when simplifying up expressions
	//numParams["if"] = 1;
	//numParams["return"] = 1;

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
		// keep while loop as "while(true) {"
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
			std::string var = createTemp();
			//decBoolean.push_back(var);
			words[i] = var;
			
			ans.push_back("bool " + var + " ; ");	
			ans.push_back(var + " " + word + " = ; ");		// have statement like "temp1 = true" prepended
		}
		else if(isIntegerLiteral(word)) {
			std::string var = createTemp();
			//decInt.push_back(var);
			words[i] = var;
			
			ans.push_back("int " + var + " ; ");	
			ans.push_back(var + " " + word + " = ; ");		// have statement like "temp2 = 20" prepended
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
	// What. are you doing. += doesn't have to be words[1]
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

	// function call?
	// regular assignment? EDIT: nah, can have special assignment
	// while loops / if statements...?

	bool hasOpenBrace = words[words.size() - 1] == "{";
	bool hasComma = words[words.size() - 1] == ";";
	if(hasOpenBrace || hasComma) {
		words.pop_back();
	}

	std::vector<std::string> postfix = shuntingYard(words);

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
		for(int i = paramIndex; i <= op; ++i) {
			subexpression.append(postfix[i]);
			subexpression.append(" ");
		}
		std::string tempVar = createTemp();
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
		postfix.insert(postfix.begin() + paramIndex, tempVar);

	}

	// 1 operator left
	
	// careful: could be "temp1 temp2 && if" or "temp1 temp2 - return" (but don't count sth like "temp return" (no ; cuz handled later)
	if(postfix[postfix.size() - 1] == "if" || postfix[postfix.size() - 1] == "return") {
		if(postfix.size() > 2) {
			std::string tempVar = createTemp();
			// create first expression: "temp1 temp2 && = temp3"
			std::string subexpression;
			for(size_t i = 0; i < postfix.size() - 1; ++i) {
				subexpression.append(postfix[i]);
				subexpression.append(" ");
			}
			subexpression.append("= " + tempVar + " ; ");
			
			ans.push_back("bool " + tempVar + " ; ");	// declare tempVar
			ans.push_back(subexpression);

			// let postfix = "temp3 if"
			std::string lastWord = postfix[postfix.size() - 1];
			postfix.clear();
			postfix.push_back(tempVar + " " + lastWord); 	// the ; or openCurly is handled below
		}
		else if(postfix.size() == 2 && postfix[0].substr(0, 5) == "!FUNC") {
			// or "!FUNC_... if" : could be a function with no arguments
			std::string tempVar = createTemp();
			std::string funcName = postfix[0];
			// declare temp variable, call no argument function and assign to temp, then if on that
			ans.push_back("bool " + tempVar + " ; ");
			ans.push_back(funcName + " = " + tempVar + " ; ");	// format of funcs with variables: x f = y ;  (meaning y is assigned value of f(x))
			postfix[0] = tempVar;
		}
	}

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

	/*
	std::cout << "before adding short-circuiting:" << std::endl;
	for(size_t i = 0; i < ans.size(); ++i) {
		std::cout << ans[i] << std::endl;
	}
	std::cout << std::endl;
	*/

	// EDIT: add short-circuiting here. Change && and || expressions to if statements
	// Change here because argument came from one line of source code.
	
	std::vector<std::string> ans2;
	for(size_t i = 0; i < ans.size(); ++i) {
		std::vector<std::string> words = getWords(ans[i]);
		bool containsOr = false;
		bool containsAnd = false;
		int opIndex = -1;
		for(size_t j = 0; j < words.size(); ++j) {
			// only && and ||; not &= or |=: 
			/* bool f() { print("Hi"); return true;}
			 * bool b = false;
			 * b &= f();     <- here f is called and prints
			 * b = (b && f()); <- here f is not called due to short-circuiting
			 */
			// aka no need to worry about &= and |=, which are expanded later
			if(words[j] == "&&") {
				containsAnd = true;
				opIndex = (int) j;
			}
			else if(words[j] == "||") {
				containsOr = true;
				opIndex = (int) j;
			}
		}


		if(!(containsAnd || containsOr)) {
			ans2.push_back(ans[i]);
		}
		else {
			// && and || are for booleans only: will not have MEM[0] as argument
			// looks like "arg1 arg2 && = arg3 ; "
			std::string arg1 = words[opIndex - 2];
			std::string arg2 = words[opIndex - 1];
			std::string arg3 = words[opIndex + 2];	
			
			// holds lines where arg2 was defined; will put in if statement. Originally in reverse order
			std::vector<std::string> arg2Lines;

			// find last occurrence of arg1 (to find lines where arg2 is defined to put in if statement)
			bool prevLineContainsArg1 = false;
			std::vector<std::string> prevWords = getWords(ans2[ans2.size() - 1]);
			for(size_t j = 0; j < prevWords.size(); ++j) {
				if(prevWords[j] == arg1) {
					prevLineContainsArg1 = true;
					break;
				}
			}

			// while prev line doesn't mention arg1, decrease: this prev line must be part of arg2
			while(!prevLineContainsArg1) {
				prevLineContainsArg1 = false;
				std::string arg2Line = ans2[ans2.size() - 1];
				
				// edit: if multiple occurences of short-circuiting, like if(true || false && false), 
				// shouldn't include a closing brace as part of arg2Line
				std::vector<std::string> tempWords = getWords(arg2Line);
				if(tempWords[0] == "}") {
					break;
				}

				ans2.pop_back();
				arg2Lines.push_back(arg2Line);
				prevWords = getWords(ans2[ans2.size() - 1]);

				for(size_t j = 0; j < prevWords.size(); ++j) {
					if(prevWords[j] == arg1) {
						prevLineContainsArg1 = true;
						break;
					}
				}
			}

			std::reverse(arg2Lines.begin(), arg2Lines.end());

			// from Wikipedia article on "Short-circuit evaluation"
			// arg1 && arg2 -> if arg1 { all arg2Lines; arg3 arg2 = ; } else { arg3 arg1 = ; }
			// arg1 || arg2 -> if arg1 { arg3 arg1 = ; } else { all arg2Lines; arg3 arg2 = ; }
			
			ans2.push_back(arg1 + " if { ");
			if(containsAnd) {
				for(size_t j = 0; j < arg2Lines.size(); ++j) {
					ans2.push_back(arg2Lines[j]);
				}

				ans2.push_back(arg3 + " " + arg2 + " = ; ");
				ans2.push_back(" } ");
				ans2.push_back("else { ");
				ans2.push_back(arg3 + " " + arg1 + " = ; ");
				ans2.push_back(" } ");
			}
			else {
				// containsOr
				ans2.push_back(arg3 + " " + arg1 + " = ; ");
				ans2.push_back(" } ");
				ans2.push_back("else { ");
				for(size_t j = 0; j < arg2Lines.size(); ++j) {
					ans2.push_back(arg2Lines[j]);
				}
				ans2.push_back(arg3 + " " + arg2 + " = ; ");
				ans2.push_back(" } ");
			}
		}
	}
	
	return ans2;
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
			std::string tempVar = createTemp();

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
 * a && b:  if a is false, then result is false
 * a || b:  if a is true, then result is true
 * avoid situations like "if(b != 0 && a / b > 0)" to avoid dividing by 0
 *
 * What is the difference between
 * bool b1 = false;
 * bool b2 = expensiveFunc();
 * bool b3 = b1 && b2;
 * vs
 * bool b3 = false && expensiveFunc()
 * ?
 */
std::vector<std::string> addShortCircuiting(std::vector<std::string> &program) {
	// whenever find temp1 temp2 && = temp3, find last line where temp1 is used. add if statement (in postfix notation)
	std::vector<std::vector<std::string> > allWords;
	for(size_t i = 0; i < program.size(); ++i) {
		std::vector<std::string> words = getWords(program[i]);
		
		bool containsAnd = false;
		bool containsOr = false;
		for(size_t j = 0; j < words.size(); ++j) {
			if(words[j] == "&&") {
				containsAnd = true;	
			}
			else if(words[j] == "||") {
				containsOr = true;
			}
		}

		if(!(containsAnd || containsOr)) {
			allWords.push_back(words);
			continue;
		}


	}

	return program;
}

/**
 * Convert hard-to-implement operators like division "/" and modulo "%" into functions:
 * While +,-,*,==,!=,<,<=,&&,... are easy operators to implement,
 * (easy == can hand-craft Turing machine with only 3 tapes: 2 inputs + 1 output)
 * EDIT: Worried about 0 vs -0... make every operator except for ||, &&, ! into a function
 * Replace "/" and "%" into "!FUNC_LIB_div" and "!FUNC_LIB_mod" (EDIT: and similar)
 */
std::vector<std::string> replaceHardOps(std::vector<std::string> &program) {
	// only easy operators are || and &&
	std::unordered_map<std::string, std::string> rename {{"+", "add"}, {"-", "sub"}, {"*", "mul"}, {"/", "div"}, {"%", "mod"}, {"u-", "neg"}, {"<", "lt"}, {"<=", "leq"}, {">", "gt"}, {">=", "geq"}, {"==", "eq"}, {"!=", "neq"}, {"^", "eor"}};
	std::string funcPrefix = "!FUNC_LIB_";

	std::string tempLine;
	for(size_t i = 0; i < program.size(); ++i) {
		std::vector<std::string> words = getWords(program[i]);
		for(size_t j = 0; j < words.size(); ++j) {
			if(rename.find(words[j]) != rename.end()) {
				words[j] = funcPrefix + rename[words[j]];
			}

			tempLine.append(words[j]);
			tempLine.append(" ");
		}

	}
	
	std::vector<std::string> ans;
	ans.push_back(tempLine);
	
	return ans;
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
	
	// have to know variable declarations (process as you go to avoid "int x"in one function, then "bool x"in another function)
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
			std::unordered_set<std::string> validTypes {"int", "bool", "void"};
			if(validTypes.find(words[0]) != validTypes.end()) {
				std::string varName = words[1];
				varType[varName] = words[0];
					
				// add in types of parameters in function declaration
				if(words[words.size() - 1] == "{") {
					// add in every parameter to know what type it is
					for(size_t j = 3; validTypes.find(words[j]) != validTypes.end(); j += 3) {
						varType[words[j + 1]] = words[j];
					}
				}
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
					std::string tempVar = createTemp();

					fixedParams.push_back(tempVar);
						
					std::string desiredType = (param.size() > 13 && param.substr(param.size() - 13, 13) == " !VAR_LIB_MEM") ? "int" : varType[param];
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
			for(int j = opIndex; j < static_cast<int>(words.size()); ++j) {
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
 * 0 MEM x = -> !VAR_LIB_MEM[0] = x -> !FUNC_LIB_memset(0, x)  -> 0 x !FUNC_LIB_memset
 * x 0 MEM = -> x = !VAR_LIB_MEM[0]  ->  x = !FUNC_LIB_memget(0) -> 0 !FUNC_LIB_memget = x (meaning assign to variable x)
 * 0 MEM 1 MEM = -> MEM[0] = MEM[1] -> int temp; temp = MEM[1]; MEM[0] = temp; -> int temp; 1 !FUNC_LIB_memget = temp; 0 temp !FUNC_LIB_memset;
 */
std::vector<std::string> convertMemoryAccess(std::vector<std::string> &program) {
	// first convert all instances of "A B = = C ;" (meaning C = (A = B)"), to "A = B; C = A;"
	// -> "A B =; C A =;"
	std::vector<std::string> ans;

	for(size_t i = 0; i < program.size(); ++i) {
		std::vector<std::string> words = getWords(program[i]);
		if(words.size() >= 6 && words[words.size() - 4] == "=" && words[words.size() - 3] == "=") {
			int index = (int) words.size() - 5;
			std::string B = words[index];
			if(B == "!VAR_LIB_MEM") {
				--index;
				B = words[index] + " " + words[index + 1];
			}
			
			--index;
			std::string A = words[index];
			if(A == "!VAR_LIB_MEM") {
				--index;
				A = words[index] + " " + words[index + 1];
			}

			std::string C = words[words.size() - 2];

			ans.push_back(A + " " + B + " = ; ");
			ans.push_back(C + " " + A + " = ; ");
		}	
		else {
			ans.push_back(program[i]);
		}
	}

	// now, remove MEM from function parameters: aka simplify 0 MEM 1 MEM g   (meaning g(MEM[0], MEM[1]))
	// only applies if the function is not assignment (=)
	std::vector<std::string> ans2;
	std::vector<std::tuple<std::string, std::vector<std::string>, std::string> > funcs = getFunctions(program);

	std::unordered_set<std::string> funcNames;
	for(size_t i = 0; i < funcs.size(); ++i) {
		std::tuple<std::string, std::vector<std::string>, std::string> func = funcs[i];
		funcNames.insert(std::get<0>(func));
	}

	for(size_t i = 0; i < ans.size(); ++i) {
		std::vector<std::string> words = getWords(ans[i]);
		int funcIndex = -1;
		for(size_t j = 0; j < words.size(); ++j) {
			std::string word = words[j];
			if(word != "=" && funcNames.find(word) != funcNames.end()) {
				funcIndex = (int) j;
				break;
			}
		}

		if(funcIndex == -1) {
			ans2.push_back(ans[i]);
			continue;
		}

		// replace every parameter of the form "x MEM ... func" with a) "int temp; temp x MEM = ; temp... func" 
		for(int j = 0; j < funcIndex; ++j) {
			if(words[j] == "!VAR_LIB_MEM") {
				std::string index = words[j - 1];

				std::string tempVar = createTemp();
				ans2.push_back("int " + tempVar + " ; ");
				ans2.push_back(tempVar + " " + index + " !VAR_LIB_MEM = ; ");

				words[j - 1] = tempVar;
				words.erase(words.begin() + j);

				funcIndex--;
				j--;
			}
		}

		std::string tempLine;
		for(size_t j = 0; j < words.size(); ++j) {
			tempLine.append(words[j]);
			tempLine.append(" ");
		}
		ans2.push_back(tempLine);
	}


	std::vector<std::string> ans3;

	for(size_t i = 0; i < ans2.size(); ++i) {
		std::vector<std::string> words = getWords(ans2[i]);

		// 0 MEM x = -> !VAR_LIB_MEM[0] = x -> !FUNC_LIB_memset(0, x)  -> 0 x !FUNC_LIB_memset
		if(words.size() == 6 && words[1] == "!VAR_LIB_MEM" && words[3] == "!VAR_LIB_MEM" && words[4] == "=") {
			// case where sth like "MEM[0] = MEM[1];" 
			std::string tempVar = createTemp();
			std::string A = words[0];
			std::string B = words[2];

			ans3.push_back("int " + tempVar + " ; ");
			ans3.push_back(B + " " + "!FUNC_LIB_memget = " + tempVar + " ; ");
			ans3.push_back(A + " " + tempVar + " !FUNC_LIB_memset ; ");
		}
		else if(words.size() == 5 && words[2] == "!VAR_LIB_MEM" && words[3] == "=") {
			// var index MEM = ;
			std::string A = words[0];
			std::string B = words[1];
			ans3.push_back(B + " " + "!FUNC_LIB_memget = " + A + " ; ");
		}
		else if(words.size() == 5 && words[1] == "!VAR_LIB_MEM" && words[3] == "=") {
			// index MEM var = ;
			std::string A = words[0];
			std::string B = words[2];
			ans3.push_back(A + " " + B + " " + "!FUNC_LIB_memset ; ");
		}
		else if(words.size() == 5 && words[1] == "!VAR_LIB_MEM" && words[2] == "=") {
			// equal sign might be flipped: 
			// index MEM = var ; (meaning assign MEM[index] to var) -> var index memget = ;, just like above 
			std::string A = words[0];
			std::string B = words[3];
			//ans3.push_back(B + " " + A + " !FUNC_LIB_memget = ; ");
			ans3.push_back(A + " !FUNC_LIB_memget = " + B + " ; ");
		}
		else {
			ans3.push_back(ans2[i]);
		}
	}

	std::vector<std::string> ans4;
	std::string tempLine;
	for(size_t i = 0; i < ans3.size(); ++i) {
		std::vector<std::string> words = getWords(ans3[i]);
		for(size_t j = 0; j < words.size(); ++j) {
			tempLine.append(words[j]);
			tempLine.append(" ");
		}
	}

	ans4.push_back(tempLine);
	return ans4;
}

/**
 * Decrease number of temp variables by reusing older, unused ones
 * For each function
 */
std::vector<std::string> reduceTemps(std::vector<std::string> &program) {
	// get functions
	std::vector<std::vector<std::string> > funcs;
	std::unordered_set<std::string> validTypes {"void", "int", "bool"};

	std::vector<std::string> allWords;
	for(size_t i = 0; i < program.size(); ++i) {
		std::vector<std::string> words = getWords(program[i]);
		for(size_t j = 0; j < words.size(); ++j) {
			allWords.push_back(words[j]);
		}
	}

	for(size_t i = 0; i < allWords.size(); ++i) {
		if(allWords[i] == "{") {
			// is function declaration
			std::vector<std::string> func;
			int closeBrace = findOpposite(allWords, i);

			for(int j = (int) i + 1; j < closeBrace; ++j) {
				func.push_back(allWords[j]);
			}

			// reformat from words to lines
			std::string tempLine;
			for(size_t j = 0; j < func.size(); ++j) {
				tempLine.append(func[j]);
				tempLine.append(" ");	
			}
			
			std::vector<std::string> funcProgram {tempLine};
			funcProgram = formatProgram(funcProgram);
			funcs.push_back(funcProgram);

			i = static_cast<size_t>(closeBrace);
		}
	}

	// reduce for each function
	for(size_t i = 0; i < funcs.size(); ++i) {
		std::vector<std::string> funcProgram = funcs[i];
		// mark where first and last occurrences of where temp variables are used (excluding declaration)
		std::unordered_map<int, int> occur1;
		std::unordered_map<int, int> occur2;
		std::unordered_set<int> tempNums;

		// start from 1, cuz ignore function declaration
		for(size_t j = 0; j < funcProgram.size(); ++j) {
			std::vector<std::string> words = getWords(funcProgram[j]);
			
			// ignore variable declarations
			if(words.size() == 3 && validTypes.find(words[0]) != validTypes.end()) {
				continue;
			}

			for(size_t k = 0; k < words.size(); ++k) {
				if(words[k].substr(0, 9) == "!VAR_TEMP") {
					int numStartIndex = 14; 		// cuz !VAR_TEMP_temp9  <- 9 starts at index 14
					int num = std::stoi(words[k].substr(numStartIndex, words[k].size() - numStartIndex));

					tempNums.insert(num);

					if(occur1.find(num) == occur1.end()) {
						// record on which line this temp variable first appeared
						occur1[num] = j;
					}
					// record on which line this temp variable last appears (we're traversing lines from top to bottom with var j)
					occur2[num] = j;
				}
			}
		}

		// describes when intervals start and end, to find maximum overlap of intervals: +1 for beginning, -1 at (end + 1)
		std::vector<int> schedule(funcProgram.size() + 1, 0);

		for(int tempNum : tempNums) {
			int time1 = occur1[tempNum];
			int time2 = occur2[tempNum];
			schedule[time1] += 1;
			schedule[time2 + 1] -= 1;
		}

		std::vector<int> busyness(schedule.size(), 0);
		busyness[0] = schedule[0];
		for(size_t j = 1; j < busyness.size(); ++j) {
			busyness[j] = busyness[j - 1] + schedule[j];
		}

		int busiest = 0;
		for(size_t j = 0; j < busyness.size(); ++j) {
			busiest = std::max(busyness[j], busiest);
		}

		// reduce from <tempNums.size()> number of temp variables, to <busiest> number of temp variables
		// greedy it
		int numNewTemp = busiest;
		//std::cout << "yea for func " << i << " numNewTemp = " << numNewTemp << std::endl;
		std::unordered_map<int, int> remap;
		std::vector<bool> currBusy(numNewTemp, false);
		
		// map from time to start / end of interval
		std::map<int, std::vector<int> > events;
		for(std::unordered_map<int, int>::iterator it = occur1.begin(); it != occur1.end(); ++it) {
			int tempNum = it->first;
			int lineNum = it->second;
			events[lineNum].push_back(tempNum);
		}

		for(std::unordered_map<int, int>::iterator it = occur2.begin(); it != occur1.end(); ++it) {
			int tempNum = it->first;
			int lineNum = it->second;
			events[lineNum].push_back(tempNum);
		}

		std::unordered_set<int> visited;
		// now find assignment of new temp variables
		for(std::map<int, std::vector<int> >::iterator it = events.begin(); it != events.end(); ++it) {
			//int lineNum = it->first;
			std::vector<int> temps = it->second;

			// handle all the starting intervals first, then the closing intervals (cuz technically the closing happens 1 timestep afterwards)
			std::vector<int> A;
			std::vector<int> B;
			for(int tempNum : temps) {
				if(visited.find(tempNum) == visited.end()) {
					A.push_back(tempNum);	
				}
				else {
					B.push_back(tempNum);
				}
			}

			for(int tempNum : A) {
				visited.insert(tempNum);

				// starting interval; assign first availble new temp variable
				int newTemp = 0;
				for(size_t j = 0; j < currBusy.size(); ++j) {
					if(!currBusy[j]) {
						newTemp = j;
						break;
					}
				}

				remap[tempNum] = newTemp;
				currBusy[newTemp] = true;

				//std::cout << "remap " << tempNum << " to " << newTemp << std::endl;
			}
			
			for(int tempNum : B) {
				visited.insert(tempNum);
				// ending an interval; the new temp variable not busy anymore
				int newTemp = remap[tempNum];
				currBusy[newTemp] = false;
			}

		}

		// now actually aasign / replace
		// don't worry about double declarations: function moveUpDeclaration will take care
		for(size_t j = 0; j < funcProgram.size(); ++j) {
			std::vector<std::string> words = getWords(funcProgram[j]);
			for(size_t k = 0; k < words.size(); ++k) {
				if(words[k].substr(0, 9) == "!VAR_TEMP") {
					int numStartIndex = 14; 		// cuz !VAR_TEMP_temp9  <- 9 starts at index 14
					int num = std::stoi(words[k].substr(numStartIndex, words[k].size() - numStartIndex));

					int replaceNum = remap[num];

					if(replaceNum != num) {
						words[k] = "!VAR_TEMP_temp" + std::to_string(replaceNum);
					}
				}
			}

			std::string tempLine;
			for(size_t k = 0; k < words.size(); ++k) {
				tempLine.append(words[k]);
				tempLine.append(" ");
			}
			funcProgram[j] = tempLine;
		}

		funcs[i] = funcProgram;
	}
	
	// convert all funcs into readable program
	// don't forget function headers
	
	// identify function headers;
	std::vector<std::string> funcDeclarations;
	for(size_t i = 0; i < program.size(); ++i) {
		std::vector<std::string> words = getWords(program[i]);
		if(validTypes.find(words[0]) != validTypes.end() && words[words.size() - 1] == "{") {
			funcDeclarations.push_back(program[i]);
		}

	}

	std::vector<std::string> ans;

	for(size_t i = 0; i < funcs.size(); ++i) {
		ans.push_back(funcDeclarations[i]);

		std::vector<std::string> funcProgram = funcs[i];
		for(size_t j = 0; j < funcProgram.size(); ++j) {
			ans.push_back(funcProgram[j]);
		}

		ans.push_back("}");
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
 * Change return statements to involve RAX register/tape
 * only for statements like "x return ;", not "return ;"
 */
std::vector<std::string> explicitReturn(std::vector<std::string> &program) {
	std::vector<std::string> ans;
	for(size_t i = 0; i < program.size(); ++i) {
		std::vector<std::string> words = getWords(program[i]);
		if(words.size() >= 3 && words[words.size() - 2] == "return") {
			std::string var = words[words.size() - 3];
			ans.push_back("!TAPE_RAX " + var + " = ; ");
			ans.push_back("return ; ");
		}
		else {
			ans.push_back(program[i]);
		}
	}

	std::vector<std::string> ans2;
	std::string temp;
	for(size_t i = 0; i < ans.size(); ++i) {
		std::vector<std::string> words = getWords(ans[i]);
		for(size_t j = 0; j < words.size(); ++j) {
			temp.append(words[j]);
			temp.append(" ");
		}
	}

	ans2.push_back(temp);
	return ans2;
}

/**
 * Add push and pop calls around functions
 */
std::vector<std::string> pushAndPop(std::vector<std::string> &program) {
	std::vector<std::string> ans;

	std::vector<std::vector<std::string> > allWords;
	for(size_t i = 0; i < program.size(); ++i) {
		allWords.push_back(getWords(program[i]));
	}

	std::unordered_set<std::string> validTypes {"void", "int", "bool"};
	for(size_t i = 0; i < program.size(); ++i) {
		std::vector<std::string> words = allWords[i];
		
		// pop parameters when starting function
		// find function declaration
		if(validTypes.find(words[0]) != validTypes.end() && words[words.size() - 1] == "{") {
			std::string funcName = words[1];

			std::vector<std::string> paramTypes;
			std::vector<std::string> paramNames;

			int openParen = findNext(words, "(", 1);
			//int closeParen = findOpposite(words, openParen);
			
			int index = openParen + 1;
			while(validTypes.find(words[index]) != validTypes.end()) {
				paramTypes.push_back(words[index]);
				paramNames.push_back(words[index + 1]);
				index += 3; 		// type + name + comma
			}

			// if next line has "declare x y z temp ;", then add parameters to this list: "declare param1 param2 x y z temp ;"
			// edit: changed moveUpDeclaration to always have a "declare" line per function
			if(i + 1 < program.size() && allWords[i + 1][0] == "declare") {
				// add parameter names to list of declared variables
				allWords[i + 1].insert(allWords[i + 1].begin() + 1, paramNames.begin(), paramNames.end());
			}

			ans.push_back(words[0] + " " + funcName + " " + "{");
			for(size_t j = 0; j < paramTypes.size(); ++j) {
				ans.push_back("pop !TAPE_PARAMS !TAPE_tape" + std::to_string(j) + " ; ");
			}
		}
		else if((words.size() >= 2 && words[words.size() - 2].substr(0, 5) == "!FUNC") || (words.size() >= 4 && words[words.size() - 4].substr(0, 5) == "!FUNC")) {
			int funcIndex = (words.size() >= 2 && words[words.size() - 2].substr(0, 5) == "!FUNC") ? (int) words.size() - 2 : (int) words.size() - 4;
			
			for(int j = funcIndex - 1; j >= 0; --j) {
				ans.push_back("push " + words[j] + " " + "!TAPE_PARAMS" + " ; ");
			}
			
			ans.push_back("call " + words[funcIndex] + " " + ";" + " ");
			if(funcIndex == static_cast<int>(words.size()) - 4) {
				ans.push_back("pop !TAPE_RAX " + words[words.size() - 2] + " ; ");	
			}
		}
		else {
			std::string tempLine;
			for(std::string word : allWords[i]) {
				tempLine.append(word);
				tempLine.append(" ");
			}

			ans.push_back(tempLine);
		}
	}
	
	std::vector<std::string> ans2;
	std::string tempLine;
	for(size_t i = 0; i < ans.size(); ++i) {
		std::vector<std::string> words = getWords(ans[i]);
		for(size_t j = 0; j < words.size(); ++j) {
			tempLine.append(words[j]);
			tempLine.append(" ");
		}
	}
	ans2.push_back(tempLine);

	return ans2;
}

/**
 * Convert variable names, like parameters and !VAR_TEMP_temp5, into tape indices: !TAPE_tape0, !TAPE_tape1,...
 * as referenced by the "declare" line in each function
 */
std::vector<std::string> remapVariableNamesToTapes(std::vector<std::string> &program) {
	std::vector<std::string> allWords;
	for(size_t i = 0; i < program.size(); ++i) {
		std::vector<std::string> words = getWords(program[i]);
		allWords.insert(allWords.end(), words.begin(), words.end());
	}

	int openBrace = findNext(allWords, "{", 0);

	while(0 <= openBrace && static_cast<size_t>(openBrace) < allWords.size()) {
		int closeBrace = findOpposite(allWords, openBrace);

		// find declaration line
		int declareLine = openBrace;
		while(allWords[declareLine] != "declare") {
			++declareLine;
		}
		
		int semicolon = findNext(allWords, ";", declareLine);
		int numVar = semicolon - declareLine - 1; 	// ex: declare a b ;  : 3 - 0 - 1 = 2 variables

		// remap variables in declare line, to "!TAPE_tape0" and so on
		std::unordered_map<std::string, std::string> mapping;
		for(int i = 0; i < numVar; ++i) {
			std::string varName = allWords[declareLine + i + 1];
			mapping[varName] = "!TAPE_tape" + std::to_string(i);
		}

		// rename the variables
		for(int i = semicolon + 1; i < closeBrace; ++i) {
			std::string word = allWords[i];
			if(mapping.find(word) != mapping.end()) {
				allWords[i] = mapping[word];
			}
		}

		// clean out the declare line
		for(int i = declareLine; i <= semicolon; ++i) {
			allWords[i] = "nop";
		}

		openBrace = findNext(allWords, "{", closeBrace + 1);
	}

	std::vector<std::string> ans;
	std::string tempLine;
	for(size_t i = 0; i < allWords.size(); ++i) {
		std::string word = allWords[i];
		if(word != "nop") {
			tempLine.append(word);
			tempLine.append(" ");
		}
	}

	ans.push_back(tempLine);
	return ans;
}

/**
 * Helper function to addJumpsAndLineNumbers. Convert statements for if and if-else
 */
std::vector<std::string> ifElseToJump(std::vector<std::string> &program, std::vector<std::string> &allWords, std::vector<int> &wordLines) {
	for(size_t i = 0; i < allWords.size(); ++i) {
		if(i + 2 < allWords.size() && allWords[i + 1] == "if") {
			// check if it's just the if case
			int openBrace = findNext(allWords, "{", i + 1);	
			int closeBrace = findOpposite(allWords, openBrace);

			if(allWords[closeBrace + 1] != "else") {
				// if scenario	

				// change "temp if {" line into "jf temp <line number of closing brace>" (jf stands for jump if temp is false)
				int lineNum1 = wordLines[openBrace];
				int lineNum2 = wordLines[closeBrace];
				program[lineNum1] = "jf " + allWords[i] + " " + std::to_string(lineNum2) + " ; ";

				if(lineNum2 == 308) {
					std::cout << "opt1" << std::endl;
				}

				// change "}" into "nop"
				program[lineNum2] = "nop ; ";
			}
			else {
				// if-else scenario

				int elseIndex = closeBrace + 1;
				int closeBrace2 = findOpposite(allWords, elseIndex + 1);

				int lineNum1 = wordLines[openBrace];
				int lineNum2 = wordLines[closeBrace];
				int lineNum3 = wordLines[elseIndex];
				int lineNum4 = wordLines[closeBrace2];

				if(lineNum3 == 308) {
					std::cout << "opt2" << std::endl;
				}

				program[lineNum1] = "jf " + allWords[i] + " " + std::to_string(lineNum3) + " ; ";
				program[lineNum2] = "jmp " + std::to_string(lineNum4) + " ; ";
				program[lineNum3] = "nop ; ";
				program[lineNum4] = "nop ; ";
			}
		}
	}
	
	return program;	
}

/**
 * Helper function to addJumpsAndLineNumbers. Convert while loops + break + continue
 */
std::vector<std::string> whileToJump(std::vector<std::string> &program, std::vector<std::string> &allWords, std::vector<int> &wordLines) {
	// stack indicating where current while loop "{" occurs index-wise in allWords
	std::vector<int> openWhile;
	
	// map from open "{" to close "}" of while loops (and vice-versa)
	std::unordered_map<int, int> braces;
	std::unordered_map<int, int> braces2;
	
	for(size_t i = 1; i < allWords.size(); ++i) {
		if(allWords[i] == "while") {
			int openBrace = findNext(allWords, "{", i);
			int closeBrace = findOpposite(allWords, openBrace);
			
			openWhile.push_back(openBrace);
			braces[openBrace] = closeBrace;
			braces2[closeBrace] = openBrace;	
		}
		else if(allWords[i] == "}" && braces2.find(i) != braces2.end()) {
			// if this is a closing brace matching an active while loop, close it (by popping from active stack)
			openWhile.pop_back();
		}
		else if(allWords[i] == "break") {
			// this break belongs to the most recently active while
			int openBrace = openWhile[openWhile.size() - 1];
			int closeBrace = braces[openBrace];	

			// jump to line after closeBrace: we manually added a nop after each one before in addJumpsAndLineNumbers
			int fromLine = wordLines[i];
			int toLine = 1 + wordLines[closeBrace];
			program[fromLine] = "jmp " + std::to_string(toLine) + " ; ";
		}
		else if(allWords[i] == "continue") {
			// this break belongs to the most recently active while
			int openBrace = openWhile[openWhile.size() - 1];
			//int closeBrace = braces[openBrace];	

			// jump to line with openBrace
			int fromLine = wordLines[i];
			int toLine = wordLines[openBrace];
			program[fromLine] = "jmp " + std::to_string(toLine) + " ; ";
		}
	}

	// we have converted the break + continues
	// now change the while loop + open brace line and the closing brace line
	for(std::unordered_map<int, int>::iterator it = braces.begin(); it != braces.end(); ++it) {
		int openBrace = it->first;
		int closeBrace = it->second;
		
		int openLine = wordLines[openBrace];
		int closeLine = wordLines[closeBrace];
		
		// line with open brace becomes nop, line with close brace jumps to line with open brace
		program[openLine] = "nop ; ";
		program[closeLine] = "jmp " + std::to_string(openLine) + " ; ";
	}
	
	return program;
}

/**
 * Helper function to addJumpsAndLineNumbers. Convert function calls to "jmp <line #>" and
 * function declarations to "nop"
 */
std::vector<std::string> funcToJump(std::vector<std::string> &program, std::vector<std::string> &allWords, std::vector<int> &wordLines) {
	// maintain map of what lines function declarations are
	std::unordered_set<std::string> validTypes {"void", "int", "bool"};
	std::unordered_map<std::string, int> funcLines;

	int lineNum = 0;
	for(size_t i = 1; i < allWords.size(); ++i) {
		std::string word = allWords[i];
		if(word == ";" || word == "{" || word == "}") {
			++lineNum;
			continue;
		}

		if(i + 2 < allWords.size() && validTypes.find(word) != validTypes.end() && allWords[i + 1].substr(0, 5) == "!FUNC" && allWords[i + 2] == "{") {
			std::string funcName = allWords[i + 1];
			funcLines[funcName] = lineNum;
		}
	}
	
	// replace each function declaration line with a "nop"
	for(std::unordered_map<std::string, int>::iterator it = funcLines.begin(); it != funcLines.end(); ++it) {
		int funcLine = it->second;
		program[funcLine] = "nop ; ";

		//std::cout << "func " << it->first << " on line " << funcLine << std::endl;
	}

	// replace each closing brace with "nop" : the only remaining closing braces were from function declarations
	for(size_t i = 0; i < program.size(); ++i) {
		std::string line = program[i];
		trim(line);
		if(line == "}") {
			program[i] = "nop ; ";
		}
	}

	// replace each occurrence of "call func", with "jmp <line #>" (if it exists)
	// if it doesn't exist, leave alone, as will take care of it when actual generate Turing Machine
	for(size_t i = 0; i + 1 < allWords.size(); ++i) {
		if(allWords[i] == "call" && allWords[i + 1].substr(0, 5) == "!FUNC") {
			std::string funcName = allWords[i + 1];
			if(funcLines.find(funcName) != funcLines.end()) {
				int fromLine = wordLines[i];
				int toLine = funcLines[funcName];
				program[fromLine] = "call " + std::to_string(toLine) + " ; ";	
			}
		}	
	}

	return program;
}

/**
 * Convert if/else/call/while loops into jump statements / line numbers
 * Fill in empty lines with "nop" : no-ops aka no-operations aka do nothing
 */
std::vector<std::string> addJumpsAndLineNumbers(std::vector<std::string> &program) {
	/* if statements:
	 *
	 * 1.if(temp) {
	 * 2.	A
	 * 3.}
	 * 4.B 
	 *
	 * translated to 
	 *
	 * 1.jf temp 3 	// if temp == false, go to nop on line 3
	 * 2.A
	 * 3.nop 		// the closing brace } becomes a nop 
	 * 4.B
	 */ 

	/* if-else statements:
	 *
	 * 1. if(temp) {
	 * 2. 	A
	 * 3. }
	 * 4. else {
	 * 5. 	B
	 * 6. }
	 * 7. C
	 *
	 * translated to
	 *
	 * 1. jf temp 4 	// if temp == false, go to nop on line 4
	 * 2. A
	 * 3. jmp 6 		// jump to nop on line 6
	 * 4. nop
	 * 5. B
	 * 6. nop
	 * 7. C
	 */

	/* while loops:
	 *
	 * 1. while(true) {
	 * 2. 	A
	 * 3. }
	 * 4. B
	 *
	 * translated to
	 *
	 * 1. nop
	 * 2. A
	 * 3. jmp 1
	 * 4. nop 	// the closing brace becomes instructions 3 and 4
	 * 5. B
	 */

	/* break + continue
	 *
	 * 1. while(true) {
	 * 2. A
	 * 3. break;
	 * 4. B
	 * 5. continue;
	 * 6. C
	 * 7. }
	 *
	 * translated to
	 *
	 * 1. nop
	 * 2. A
	 * 3. jmp 8
	 * 4. B
	 * 5. jmp 1
	 * 6. C
	 * 7. jmp 1
	 * 8. nop 		// closing  brace becomes instructions 7 and 8
	 */
	
	/* function call
	 * 
	 * 1. int f {
	 *
	 * 3. call f;
	 *
	 * translated to
	 *
	 * 1. nop
	 *
	 * 3. call 1 		// call <number> first pushes all variable tapes; then jump <number>
	 *
	 *
	 */

	// first, add a placeholder jump as the very first statement: jumps to main (to a copy of program)
	std::vector<std::string> p2 {"jmp # ; "};
	p2.insert(p2.end(), program.begin(), program.end());

	// convert to words
	std::vector<std::string> allWords;
	for(size_t i = 0; i < p2.size(); ++i) {
		std::vector<std::string> words = getWords(p2[i]);
		allWords.insert(allWords.end(), words.begin(), words.end());
	}

	std::vector<std::string> allWords2;

	// end of while loop closing brace gets translated to two lines (jump and nop), so add nop. 
	// all other conversions don't change number of lines. add in nop so translations later in 
	// this function aren't as painful
	
	// allWords2 first gets filled in words backwards
	for(int i = allWords.size() - 1; i >= 0; --i) {
		std::string word = allWords[i];

		if(word == "}") {
			int openBrace = findOpposite(allWords, i);
			if(allWords[openBrace - 1] == "while") {
				// if i is index of closing brace of while loop, fill in "nop ; " backwards
				allWords2.push_back(";");
				allWords2.push_back("nop");
			}
		}

		allWords2.push_back(word);
	}

	// now reverse words into correct order
	std::reverse(allWords2.begin(), allWords2.end());

	// find which line main() starts on
	for(size_t i = 1; i < p2.size(); ++i) {
		std::vector<std::string> words = getWords(p2[i]);

		if(words.size() == 3 && words[0] == "int" && words[1] == "!FUNC_USER_main" && words[2] == "{") {
			p2[0] = "jmp " + std::to_string(i) + " ; ";
			allWords2[1] = std::to_string(i);
		}
	}

	// maintain lookup of what line each word belongs in
	std::vector<int> wordLines(allWords2.size(), -1);

	if(wordLines.size() > 0) {
		wordLines[0] = 0;
	}

	int lineNum = 0;
	for(size_t i = 1; i < allWords2.size(); ++i) {
		std::string word = allWords2[i];
		if(wordLines[i] == -1) {
			wordLines[i] = wordLines[i - 1];
		}

		if(word == ";" || word == "{" || word == "}") {
			++lineNum;
			if(i + 1 < allWords2.size()) {
				wordLines[i + 1] = 1 + wordLines[i]; 
			}
			continue;
		}
	}

	// now get back program in line form
	std::vector<std::string> p3;
	std::string tempLine;
	for(size_t i = 0; i < allWords2.size(); ++i) {
		std::string word = allWords2[i];
		if(i == 0) {
			tempLine.append(word);
			tempLine.append(" ");
		}
		else {
			if(wordLines[i] == wordLines[i-1]) { 	// if words belong to same line, append to same line
				tempLine.append(word);
				tempLine.append(" ");
			}
			else {
				p3.push_back(tempLine);
				tempLine.clear();
				tempLine.append(word);
				tempLine.append(" ");
			}
		}
	}

	if(tempLine.size() > 0) {
		p3.push_back(tempLine);
	}

	// do translations! changes appear in p3, the line form of the program. view above long comments. match based on syntax (especially if vs if-else)
	p3 = ifElseToJump(p3, allWords2, wordLines);
	p3 = whileToJump(p3, allWords2, wordLines);
	p3 = funcToJump(p3, allWords2, wordLines);

	// convert p3 (line program) into bunch of words to reformat
	std::vector<std::string> ans;
	tempLine.clear();
	for(size_t i = 0; i < p3.size(); ++i) {
		std::string line = p3[i];
		std::vector<std::string> words = getWords(line);
		for(size_t j = 0; j < words.size(); ++j) {
			tempLine.append(words[j]);
			tempLine.append(" ");
		}
	}

	ans.push_back(tempLine);

	//std::cout << "Done with jumps" << std::endl;

	return ans;
}

std::vector<std::string> sourceToAssembly(const std::vector<std::string> &program) {
	std::vector<std::string> modifiedProgram = program;

	modifiedProgram = removeComments(modifiedProgram);
	modifiedProgram = addSpaceAroundOperators(modifiedProgram);
	modifiedProgram = formatProgram(modifiedProgram);

	modifiedProgram = renameUserVariables(modifiedProgram);
	modifiedProgram = renameBuiltInVariables(modifiedProgram);
	modifiedProgram = formatProgram(modifiedProgram);

	modifiedProgram = addLibraryFunctions(modifiedProgram);

	modifiedProgram = forToWhile(modifiedProgram);
	modifiedProgram = convertElif(modifiedProgram);
	modifiedProgram = formatProgram(modifiedProgram);

	modifiedProgram = changeWhileLoops(modifiedProgram);
	modifiedProgram = formatProgram(modifiedProgram);

	modifiedProgram = voidReturns(modifiedProgram);
	modifiedProgram = formatProgram(modifiedProgram);

	modifiedProgram = parenthesisReturn(modifiedProgram);
	modifiedProgram = formatProgram(modifiedProgram);

	modifiedProgram = assignDefaultToDeclaration(modifiedProgram);
	modifiedProgram = formatProgram(modifiedProgram);

	modifiedProgram = simplifyExpressions(modifiedProgram);
	modifiedProgram = formatProgram(modifiedProgram);

	modifiedProgram = convertSpecialAssignment(modifiedProgram);
	modifiedProgram = formatProgram(modifiedProgram); 

	modifiedProgram = replaceHardOps(modifiedProgram);
	modifiedProgram = formatProgram(modifiedProgram);

	modifiedProgram = paramsToTemp(modifiedProgram);
	modifiedProgram = formatProgram(modifiedProgram);

	modifiedProgram = convertMemoryAccess(modifiedProgram);
	modifiedProgram = formatProgram(modifiedProgram);

	modifiedProgram = reduceTemps(modifiedProgram);
	modifiedProgram = formatProgram(modifiedProgram);

	modifiedProgram = moveUpDeclaration(modifiedProgram);
	modifiedProgram = formatProgram(modifiedProgram);

	modifiedProgram = explicitReturn(modifiedProgram);
	modifiedProgram = formatProgram(modifiedProgram);

	modifiedProgram = pushAndPop(modifiedProgram);
	modifiedProgram = formatProgram(modifiedProgram);

	modifiedProgram = remapVariableNamesToTapes(modifiedProgram);
	modifiedProgram = formatProgram(modifiedProgram);

	modifiedProgram = addJumpsAndLineNumbers(modifiedProgram);
	modifiedProgram = formatProgram(modifiedProgram);

	return modifiedProgram;
}

#if SINGLE_FILE==1
int main() {
	std::string fileName = "../programs/example.cpp";
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
		std::cout << "Initial compilation OK" << std::endl;
	}

	std::vector<std::string> program;
	std::string line;
	while(std::getline(file, line)) {
		program.push_back(line);
	}
	
	file.close();
	
	std::vector<std::string> transformedProgram = sourceToAssembly(program);
	//printPrettyProgram(transformedProgram);
	printProgram(transformedProgram);

	bool writeAssemblyToFile = true;
	if(writeAssemblyToFile) {
		std::string outFileName = "assembly.txt";
		std::ofstream outFile(outFileName);

		if(outFile.is_open()) {
			for(size_t i = 0; i < transformedProgram.size(); ++i) {
				outFile << transformedProgram[i] << std::endl;
			}
			outFile.close();
		}
		else {
			std::cout << "Unable to open writing file " << outFileName << std::endl;
		}
	}

	return 0;
}
#endif
