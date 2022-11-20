#include "unittesting/unittests.hpp"

// run from TMCompiler root:
// g++ -std=c++11 -ITMCompiler/src TMCompiler/test/test_unittesting.cpp

int factorial(int n) {
	int prod = 1;
	for(int i = 2; i <= n; ++i) {
		prod *= i;
	}

	return prod;
}

TEST_CASE(test0) {
	return factorial(0) == 1;
}

TEST_CASE(test1) {
	return factorial(1) == 1;
}

TEST_CASE(test2) {
	return factorial(2) == 2;
}

TEST_CASE(test3) {
	return factorial(3) == 6;
}

TEST_CASE(test4) {
	return factorial(4) == 24;
}

TEST_CASE(test5) {
	return factorial(5) == 120;
}

TEST_CASE(test6) {
	return factorial(6) == 720;
}

TEST_CASE(test7) {
	return factorial(7) == 100;
}

TEST_CASE(test8) {
	throw std::runtime_error("Gak!");
	return true;
}
