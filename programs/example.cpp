int factorial(int n) {
	if(n == 0 || n == 1) {
		return 1;
	}

	return n * factorial(n-1);
}

bool isOdd(int n) {
	if(n < 0) {
		n = -n;
	}

	return !isEven(n);
}

bool isEven(int n) {
	if(n < 0) {
		n = -n;
	}

	if(n == 0) {
		return true;
	}

	return isOdd(n - 1);
}

int main() {
	int a = 99 * 99;
	int b = factorial(5);

	printInt(a);
	printSpace();

	printInt(b);
	printSpace();

	int ans = 0;

	if(isOdd(3)) {
		ans += 1;
	}

	if(isEven(3)) {
		ans += 2;
	}

	if(isOdd(0)) {
		ans += 4;
	}

	return ans;
}
