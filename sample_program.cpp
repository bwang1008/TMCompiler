int fib(int x) {
	if(x == 0 || x == 1) {
		return x;
	}

	return fib(x - 2) + fib(x - 1);
}

int foo(int x) {
	if(x % 2 == 1) {
		return 2 * x;
	}

	return foo(x - 1);
}

int main() {
	int ans = foo(32);

	ans += fib(5);
	return ans;
}
