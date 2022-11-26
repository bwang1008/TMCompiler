#ifndef UNITTESTS_HPP
#define UNITTESTS_HPP

#include <exception>
#include <iostream>
#include <list>
#include <map>
#include <string>
#include <utility>

#define REGISTER_TEST(FUNCTION_NAME) \
	int DUMMY_REGISTER_ ## FUNCTION_NAME = ( NAMES_TO_FUNCTIONS[#FUNCTION_NAME] = &FUNCTION_NAME , 0)

// declare the function, register it, then define
#define TEST_CASE(FUNCTION_NAME) \
	bool FUNCTION_NAME () ; \
	REGISTER_TEST(FUNCTION_NAME) ; \
	bool FUNCTION_NAME ()

using FUNCTION_POINTER = bool (*)(void);
std::map<std::string, FUNCTION_POINTER> NAMES_TO_FUNCTIONS;

struct TestCasesSummary {
	int numPassed;
	int numFailed;
	int numErrored;
	std::list<std::string> failedFunctions;
	std::list<std::pair<std::string, std::string> > erroredFunctions;

	TestCasesSummary(): numPassed{0}, numFailed{0}, numErrored{0}, failedFunctions(), erroredFunctions() {

	}
};

TestCasesSummary runAllTestCases() {
	TestCasesSummary summary;
	for(std::map<std::string, FUNCTION_POINTER>::iterator it = NAMES_TO_FUNCTIONS.begin(); it != NAMES_TO_FUNCTIONS.end(); ++it) {
		std::string functionName = it->first;
		FUNCTION_POINTER func = it->second;

		try {
			bool passed = func();
			if(passed) {
				++summary.numPassed;
			}
			else {
				++summary.numFailed;
				summary.failedFunctions.push_back(functionName);
			}
		}
		catch(const std::exception &e) {
			++summary.numErrored;
			summary.erroredFunctions.push_back(std::make_pair(functionName, e.what()));
		}
	}

	return summary;
}

int main() {
	// NAMES_TO_FUNCTIONS initialized and filled by this point 
	TestCasesSummary summary = runAllTestCases();

	const std::string outputSeparator = "----------------------------------------";
	int numTestCases = summary.numPassed + summary.numFailed + summary.numErrored;

	if(numTestCases == 0) {
		std::cout << "There are 0 test cases." << std::endl;
		return 0;
	}

	std::cout << outputSeparator << std::endl;

	if(summary.numPassed == numTestCases) {
		std::cout << "All " << numTestCases << " test cases passed!" << std::endl;
		return 0;
	}

	if(summary.erroredFunctions.size() > 0) {
		for(std::pair<std::string, std::string> result : summary.erroredFunctions) {
			std::cout << result.first << " errored: " << std::endl << result.second << std::endl;
		}

		std::cout << outputSeparator << std::endl;
	}

	if(summary.failedFunctions.size() > 0) {
		for(std::string result : summary.failedFunctions) {
			std::cout << result << " failed" << std::endl;
		}

		std::cout << outputSeparator << std::endl;
	}

	std::cout << "Number of tests passed      : " << summary.numPassed << std::endl;
	std::cout << "Number of tests failed      : " << summary.numFailed << std::endl;
	std::cout << "Number of tests errored     : " << summary.numErrored << std::endl;
	std::cout << outputSeparator << std::endl;

	return 0;
}

#endif
