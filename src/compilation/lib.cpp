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
	if(isNeg(x) && isPos(y)) {
		if(-x == y) {
			// cxse: -5 + 5
			return 0;
		}
		else if(-x < y) {
			// cxse: -3 + 5
			return basic_sub(y, -x);
		}
		else {
			// -x > y: case: -5 + 3
			return -basic_sub(-x, y);
		}
	}
	else if(isPos(x) && isNeg(y)) {
		if(-y == x) {
			return 0;
		}
		else if(-y < x) {
			// case: 5 + -3
			return basic_sub(x, -y);
		}
		else {
			// -y > x: case: 3 + (-5)
			return -basic_sub(-y, x);
		}
	}
	else if(isNeg(x) && isNeg(y)) {
		// case: -5 + (-3)
		return -basic_add(-x, -y);
	}
	else {
		// both positive
		return basic_add(x, y);
	}
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
	while(y0 > y) {
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

	return sub(0, x);
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
int xor(int x, int y) {
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

void memset(int index, int val) {
	
}

int memget(int index) {
	return 0;
}
