/**
 * Sort N integers, using heap sort.
 * Input: the first integer is N, the number of integers to sort.
 * Then there will be N integers.
 * Output: the N numbers in sorted order
 */

int getLeft(int i) {
	return 2 * i + 1;
}

int getRight(int i) {
	return 2 * i + 2;
}

int parent(int i) {
	return (i-1) / 2;
}

void swap(int heapPointer, int i, int j) {
	int temp = MEM[heapPointer + i];
	MEM[heapPointer + i] = MEM[heapPointer + j];
	MEM[heapPointer + j] = temp;
}

void bubbleUp(int heapPointer, int index) {
	while(index != 0 && MEM[heapPointer + parent(index)] > MEM[heapPointer + index]) {
		swap(heapPointer, index, parent(index));
		index = parent(index);
	}
}

void add(int heapPointer, int heapSize, int element) {
	MEM[heapPointer + heapSize] = element;
	
	bubbleUp(heapPointer, heapSize);
}

void heapify(int heapPointer, int heapSize, int index) {
	int smallest = index;
	int left = getLeft(index);
	int right = getRight(index);

	if(left < heapSize && MEM[heapPointer + left] < MEM[heapPointer + smallest]) {
		smallest = left;
	}
	if(right < heapSize && MEM[heapPointer + right] < MEM[heapPointer + smallest]) {
		smallest = right;
	}

	if(smallest != index) {
		swap(heapPointer, index, smallest);
		heapify(heapPointer, heapSize, smallest);
	}
}

int extractMin(int heapPointer, int heapSize) {
	if(heapSize == 1) {
		int ans = MEM[heapPointer];
		return ans;
	}

	int ans = MEM[heapPointer];
	MEM[heapPointer] = MEM[heapPointer + heapSize - 1];
	heapify(heapPointer, heapSize, 0);

	return ans;
}

int main() {
	int N = nextInt();

	for(int i = 0; i < N; i += 1) {
		MEM[i] = nextInt();
	}

	int heapPointer = N;
	int heapSize = 0;

	for(int i = 0; i < N; i += 1) {
		add(heapPointer, heapSize, MEM[i]);
		heapSize += 1;
	}

	for(int i = 0; i < N; i += 1) {
		int element = extractMin(heapPointer, heapSize);
		heapSize -= 1;

		printInt(element);
		printSpace();
	}

	return 0;
}
/*
5
9 5 8 2 5

->
2 5 5 8 9

Input bits: 0101_01001_0101_00001_001_0101
Output bits: 001_0101_0101_00001_01001
*/
