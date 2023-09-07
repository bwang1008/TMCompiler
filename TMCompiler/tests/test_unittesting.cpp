#include <cstddef>	  // std::size_t
#include <stdexcept>  // std::runtime_error
#include <vector>	  // std::vector

#include <TMCompiler/utils/unittesting/unittests.hpp>

// run from TMCompiler root:
// g++ -std=c++11 -I. TMCompiler/tests/test_unittesting.cpp

auto factorial(std::size_t n) -> std::size_t {
	std::size_t prod = 1;
	for(std::size_t i = 2; i <= n; ++i) {
		prod *= i;
	}

	return prod;
}

TEST_CASE(test_unittesting_0) {
	ASSERT(factorial(0) == 1);
}

TEST_CASE(test_unittesting_1) {
	ASSERT(factorial(1) == 1);
}

TEST_CASE(test_unittesting_2) {
	ASSERT(factorial(2) == 2);
}

TEST_CASE(test_unittesting_3) {
	const std::size_t fac3 = 6;
	ASSERT(factorial(3) == fac3);
}

TEST_CASE(test_unittesting_4) {
	const std::size_t fac4 = 24;
	ASSERT(factorial(4) == fac4);
}

TEST_CASE(test_unittesting_5) {
	const std::size_t test_arg = 5;
	const std::size_t fac5 = 120;
	ASSERT(factorial(test_arg) == fac5);
}

TEST_CASE(test_unittesting_6) {
	const std::size_t test_arg = 6;
	const std::size_t fac6 = 720;
	ASSERT(factorial(test_arg) == fac6);
}

TEST_CASE(test_unittesting_7) {
	const std::size_t test_arg = 7;
	const std::size_t fac7 = 5040;
	ASSERT(factorial(test_arg) == fac7);
}

TEST_CASE(test_unittesting_8) {
	// throw std::runtime_error("Gak!");
	ASSERT(true);
}

TEST_CASE(test_unittesting_9) {
	const std::size_t factorial0 = 1;
	const std::size_t factorial1 = 1;
	const std::size_t factorial2 = 2;
	const std::size_t factorial3 = 6;
	const std::size_t factorial4 = 24;
	const std::size_t factorial5 = 120;
	std::vector<std::size_t> answers = {
		factorial0, factorial1, factorial2, factorial3, factorial4, factorial5};
	for(std::size_t i = 0; i < answers.size(); ++i) {
		ASSERT(factorial(i) == answers[i]);
	}
}
