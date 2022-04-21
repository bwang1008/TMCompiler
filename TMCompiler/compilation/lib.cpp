// private functions - all operators (except for !, &&, ||)
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
// int basic_mul2(int x): return 2*x : x must be positive
// int basic_div2(int x): return x/2 (floor): x must be positive
// bool isEven(int x): return (x % 2)? 0 : 1
// bool isOdd(int x): return (bool) (x % 2)
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


// void moveTapeVar0HeadRight(): move head of MEM_bits right
// void moveTapeVar0HeadLeft(): move head of MEM_bits left
// bool tapeVar0IsZero(): check if tape of variable 0 (usually tape 19) is currently at a 0
// bool tapeVar0IsOne(): check if tape of variable 0 (usually tape 19) is currently at a 1
// bool tapeVar0IsBlank(): check if tape of variable 0 (usually tape 19) is currently at blank

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

	// in reference to https://math.stackexchange.com/questions/1147825/designing-a-turing-machine-for-binary-multiplication	

	// swap if x > y: without, 15 * 3758175 took 32,022 steps to compute, but
	// 3758175 * 15 took 111,698 steps to compute (at least at this point in time).
	if(x > y) {
		int temp = x;
		x = y;
		y = temp;
	}

	int ans = 0;

	while(!isZero(x)) {
		if(isOdd(x)) {
			ans += y;
		}

		x = basic_div2(x);
		y = basic_mul2(y);
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
		
		//int desiredBitIndex = ((index + valIndex) * (index + valIndex + 1)) / 2 + index;

		int desiredBitIndex = index + valIndex;
		if(!isZero(desiredBitIndex)) {
			int temp = desiredBitIndex * (desiredBitIndex + 1);
			temp = basic_div2(temp);
			desiredBitIndex = temp + index;
		}

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
			bool bit = isOdd(V);
			if(bit) {
				setMemBitOne();
			}
			else {
				setMemBitZero();
			}

			V = basic_div2(V);
		}
		
		valIndex += 1;
	}

	// must clear out the next tape cell by making it blank
	//int desiredBitIndex = ((index + valIndex) * (index + valIndex + 1)) / 2 + index;
	
	// equivalent to above, but better performance
	int desiredBitIndex = index + valIndex;
	if(!isZero(desiredBitIndex)) {
		int temp = desiredBitIndex * (desiredBitIndex + 1);
		temp = basic_div2(temp);
		desiredBitIndex = temp + index;
	}

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

	//int desiredBitIndex = (index * (index + 1)) / 2 + index;
	
	int desiredBitIndex = index;
	if(!isZero(desiredBitIndex)) {
		int temp = desiredBitIndex * (desiredBitIndex + 1);
		temp = basic_div2(temp);
		desiredBitIndex = temp + index;
	}

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

		//desiredBitIndex = ((index + valIndex) * (index + valIndex + 1)) / 2 + index;

		desiredBitIndex = index + valIndex;
		if(!isZero(desiredBitIndex)) {
			int temp = desiredBitIndex * (desiredBitIndex + 1);
			temp = basic_div2(temp);
			desiredBitIndex = temp + index;
		}

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
