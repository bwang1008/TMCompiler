#include "unittests.hpp"

#include <iostream>	 // std::cout, std::endl
#include <string>	 // std::string
#include <utility>	 // std::pair

int numPassingAsserts = 0;
int numFailingAsserts = 0;

std::map<std::string, FunctionPointer> NAMES_TO_FUNCTIONS;

void ASSERT(const bool test_expression) {
	if(!test_expression) {
		++numFailingAsserts;
		FAIL_ASSERT();
	} else {
		++numPassingAsserts;
	}
}

auto runAllTestCases() -> TestCasesSummary {
	TestCasesSummary summary;

	for(const std::pair<const std::string, FunctionPointer>& it :
		NAMES_TO_FUNCTIONS) {
		const std::string function_name = it.first;
		const FunctionPointer func = it.second;

		try {
			const int prevFailingAsserts = numFailingAsserts;
			func();
			const bool passed = (numFailingAsserts == prevFailingAsserts);

			if(passed) {
				++summary.numPassed;
			} else {
				++summary.numFailed;
				summary.failedFunctions.push_back(function_name);
			}
		} catch(const std::exception& e) {
			++summary.numErrored;
			summary.erroredFunctions.emplace_back(function_name, e.what());
		}
	}

	return summary;
}

auto main() -> int {
	// NAMES_TO_FUNCTIONS initialized and filled by this point
	const TestCasesSummary summary = runAllTestCases();

	const std::string outputSeparator =
		"----------------------------------------";
	const int numTestCases =
		summary.numPassed + summary.numFailed + summary.numErrored;

	if(numTestCases == 0) {
		std::cout << "There are 0 test cases." << std::endl;
		return 0;
	}

	std::cout << outputSeparator << std::endl;

	if(summary.numPassed == numTestCases) {
		std::cout << "All " << numTestCases << " test cases passed!"
				  << std::endl;
		std::cout << "All " << numPassingAsserts << " asserts passed!"
				  << std::endl;
		std::cout << outputSeparator << std::endl;
		return 0;
	}

	if(!summary.erroredFunctions.empty()) {
		for(const std::pair<std::string, std::string>& result :
			summary.erroredFunctions) {
			std::cout << result.first << " errored: " << std::endl
					  << result.second << std::endl;
		}

		std::cout << outputSeparator << std::endl;
	}

	if(!summary.failedFunctions.empty()) {
		for(const std::string& result : summary.failedFunctions) {
			std::cout << result << " failed" << std::endl;
		}

		std::cout << outputSeparator << std::endl;
	}

	std::cout << "Number of tests passed      : " << summary.numPassed
			  << std::endl;
	std::cout << "Number of tests failed      : " << summary.numFailed
			  << std::endl;
	std::cout << "Number of tests errored     : " << summary.numErrored
			  << std::endl;
	std::cout << outputSeparator << std::endl;

	std::cout << "Number of passing asserts   : " << numPassingAsserts
			  << std::endl;
	std::cout << "Number of failing asserts   : " << numFailingAsserts
			  << std::endl;
	std::cout << outputSeparator << std::endl;

	return 0;
}
