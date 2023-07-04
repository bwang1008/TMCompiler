#include "unittests.hpp"

#include <exception>  // std::exception
#include <iostream>	  // std::cout, std::endl
#include <list>		  // std::list
#include <map>		  // std::map
#include <string>	  // std::string
#include <utility>	  // std::pair

int num_passing_asserts = 0;
int num_failing_asserts = 0;

std::map<std::string, FunctionPointer> NAMES_TO_FUNCTIONS;

struct TestCasesSummary {
	int num_passed{0};
	int num_failed{0};
	int num_errored{0};
	std::list<std::string> failed_functions;
	std::list<std::pair<std::string, std::string> > errored_functions;
};

auto run_all_test_cases() -> TestCasesSummary {
	TestCasesSummary summary;

	for(const std::pair<const std::string, FunctionPointer>& it :
		NAMES_TO_FUNCTIONS) {
		const std::string function_name = it.first;
		const FunctionPointer func = it.second;

		try {
			const int prev_failing_asserts = num_failing_asserts;
			func();
			const bool passed = (num_failing_asserts == prev_failing_asserts);

			if(passed) {
				++summary.num_passed;
			} else {
				++summary.num_failed;
				summary.failed_functions.push_back(function_name);
			}
		} catch(const std::exception& e) {
			++summary.num_errored;
			summary.errored_functions.emplace_back(function_name, e.what());
		}
	}

	return summary;
}

auto main() -> int {
	// NAMES_TO_FUNCTIONS initialized and filled by this point
	const TestCasesSummary summary = run_all_test_cases();

	const std::string output_separator =
		"----------------------------------------";
	const int num_test_cases =
		summary.num_passed + summary.num_failed + summary.num_errored;

	if(num_test_cases == 0) {
		std::cout << "There are 0 test cases." << std::endl;
		return 0;
	}

	std::cout << output_separator << std::endl;

	if(summary.num_passed == num_test_cases) {
		std::cout << "All " << num_test_cases << " test cases passed!"
				  << std::endl;
		std::cout << "All " << num_passing_asserts << " asserts passed!"
				  << std::endl;
		std::cout << output_separator << std::endl;
		return 0;
	}

	if(!summary.errored_functions.empty()) {
		for(const std::pair<std::string, std::string>& result :
			summary.errored_functions) {
			std::cout << result.first << " errored: " << std::endl
					  << result.second << std::endl;
		}

		std::cout << output_separator << std::endl;
	}

	if(!summary.failed_functions.empty()) {
		for(const std::string& result : summary.failed_functions) {
			std::cout << result << " failed" << std::endl;
		}

		std::cout << output_separator << std::endl;
	}

	std::cout << "Number of tests passed      : " << summary.num_passed
			  << std::endl;
	std::cout << "Number of tests failed      : " << summary.num_failed
			  << std::endl;
	std::cout << "Number of tests errored     : " << summary.num_errored
			  << std::endl;
	std::cout << output_separator << std::endl;

	std::cout << "Number of passing asserts   : " << num_passing_asserts
			  << std::endl;
	std::cout << "Number of failing asserts   : " << num_failing_asserts
			  << std::endl;
	std::cout << output_separator << std::endl;

	return 0;
}
