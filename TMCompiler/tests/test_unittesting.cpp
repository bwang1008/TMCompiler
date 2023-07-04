#include <vector>

#include <TMCompiler/utils/unittesting/unittests.hpp>

// run from TMCompiler root:
// g++ -std=c++11 -I. TMCompiler/tests/test_unittesting.cpp

auto factorial(int n) -> int {
	int prod = 1;
	for(int i = 2; i <= n; ++i) {
		prod *= i;
	}

	return prod;
}

TEST_CASE(test0) {
	ASSERT(factorial(0) == 1);
}

TEST_CASE(test1) {
	ASSERT(factorial(1) == 1);
}

TEST_CASE(test2) {
	ASSERT(factorial(2) == 2);
}

TEST_CASE(test3) {
	const int fac3 = 6;
	ASSERT(factorial(3) == fac3);
}

TEST_CASE(test4) {
	const int fac4 = 24;
	ASSERT(factorial(4) == fac4);
}

TEST_CASE(test5) {
	const int test_arg = 5;
	const int fac5 = 120;
	ASSERT(factorial(test_arg) == fac5);
}

TEST_CASE(test6) {
	const int test_arg = 6;
	const int fac6 = 720;
	ASSERT(factorial(test_arg) == fac6);
}

TEST_CASE(test7) {
	const int test_arg = 7;
	const int fac7 = 5040;
	ASSERT(factorial(test_arg) == fac7);
}

TEST_CASE(test8) {
	// throw std::runtime_error("Gak!");
	ASSERT(true);
}

TEST_CASE(test9) {
	const int factorial0 = 1;
	const int factorial1 = 1;
	const int factorial2 = 2;
	const int factorial3 = 6;
	const int factorial4 = 24;
	const int factorial5 = 120;
	std::vector<int> answers = {
		factorial0, factorial1, factorial2, factorial3, factorial4, factorial5};
	for(std::size_t i = 0; i < answers.size(); ++i) {
		ASSERT(factorial(i) == answers[i]);
	}
}
