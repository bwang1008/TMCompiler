// private functions - all operators (except for !, &&, ||)
// reason: worried about 0 vs -0 being registered as different...
// as well as private functions: memset, memget. The functions nextInt, printInt, printSpace implemented in Turing Machine

// actual functions to be implemented in Turing-Machine: isZero, isNeg, isPos, basic_add, basic_sub, basic_xor, basic_eq: semantically,
// bool isZero(int x): return (x == 0)
// bool isNeg(int x): return (x < 0)
// bool isPos(int x): return (x > 0)
// int basic_add(int x, int y): return x + y	: x,y must both be positive
// int basic_sub(int x, int y): return x - y	: x,y must both be positive, x > y
// int basic_xor(int x, int y): return x ^ y	: x,y must both be non-zero
// int basic_eq(int x, int y): return x == y    : x,y must both be positive
// int basic_lt(int x, int y): return x < y		: x,y must both be positive
// int basic_neg(int x): return -x				: x must be non-zero
//
// int getMemBitIndex(): MEM represented as bits. What index is head at?
// void setMemBitIndex(int x): set the head of MEM_bits
// void moveMemHeadRight(): move head of MEM_bits right
// void moveMemHeadLeft(): move head of MEM_bits left
// void setMemBitZero(): set tape cell at head of MEM_bits to be 0
// void setMemBitOne(): set tape cell at head of MEM_bits to be 1
// void setMemBitBlank(): set tape cell at head of MEM_bits to be blank
// bool memBitIsZero(): check if tape cell at head of MEM_bits is 0
// bool memBitIsOne(): check if tape cell at head of MEM_bits is 1
// bool memBitIsBlank(): check if tape cell at head of MEM_bits is blank

// unlike !,&&,|| however, these above are functions: need to manually pop off
// from parameters stack when processing

/**
 * implement x + y
 */
int add(int x, int y) {
	if(isZero(x)) {
		return y;
	}
	if(isZero(y)) {
		return x;
	}

	// here, both are non-zero

	bool b1 = isPos(x);
	bool b2 = isPos(y);

	if(b1) {
		if(b2) {
			return basic_add(x, y);
		}

		// y is negative
		int negY = basic_neg(y);
		if(negY == x) {
			return 0;
		}

		if(negY < x) {
			// case: 5 + -3
			return basic_sub(x, negY);
		}

		// case: 3 + (-5)
		return basic_neg(basic_sub(negY, x));
	}
	
	// x is negative
	if(b2) {
		// y is positive
		int negX = basic_neg(x);
		if(negX == y) {
			return 0;
		}

		if(negX < y) {
			// case: -3 + 5
			return basic_sub(y, negX);
		}

		// case: -5 + 3
		return basic_neg(basic_sub(negX, y));
	}
	
	// both x and y are negative

	// case: -5 + (-3)
	return basic_neg(basic_add(basic_neg(x), basic_neg(y)));

}

/**
 * implement x - y
 */
int sub(int x, int y) {
	if(isZero(x)) {
		return -y;
	}
	
	if(isZero(y)) {
		return x;
	}
		
	// here, both non-zero
	if(isNeg(x) && isNeg(y)) {
		// ex: -3 - (-5)  == -3 + 5
		return add(x, -y);
	}
	else if(isNeg(x) && isPos(y)) {
		// ex: -3 - (5) == -(3 + 5)
		return -basic_add(-x, y);
	}
	else if(isPos(x) && isNeg(y)) {
		// ex: 3 - (-5) == 3 + 5
		return basic_add(x, -y);
	}
	else {
		// both positive
		// ex: 3 - 5 == 3 + (-5)
		return add(x, -y);
	}
}

/**
 * implement x * y
 */
int mul(int x, int y) {
	if(isZero(x) || isZero(y)) {
		return 0;
	}
	
	bool ansNeg = false;

	if(isNeg(x)) {
		x = -x;
		ansNeg = true;
	}

	if(isNeg(y)) {
		y = -y;
		ansNeg = !ansNeg;
	}

	// reduced binary search
	// use only addition, subtraction, and comparisons
	// subtract highest power of 2 from y0 (which is y1);
	// add to ans the value (x * that power of 2) (which is val1)
	// ex: 3 * 11 == 3*8 + remaining 3*3
	int ans = 0;

	int y0 = y;
	while(y0 > 0) {
		int y1 = 1;
		int y2 = 2;
		int val1 = x;
		int val2 = x + x;

		while(val2 <= y0) {
			y1 = y2;
			val1 = val2;
			y2 += y2;
			val2 += val2;
		}

		ans += val1;
		y0 -= y1;
	}
	
	if(ansNeg) {
		ans = -ans;
	}

	return ans;
}

int div(int x, int y) {
	if(isZero(x)) {
		return 0;
	}
	
	if(x < 0 && y < 0) {
		return div(-x, -y);
	}
	if(x < 0) {
		return -div(-x, y);
	}
	if(y < 0) {
		return -div(x, -y);
	}
	// replace with enhanced binary search? then need division by 2...

	int ans = 0;

	/*
	while(y * (ans + 1) < x)  {
		ans += 1;
	}
	*/

	// rudimentary binary search: find largest power of two less than or equal to it, subtract, repeat
	int n = x;
	while(n >= y) {			// if n < y, then remaining to jump is just 0; nothing to add
		int prevJump = 0;
		int jump = 1;
		while(y * (ans + jump) <= x) {
			prevJump = jump;
			jump += jump;
		}

		ans += prevJump;
		n = x - (y * ans);
	}
	
	// analysis: each iteration of outer while loop, gains most-significant digit of ans.
	// takes O(log(x)) calls to multiplication to perform one outer while loop
	// total: O(log^2(x)) calls to multiplication. at least better than original naive! 
	return ans;	
}

int mod(int x, int y) {
	int d = div(x, y);
	return x - y * d;
}

int neg(int x) {
	if(isZero(x)) {
		return 0;
	}

	// can't use sub cuz sub uses neg
	return basic_neg(x);
}

bool lt(int x, int y) {
	// careful: tho isPos(y - x) seems nice, 
	// sub(traction) uses "<" operator
	// avoid dependency loop please...
	
	if(isNeg(x)) {
		if(isNeg(y)) {
			// cases: -a < -b  -> (a+b) + (-a) <? (a+b) + (-b)  -> b <? a
			return lt(-y, -x);
		}
		else {
			// y is 0 or positive
			return true;
		}
	}
	else if(isZero(x)) {
		if(isNeg(y)) {
			// 0 <? -5  -> false
			return false;
		}
		else if(isZero(y)) {
			return false;
		}
		else {
			// y positive
			return true;
		}
	}
	else {
		// x is positive
		if(isNeg(y) || isZero(y)) {
			return false;
		}
		else {
			// y positive
			return basic_lt(x, y);
		}
	}
}

bool leq(int x, int y) {
	return (x == y) || (x < y);
}

bool gt(int x, int y) {
	return !(x <= y);
}

bool geq(int x, int y) {
	return !(x < y);
}

bool eq(int x, int y) {
	if(isZero(x)) {
		if(isZero(y)) {
			return true;
		}
		return false;
	}
	else if(isNeg(x)) {
		if(isNeg(y)) {
			return basic_eq(-x, -y);
		}
		// y >= 0
		return false;
	}
	else {
		// x is positive
		if(isPos(y)) {
			return basic_eq(x, y);
		}
		// y <= 0
		return false;
	}
}

bool neq(int x, int y) {
	return !(x == y);
}

// return type of xor can be bool or int, but doesn't really matter to a tape
// EDIT: apparently "xor" is a keyword in C++; have to use sth else ("eor")
int eor(int x, int y) {
	if(isZero(x)) {
		return y;
	}

	if(isZero(y)) {
		return x;
	}

	// here, x and y are non-zero
	return basic_xor(x, y);
}


// private functions

// actually just memset, memget: printInt, printSpace, nextInt can be implemented in Turing Machine

/**
 * Add val into MEM[index]. As function of bits, to set bit y of index x, set bits[(x+y)^2 + y]
 * from cstheory.stackexchange.com/questions/18688/a-small-c-like-language-that-turing-machines-can-simulate
 * EDIT: use diagonalization trick: 
 * 0136
 * 247
 * 58
 * 9
 * So row r and col c corresponds to 1d index of (r+c)*(r+c+1)/2 + r
 * represent 0 as 0, 11 (8 + 3) as 01101 (pos and 1 + 2 + 8), and -6 as 1011 (neg and 2 + 4)
 */
void memset(int index, int val) {
	int currBitIndex = getMemBitIndex();

	bool handledSign = false;
	int valIndex = 0;
	int V = val;

	while(!isZero(V) || !handledSign) {
		// function above: (x+y)^2 + y
		int desiredBitIndex = ((index + valIndex) * (index + valIndex + 1)) / 2 + index;

		while(currBitIndex > desiredBitIndex) {
			currBitIndex -= 1;
			moveMemHeadLeft();
		}
		while(currBitIndex < desiredBitIndex) {
			currBitIndex += 1;
			moveMemHeadRight();
		}

		if(!handledSign) {
			if(isNeg(V)) {
				setMemBitOne();
				V = -V;
			}
			else {
				setMemBitZero();
			}

			handledSign = true;
		}
		else {
			int V2 = V / 2;
			int bit = V - (2 * V2);
			if(bit == 1) {
				setMemBitOne();
			}
			else {
				setMemBitZero();
			}

			V = V2;
		}
		
		valIndex += 1;
	}

	// must clear out the next tape cell by making it blank
	int desiredBitIndex = ((index + valIndex) * (index + valIndex + 1)) / 2 + index;
	while(currBitIndex < desiredBitIndex) {
		currBitIndex += 1;
		moveMemHeadRight();
	}

	setMemBitBlank();

	setMemBitIndex(currBitIndex);
}

int memget(int index) {
	int ans = 0;
	int currBitIndex = getMemBitIndex();
	int desiredBitIndex = (index * (index + 1)) / 2 + index;

	//printInt(desiredBitIndex);

	while(currBitIndex > desiredBitIndex) {
		currBitIndex -= 1;
		moveMemHeadLeft();
	}
	while(currBitIndex < desiredBitIndex) {
		currBitIndex += 1;
		moveMemHeadRight();
	}
	
	int valIndex = 0;
	bool shouldBeNegative = false;
	int pow2 = 0;
	while(!memBitIsBlank()) {
		// first bit encountered is the sign bit
		if(isZero(valIndex)) {
			// 1 for negative, 0 for non-negative
			if(memBitIsOne()) {
				shouldBeNegative = true;	
			}
		}
		else {
			// goes from 1,2,4,8,... increasing. Little-endian
			if(memBitIsOne()) {
				ans += pow2;
			}
		}

		valIndex += 1;
		if(isZero(pow2)) {
			pow2 = 1;
		}
		else {
			pow2 += pow2;
		}

		desiredBitIndex = ((index + valIndex) * (index + valIndex + 1)) / 2 + index;
		while(currBitIndex < desiredBitIndex) {
			currBitIndex += 1;
			moveMemHeadRight();
		}
	}

	if(shouldBeNegative) {
		ans = -ans;
	}

	setMemBitIndex(currBitIndex);

	return ans;
}
