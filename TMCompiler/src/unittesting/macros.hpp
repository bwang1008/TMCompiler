#ifndef MACROS_HPP
#define MACROS_HPP

#define REGISTER_TEST(FUNCTION_NAME) \
	namesToFunctions[#FUNCTION_NAME] = & ## FUNCTION_NAME

// declare the function, register it, then define
#define TEST_CASE(FUNCTION_NAME) \
	bool FUNCTION_NAME () ; \
	REGISTER_TEST(FUNCTION_NAME) ; \
	bool FUNCTION_NAME ()


#endif
