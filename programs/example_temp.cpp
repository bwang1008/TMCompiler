int MEM[5];
int nextInt();
void printInt(int x);
void printSpace();
/*
void what(int x) {

}

// simulate transformation
int linear(int a, int b, int x) {
	int temp = a * x + b;
	
	return temp;
}

void genPrimes(int N) {
	for(int i = 2; i <= N; i += 1) {
		if(MEM[i] == 1) {
			continue;
		}

		for(int j = i*i; j <= N; j += i) {
			MEM[j] = 1;
		}
	}

	for(int i = 2; i <= N; i += 1) {
		if(MEM[i] == 0) {
			printInt(i);
			printSpace();
		}
	}
}
*/

int factorial(int n) {
	if(n == 0 || n == 1) {
		return 1;
	}

	return n * factorial(n-1);
}

int comp(int a, int b) {
	if(b == 0) {
		return a;
	}

	return a + comp(a, b-1);
}

int fib(int n) {
	if(n == 0 || n == 1) {
		return n;
	}

	return fib(n-1) + fib(n-2);
}

int main() {
	//int x = nextInt();
	//int y = nextInt();
	//int z = MEM[2] + 7;

	/*
	int x = 2;
	printInt(x);
	
	int y = x + 1;
	printInt(y);
	int z = x * y;
	printInt(z);
	*/

	/*
	int d = prod / 2;

	printInt(d);
	int z = d;
	*/

	/*
	MEM[3] = MEM[1] - 1;
	MEM[5] = 7;
	MEM[MEM[2] += 3] += MEM[5];
	//MEM[5] = MEM[3];

	printInt(MEM[1]);
	printSpace();
	printInt(MEM[2]);
	printSpace();
	printInt(MEM[3]);
	printSpace();
	printInt(MEM[5]);
	printSpace();
	*/
	
	//printInt(MEM[2]);
	//printSpace();
	//printInt(MEM[5]);
	//printSpace();

	/*
	for(int i = 0; i < 5; i += 1) {
		if(i % 3 == 0) {
			continue;
		}

		else if(i % 3 == 1) {
			z += 1;
		}
		else if(i % 3 == 2) {
			z *= 2;
		}
	
		for(int j = 0; j < 3; j += 1) {
			if(j % 2 == 0) {
				continue;
			}
			z += i;
		}
	}

	bool b = !!!!!true;
	bool c = (b ^= b);

	int s = 0;
	for(int i = 0; i < 10; i += 1) {
		b = !b;
		if(b) {
			s += i;
		}
	}

	int ans = -linear(x, -x, x) + 0;
	printInt(ans);

	ans += 1;
	*/

	//int w;
	//w = x = y = 18;
	
	/*
	if(true && false || x > y) {
		return -5;
	}

	//int w = 5;

	return 7;
	*/

	/*
	genPrimes(25);

	bool b = true;
	bool c = (1 > 25);
	b &= !c;
	b |= c;
	
	c = b ^ b;
	*/

	int ans = 3758175 * 15;

	return ans;
}
