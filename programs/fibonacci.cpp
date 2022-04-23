int fib(int n) {
	if(n == 0 || n == 1) {
		return n;
	}

	int prev = 0;
	int curr = 1;

	for(int i = 2; i <= n; i += 1) {
		int next = prev + curr;
		prev = curr;
		curr = next;
	}

	return curr;
}

int main() {
	int n = nextInt();
	return fib(n);
}
