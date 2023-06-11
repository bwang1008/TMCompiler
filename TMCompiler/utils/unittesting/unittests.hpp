#ifndef UNITTESTS_HPP
#define UNITTESTS_HPP

#include <exception>
#include <iostream>
#include <list>
#include <map>
#include <string>
#include <utility>

using FunctionPointer = void (*)();

// defined in unittests.cpp
extern int numPassingAsserts;
extern int numFailingAsserts;
extern std::map<std::string, FunctionPointer> NAMES_TO_FUNCTIONS;

// assigns function to global map. runs before main by using comma operator
#define REGISTER_TEST(FUNCTION_NAME)     \
	int DUMMY_REGISTER_##FUNCTION_NAME = \
		(NAMES_TO_FUNCTIONS[#FUNCTION_NAME] = &(FUNCTION_NAME), 0)

// declare the function, register it, then define
#define TEST_CASE(FUNCTION_NAME)  \
	void FUNCTION_NAME();         \
	REGISTER_TEST(FUNCTION_NAME); \
	void FUNCTION_NAME()

#define FAIL_ASSERT()                                       \
	std::cout << "Assert failed in " << __FILE__ << " in "  \
			  << static_cast<const char*>(__func__) << "()" \
			  << " on line " << __LINE__ << std::endl;

void ASSERT(bool test_expression);

struct TestCasesSummary {
	int numPassed{0};
	int numFailed{0};
	int numErrored{0};
	std::list<std::string> failedFunctions;
	std::list<std::pair<std::string, std::string> > erroredFunctions;
};

#endif
