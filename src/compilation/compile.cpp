// takes in a file path for higher-level code and turns into a TM file

#include <iostream>		// std::cout, std::endl	
#include <fstream>		// std::ifstream
#include <string>		// std::string
#include <vector>		// std::vector	

// ideas: step 1 : convert for to while
// step 2: convert while to jump, including break/continue
// 

std::vector<char> translate0(std::vector<std::string> programLines) {
	std::vector<char> ans;

	for(const std::string &line : programLines) {
		for(const char &c : line) {
			if(c != ' ' && c != '\n') {
				ans.push_back(c);
			}
		}
	}

	return ans;
}

int main() {
	std::string fileFullName = "example.hltm";
	std::ifstream myFile(fileFullName);

	if(!myFile.is_open()) {
		std::cout << "Could not open file " << fileFullName << std::endl;
		return 0;
	}
	
	std::vector<std::string> programLines;
	std::string programLine;
	while(std::getline(myFile, programLine)) {
		programLines.push_back(programLine);
	}

	std::vector<char> ans = translate0(programLines);

	std::cout << "[";
	for(char c : ans) {
		std::cout << c;
	}

	std::cout << "]" << std::endl;
	return 0;
}
