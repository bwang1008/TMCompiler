/**
 * Add N integers
 * Input: the first integer is N, the number of integers to sort.
 * Then there will be N integers.
 * Output: the N numbers in sorted order
 *
 * Just to demonstrate you can achieve something that looks like malloc in C
 */

void initializeMalloc() {
	MEM[0] = 1;
}

/**
 * Reserve memory by using new memory; no deleting old memory
 */
int malloc(int size) {
	int ptr = MEM[0];
	MEM[0] += size;
	return ptr;
}

int main() {
	initializeMalloc();

	// read in input
	int N = nextInt();

	// reserve space for N integers
	int arrPtr = malloc(N);
	for(int i = 0; i < N; i += 1) {
		MEM[arrPtr + i] = nextInt();	// looks like *(arrPtr + i) = nextInt() in C
	}

	int sum = 0;
	for(int i = 0; i < N; i += 1) {
		sum += MEM[arrPtr + i];
	}

	printInt(sum);
	printSpace();

	return 0;
}
