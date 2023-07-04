#ifndef UNITTESTS_HPP
#define UNITTESTS_HPP

#include <iostream>	 // std::cout, std::endl
#include <map>		 // std::map
#include <string>	 // std::string

using FunctionPointer = void (*)();

// defined in unittests.cpp
extern int num_passing_asserts;
extern int num_failing_asserts;
extern std::map<std::string, FunctionPointer> NAMES_TO_FUNCTIONS;

// declare the function, register it, then define
// Registering assigns function to global map. runs before main by using comma
// operator
#define TEST_CASE(FUNCTION_NAME)                                    \
	void FUNCTION_NAME();                                           \
	int DUMMY_REGISTER_##FUNCTION_NAME =                            \
		(NAMES_TO_FUNCTIONS[#FUNCTION_NAME] = &(FUNCTION_NAME), 0); \
	void FUNCTION_NAME()

#define ASSERT(BOOL_EXPR)                                       \
	if(!(BOOL_EXPR)) {                                          \
		++num_failing_asserts;                                  \
		std::cout << "Assert failed in " << __FILE__ << " in "  \
				  << static_cast<const char*>(__func__) << "()" \
				  << " on line " << __LINE__ << std::endl;      \
	} else                                                      \
		++num_passing_asserts

#endif
