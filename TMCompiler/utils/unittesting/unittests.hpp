#ifndef UNITTESTS_HPP
#define UNITTESTS_HPP

#include <exception>
#include <iostream>
#include <list>
#include <map>
#include <string>
#include <utility>

int numPassingAsserts = 0;
int numFailingAsserts = 0;

// assigns function to global map. runs before main by using comma operator
#define REGISTER_TEST(FUNCTION_NAME) \
	int DUMMY_REGISTER_ ## FUNCTION_NAME = ( NAMES_TO_FUNCTIONS[#FUNCTION_NAME] = &FUNCTION_NAME , 0)

// declare the function, register it, then define
#define TEST_CASE(FUNCTION_NAME) \
	void FUNCTION_NAME () ; \
	REGISTER_TEST(FUNCTION_NAME) ; \
	void FUNCTION_NAME ()

#define FAIL_ASSERT() \
	std::cout << "Assert failed in " << __FILE__ << " in " << static_cast<const char*>(__func__) << "()" << " on line " << __LINE__ << std::endl;
	

#define ASSERT(BOOL_EXPR) \
	if( ! (BOOL_EXPR) ) { \
		++numFailingAsserts; \
		FAIL_ASSERT(); \
	} \
	else \
		++numPassingAsserts
		
using FUNCTION_POINTER = void (*)(void);
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
			int prevFailingAsserts = numFailingAsserts;
			func();
			bool passed = (numFailingAsserts == prevFailingAsserts);

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
			summary.erroredFunctions.emplace_back(functionName, e.what());
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
		std::cout << "All " << numPassingAsserts << " asserts passed!" << std::endl;
		std::cout << outputSeparator << std::endl;
		return 0;
	}

	if(!summary.erroredFunctions.empty()) {
		for(const std::pair<std::string, std::string>& result : summary.erroredFunctions) {
			std::cout << result.first << " errored: " << std::endl << result.second << std::endl;
		}

		std::cout << outputSeparator << std::endl;
	}

	if(!summary.failedFunctions.empty()) {
		for(const std::string& result : summary.failedFunctions) {
			std::cout << result << " failed" << std::endl;
		}

		std::cout << outputSeparator << std::endl;
	}

	std::cout << "Number of tests passed      : " << summary.numPassed << std::endl;
	std::cout << "Number of tests failed      : " << summary.numFailed << std::endl;
	std::cout << "Number of tests errored     : " << summary.numErrored << std::endl;
	std::cout << outputSeparator << std::endl;

	std::cout << "Number of passing asserts   : " << numPassingAsserts << std::endl;
	std::cout << "Number of failing asserts   : " << numFailingAsserts << std::endl;
	std::cout << outputSeparator << std::endl;

	return 0;
}

#endif
