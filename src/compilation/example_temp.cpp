int MEM[5];
int nextInt();
void printInt(int x);
void printSpace();
void what(int x) {

}

// simulate transformation
int linear(int a, int b, int c) {
	int temp = a * b / c;
	
	int x = a - -1;	
	
	return temp;
}

int main() {
	int x = nextInt();
	int y = x + 1;
	int z = MEM[2] + 1;

	MEM[3] = MEM[1] - 1;
	MEM[MEM[2] += 3] += MEM[5];
	MEM[5] = MEM[3];

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

	int w;
	w = x = y = 18;
	
	//int x = 5;
	//int y = 2;

	if(true && false || x > y) {
		return x/y;
	}

	//int w = 5;

	return w - x*y;
}
