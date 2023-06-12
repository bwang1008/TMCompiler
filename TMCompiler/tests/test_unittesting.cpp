#include <vector>

#include <TMCompiler/utils/unittesting/unittests.hpp>

// run from TMCompiler root:
// g++ -std=c++11 -I. TMCompiler/tests/test_unittesting.cpp

int factorial(int n) {
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
	ASSERT(factorial(3) == 6);
}

TEST_CASE(test4) {
	ASSERT(factorial(4) == 24);
}

TEST_CASE(test5) {
	ASSERT(factorial(5) == 120);
}

TEST_CASE(test6) {
	ASSERT(factorial(6) == 720);
}

TEST_CASE(test7) {
	ASSERT(factorial(7) == 5040);
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
