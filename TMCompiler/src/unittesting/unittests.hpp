#ifndef UNITTESTS_HPP
#define UNITTESTS_HPP

// YES

#define REGISTER_TEST(FUNCTION_NAME) \
	void register_ ## FUNCTION_NAME () { \
		namesToFunctions[#FUNCTION_NAME] = &FUNCTION_NAME ; \
	} \
	int dummy_register_ ## FUNCTION_NAME = ( register_ ## FUNCTION_NAME (), 0)

// declare the function, register it, then define
#define TEST_CASE(FUNCTION_NAME) \
	bool FUNCTION_NAME () ; \
	REGISTER_TEST(FUNCTION_NAME) ; \
	bool FUNCTION_NAME ()

using FUNCTION_POINTER = bool (*)(void);
std::map<std::string, FUNCTION_POINTER> namesToFunctions;

#endif
