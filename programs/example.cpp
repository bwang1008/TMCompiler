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
	
	/*
	 * Below is equivalent to 
	 * MEM[2] = 2;
	 * MEM[2] += MEM[2];
	 */
	MEM[MEM[2] = 2] += MEM[2];
	printInt(MEM[2]);
	printSpace();

	int ans = 0;

	bool b = !!!false;

	if(isOdd(3)) {
		ans += 1;
	}
	else if(b) {
		ans = 23;
	}
	else {
		ans = 31;
	}

	return ans;
}
