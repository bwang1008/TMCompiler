#include "unit1.h"

#include <algorithm>		// std::reverse
#include <stdexcept>
#include <unordered_map>	// std::unordered_map
#include <utility>			// std::pair, std::make_pair
#include <string>			// std::string, std::stoi
#include <vector>			// std::vector

#include "multi_tape_builder.h"
#include "utils.h"
#include "../tm_definition/transition.h"
#include "../tm_definition/multi_tape_turing_machine.h"

/**
 * Turns sth like "!TAPE_tape5" into 5
 */
int parseTapeNum(const std::string &word) {
	if(word.size() < 10 || word.substr(0, 10) != "!TAPE_tape") {
		throw std::invalid_argument("word must start with !TAPE_tape");
	}

	const std::string suffix = word.substr(10, word.size() - 10);

	return std::stoi(suffix);
}

/**
 * Return number of distinct variables of form "!TAPE_tapeX" for some integer X
 */
int countTapeVariables(const std::vector<std::string> &assembly) {
	int numVars = 0;

	for(size_t i = 0; i < assembly.size(); ++i) {
		const std::vector<std::string> words = getWords(assembly[i]);
		for(size_t j = 0; j < words.size(); ++j) {
			const std::string word = words[j];
			if(word.substr(0, 10) == "!TAPE_tape") {
				const int tapeIndex = parseTapeNum(word);
				if(tapeIndex > numVars) {
					numVars = tapeIndex;
				}
			}
		}
	}

	return numVars + 1;
}

/**
 * calculate biginteger / 2 (floor integer division)
 * assume val is positive
 */
std::string divideIntegerBy2(const std::string &val) {
	bool carry = false;

	std::string ans;
	for(size_t i = 0; i < val.size(); ++i) {
		int digit = (val[i] - '0');
		
		if(digit < 0 || digit >= 10) {
			throw std::invalid_argument(val + " not an integer");
		}

		if(carry) {
			digit += 10;
		}

		ans.push_back(digit / 2);
		carry = (digit % 2 == 1);
	}

	size_t firstNon0 = 0;
	while(firstNon0 < ans.size() && ans[firstNon0] == '0') {
		++firstNon0;
	}

	ans = ans.substr(firstNon0, ans.size() - firstNon0);
	
	if(ans.size() == 0) {
		ans.push_back('0');
	}

	return ans;
}

/**
 * Convert string representing integer literal to bits
 * Use style of (1 if neg, 0 if non-negative), then bits from 
 * least-significant to most-significant. 0 is just "0"
 */
std::string convertIntegerToBits(std::string val) {
	if(val.size() == 0) {
		throw std::invalid_argument(val + " is blank, so not an integer");
	}

	std::string bits;

	if(val[0] == '-') {
		bits.push_back('1');
		val = val.substr(1, val.size() - 1);
	}
	else {
		bits.push_back('0');
	}

	while(val.size() != 1 || val[0] != '0') {
		const int mod2 = ((val.back() - '0') % 2);
		bits.push_back(mod2);
		val = divideIntegerBy2(val);
	}

	return val;
}

/**
 * add nodes and transitions to increment ip
 * should be called only once to construct
 */
void addIncrementIP(MultiTapeBuilder &builder) {
	// ip is in 2's complement
	// worked out on paper
	
	const int q0 = builder.newNode("before");
	const int q1 = builder.newNode();
	const int q2 = builder.newNode();
	const int q3 = builder.node("sideways");

	const int ipTapeIndex = builder.tapeIndex("ip");

	// q0: if see 0 or 1, go right, stay at q0
	// Else see blank: go left and to q1.
	
	builder.add1TapeTransition(q0, q0, ipTapeIndex, "[01]", ".", 1);
	builder.add1TapeTransition(q0, q1, ipTapeIndex, "_", "_", -1);

	// q1: if see 1, write a 0, go left, stay at q1
	// if see 0, write a 1, go left, go to q2.
	// if see blank, write a blank, head no move, go to q2

	builder.add1TapeTransition(q1, q1, ipTapeIndex, "1", "0", -1);
	builder.add1TapeTransition(q1, q2, ipTapeIndex, "0", "1", -1);
	builder.add1TapeTransition(q1, q2, ipTapeIndex, "_", "_", 0);

	// q2: if see 0 or 1, write same, go left, stay at q2
	// if see blank, write blank, move right, to q3

	builder.add1TapeTransition(q2, q2, ipTapeIndex, "[01]", ".", -1);
	builder.add1TapeTransition(q2, q3, ipTapeIndex, "_", ".", 1);


	// q3 is end node: have to connect to setting sideways ip tape

	// sideways tapes are consecutive. Read from IP tape, write to sideways tape
	const int sidewaysTapeIndex = builder.tapeIndex("ipSideways");
	int prevState = q3;

	for(size_t i = 0; i < builder.ipSize; ++i) {
		const int toState = builder.newNode();

		// read a 0? write a 0. read 1 ? write a 1. Can't use . because only for same tapes
		for(int j = 0; j < 2; ++j) {
			std::vector<std::pair<int, std::string> > reads;
			std::vector<std::pair<int, std::string> > writes;
			std::vector<std::pair<int, int> > shifts;

			reads.emplace_back(ipTapeIndex, std::to_string(j));
			writes.emplace_back(sidewaysTapeIndex + i, std::to_string(j));
			shifts.emplace_back(sidewaysTapeIndex + i, 1);

			builder.addTransition(prevState, toState, reads, writes, shifts);
		}

		prevState = toState;
	}

	// but now head of ip tape at the end. move back left to beginning of ip
	for(size_t i = 0; i < builder.ipSize; ++i) {
		const int toState = builder.newNode();
		// whatever you read, don't change it, but move left
		builder.add1TapeTransition(prevState, toState, ipTapeIndex, ".", ".", -1);
		prevState = toState;
	}

	// go from this last state, into the "after" state: since now 
	// we are after incrementing IP
	builder.add1TapeTransition(prevState, builder.node("after"), ipTapeIndex, ".", ".", 0);
}

/*
 * push -2 onto ipStack
 * use p bits to represent -2 in 2's complement:
 * bunch of 1s, but last is 0. 
 * Ex: if p is 5, then -2 represented as 11110.
 * Make sure to move head back to start (left)
 * return last node used
 * should be called only once to construct
*/
int addNeg2ToIpStack(MultiTapeBuilder &builder) {
	const int ipStackTapeIndex = builder.tapeIndex("output");

	// for a given program, ipSize is constant
	// so add a constant number of transitions to palce in -2

	int prevState = builder.node("start");
	for(size_t i = 0; i < builder.ipSize; ++i) {
		const int toState = builder.newNode();
		const int bit = (i == builder.ipSize - 1) ? 0 : 1;
		builder.add1TapeTransition(prevState, toState, ipStackTapeIndex, ".", std::to_string(bit), 1);

		prevState = toState;
	}
	
	// move head back to start of -2
	for(size_t i = 0; i < builder.ipSize; ++i) {
		const int toState = builder.newNode();
		builder.add1TapeTransition(prevState, toState, ipStackTapeIndex, ".", ".", -1);
		prevState = toState;
	}
	
	return prevState;
}

/**
 * Set all (ipSize) bits in IP to 0
 * Connect to node "sideways"
 */
void setInitialIP(MultiTapeBuilder &builder, int prevState) {
	const int ipTapeIndex = builder.tapeIndex("ip");
	for(size_t i = 0; i < builder.ipSize; ++i) {
		const int toState = builder.newNode();
		builder.add1TapeTransition(prevState, toState, ipTapeIndex, ".", "0", 1);
		prevState = toState;
	}

	// now move head back to beginning of IP (to the left)
	for(size_t i = 0; i < builder.ipSize; ++i) {
		const int toState = builder.newNode();
		builder.add1TapeTransition(prevState, toState, ipTapeIndex, ".", ".", -1);
		prevState = toState;
	}

	// connect node "sideways" to write 00000 in sideways
	builder.add1TapeTransition(prevState, builder.node("sideways"), ipTapeIndex, ".", ".", 0);
}

/**
 * When IP == -1, (aka IP in bits == 11111), 
 * transition from node "after" to node "end"
 */
void addExitClause(MultiTapeBuilder &builder) {	
	const int q0 = builder.node("after");
	const int q1 = builder.node("end");
	
	std::vector<std::pair<int, std::string> > reads;
	std::vector<std::pair<int, std::string> > writes;
	std::vector<std::pair<int, int> > shifts;
	
	const int sidewaysTapeIndex = builder.tapeIndex("ipSideways");
	
	for(size_t i = 0; i < builder.ipSize; ++i) {
		reads.emplace_back(sidewaysTapeIndex + i, "1");
	}
	
	// don't write anything
	// no shifts either
	builder.addTransition(q0, q1, reads, writes, shifts);
}

void initialize(MultiTapeBuilder &builder) {
	// initialize basic states
	builder.newNode("start");
	builder.newNode("end");
	builder.newNode("before");
	builder.newNode("after");
	builder.newNode("sideways");

	/*
	// push 0 into each varTape
	int prevState = builder.node("start");
	for(size_t i = 0; i < builder.numVars; ++i) {
		int tape = builder.tapeIndex("variables") + i;
		int q = builder.newNode();
		builder.add1TapeTransition(prevState, q, tape, ".", "0", 0);
		prevState = q;
	}
	*/
	
	// add nodes for incrementing IP between before and after
	addIncrementIP(builder);

	// put -2 into ip stack: so when main returns,
	// ip incremented from -2 to -1, which then we exit
	const int prevState = addNeg2ToIpStack(builder);

	// push 0 into IP, using p bits. Just 00000.
	// push 0 into ipSideways: just all 0 in each one.
	setInitialIP(builder, prevState);	
	
	// exit condition, aka when the TM halts, is when IP == -1:
	// in bits, IP is all 1: IP == 11111 (cuz 2's complement)
	addExitClause(builder);
}

/**
 * Add a transition from fromState to toState if IP == currIP
 */
void handleIPTransition(MultiTapeBuilder &builder, const size_t currIP, const int fromState, const int toState) {
	std::vector<std::pair<int, std::string> > reads;
	std::vector<std::pair<int, std::string> > writes;
	std::vector<std::pair<int, int> > shifts;

	std::vector<std::string> bits;
	size_t val = currIP;
	for(size_t i = 0; i < builder.ipSize; ++i) {
		bits.push_back(std::to_string(val % 2));
		val /= 2;
	}
	
	std::reverse(bits.begin(), bits.end());
	
	const int sidewaysTapeIndex = builder.tapeIndex("ipSideways");
	for(size_t i = 0; i < builder.ipSize; ++i) {
		reads.emplace_back(sidewaysTapeIndex + i, bits[i]);
	}

	// no writes, no shifts, just read the sideways tapes
	builder.addTransition(fromState, toState, reads, writes, shifts);
}

/**
 * Copy sth from fromTape toTape. Assumes there's space in toTape
 */
void copyBetweenTapes(MultiTapeBuilder &builder, const int fromTape, const int toTape, const int startNode, const int endNode) {
	std::vector<std::pair<int, std::string> > reads;
	std::vector<std::pair<int, std::string> > writes;
	std::vector<std::pair<int, int> > shifts;

	// from start node, if read a 0, write a 0 to toTape, move both right
	reads.emplace_back(fromTape, "0");
	writes.emplace_back(toTape, "0");
	shifts.emplace_back(fromTape, 1);
	shifts.emplace_back(toTape, 1);
	
	builder.addTransition(startNode, startNode, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();

	reads.emplace_back(fromTape, "1");
	writes.emplace_back(toTape, "1");
	shifts.emplace_back(fromTape, 1);
	shifts.emplace_back(toTape, 1);

	builder.addTransition(startNode, startNode, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();

	// once see a blank, YOU MUST WRITE A BLANK IN TOTAPE, move back left
	// must write cuz then signals end of number...like if ur overwriting rax or sth
	const int q2 = builder.newNode();
	reads.emplace_back(fromTape, "_");
	writes.emplace_back(toTape, "_");
	shifts.emplace_back(fromTape, -1);
	shifts.emplace_back(toTape, -1);
	
	builder.addTransition(startNode, q2, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();
		
	// move both back until see space
	reads.emplace_back(fromTape, "[01]");
	shifts.emplace_back(fromTape, -1);
	shifts.emplace_back(toTape, -1);

	builder.addTransition(q2, q2, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();
	
	// now that you both see space again, move right
	reads.emplace_back(fromTape, "_");
	shifts.emplace_back(fromTape, 1);
	shifts.emplace_back(toTape, 1);
	
	builder.addTransition(q2, endNode, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear(); 
}

/**
 * Goes to end of current num, and goes right 2 (so 1 blank in between)
 * Assume tape is already not empty
 */
void pushEmptyFrame(MultiTapeBuilder &builder, const int tape, const int startNode, const int endNode) {
	const int q1 = builder.newNode();
	const int q2 = builder.newNode();
	
	// if see [01], go to q1. else see blank: go to q2, and move right
	builder.add1TapeTransition(startNode, q1, tape, "[01]", ".", 0);
	builder.add1TapeTransition(startNode, q2, tape, "_", ".", 1);
	
	// q1: while see [01], move right. when see blank, move to endNode
	builder.add1TapeTransition(q1, q1, tape, "[01]", ".", 1);
	builder.add1TapeTransition(q1, endNode, tape, "_", ".", 1);
	
	// q2 just moves another right
	builder.add1TapeTransition(q2, endNode, tape, "_", ".", 1);
}

/**
 * Pop off num from stack
 */
void popOffTop(MultiTapeBuilder &builder, const int tape, const int startNode, const int endNode) {
	// startNode: current tape could be blank! so just move back two
	const int q1 = builder.newNode();
	const int mid = builder.newNode();
	
	builder.add1TapeTransition(startNode, q1, tape, "_", ".", -1);
	builder.add1TapeTransition(q1, mid, tape, ".", ".", -1);
	
	// startNode: otherwise, erase currentNum
	const int q3 = builder.newNode();
	builder.add1TapeTransition(startNode, q3, tape, "[01]", ".", 0);
	// q3 is where we go to end of num
	const int q4 = builder.newNode();
	builder.add1TapeTransition(q3, q3, tape, "[01]", ".", 1);
	builder.add1TapeTransition(q3, q4, tape, "_", ".", -1);
	// q4 is where we erase the num, and move left
	builder.add1TapeTransition(q4, q4, tape, "[01]", "_", -1);
	builder.add1TapeTransition(q4, mid, tape, "_", ".", -1);
	
	// ok now we are two left from where we first started on the tape
	// mid: current cell could be blank! if so, just move to endNode
	builder.add1TapeTransition(mid, endNode, tape, "_", ".", 0);
	
	// mid: else read a [01]. gotta go to beginning of num
	const int q5 = builder.newNode();
	builder.add1TapeTransition(mid, q5, tape, "[01]", ".", 0);
	builder.add1TapeTransition(q5, q5, tape, "[01]", ".", -1);
	builder.add1TapeTransition(q5, endNode, tape, "_", ".", 1);
}

/**
 * If assembly line is nop, and reach ip == currIP,
 * then connect node "after" to "before"
 */
void handleNop(MultiTapeBuilder &builder, const size_t currIP) {
	const int q0 = builder.node("before");
	const int q1 = builder.node("after");

	handleIPTransition(builder, currIP, q1, q0);
}

/**
 * Jmp statement.
 */
void handleJump(MultiTapeBuilder &builder, const size_t currIP, const std::vector<std::string> &words) {
	const int q0 = builder.newNode();
	handleIPTransition(builder, currIP, builder.node("after"), q0);
	
	int jumpLine = std::stoi(words[1]);
	
	std::vector<int> bits;
	while(jumpLine > 0) {
		bits.push_back(jumpLine % 2);
		jumpLine /= 2;
	}
	
	while(bits.size() < builder.ipSize) {
		bits.push_back(0);
	}
	
	std::reverse(bits.begin(), bits.end());

	int prevState = q0;
	const int ipTapeIndex = builder.tapeIndex("ip");
	for(size_t i = 0; i < builder.ipSize; ++i) {
		const int q = builder.newNode();
		builder.add1TapeTransition(prevState, q, ipTapeIndex, ".", std::to_string(bits[i]), 1);
		prevState = q0;
	}
	
	// move head of ipTapeIndex back to start
	for(size_t i = 0; i < builder.ipSize; ++i) {
		const int q = builder.newNode();
		const int shift = (i + 1 == builder.ipSize) ? 0 : -1;
		builder.add1TapeTransition(prevState, q, ipTapeIndex, ".", ".", shift);
		prevState = q;
	}
	
	// now connect to sideways
	builder.add1TapeTransition(prevState, builder.node("sideways"), ipTapeIndex, ".", ".", 0);
}

/**
 * Pop either from PARAMS or RAX; moved to a varTape
 */
void handlePop(MultiTapeBuilder &builder, const size_t currIP, const std::vector<std::string> &words) {
	int fromTape;
	bool fromParams = false;
	if(words[1] == "!TAPE_PARAMS") {
		fromParams = true;
		fromTape = builder.tapeIndex("paramStack");
	}
	else if(words[1] == "!TAPE_RAX") {
		fromTape = builder.tapeIndex("rax");
	}
	else {
		throw std::invalid_argument("Invalid request at " + std::to_string(currIP));
	}
	
	const int toTape = builder.tapeIndex("variables") + parseTapeNum(words[2]);
	
	// transition from node "after" to a new node, by reading currIP
	const int q0 = builder.node("after");
	const int q1 = builder.newNode();
	const int q2 = builder.newNode();
	const int endNode = builder.node("before");

	handleIPTransition(builder, currIP, q0, q1);
	
	// if from paramStack, erase from stack; else just move left. also move toTape's copy left
	if(fromParams) {
		// if from parameter stack, pop off, aka erase
		//pop1ThingFromStack(builder, fromTape, toTape, q1, endNode);
		copyBetweenTapes(builder, fromTape, toTape, q1, q2);
		popOffTop(builder, fromTape, q2, endNode);
	}
	else {
		copyBetweenTapes(builder, fromTape, toTape, q1, endNode);
	}

	// by here, everything connected to node "before"
}

/**
 * handling assembly code of pushing variable onto a stack
 */
void handlePush(MultiTapeBuilder &builder, const size_t currIP, const std::vector<std::string> &words) {
	const int q0 = builder.newNode();
	const int q1 = builder.newNode();
	const int endNode = builder.node("before");

	handleIPTransition(builder, currIP, builder.node("after"), q0);

	int fromTape;
	if(words[1].size() >= 10 && words[1].substr(0, 10) == "!TAPE_tape") {
		fromTape = builder.tapeIndex("variables") + parseTapeNum(words[1]);
	}
	else {
		throw std::invalid_argument("Invalid push at line " + std::to_string(currIP));
	}
	
	int toTape;
	if(words[2] == "!TAPE_PARAMS") {
		toTape = builder.tapeIndex("paramStack");
	}
	else {
		throw std::invalid_argument("Invalid push at line " + std::to_string(currIP));
	}

	//push1ThingToStack(builder, fromTape, toTape, q0, builder.node("before"));
	pushEmptyFrame(builder, toTape, q0, q1);
	copyBetweenTapes(builder, fromTape, toTape, q1, endNode);
}

/**
 * handle assembly code of checking whether value at tape is zero or not.
 * write 0 into rax if not zero; write a 1 into rax if it zero
 */
void handleIsZero(MultiTapeBuilder &builder, const int startNode, const int endNode) {
	// must pop from paramStack
	const int q0 = builder.newNode();
	const int q1 = builder.newNode();
	const int penultimateNode = builder.newNode();

	copyBetweenTapes(builder, builder.tapeIndex("paramStack"), builder.tapeIndex("variables"), startNode, q0);
	popOffTop(builder, builder.tapeIndex("paramStack"), q0, q1);
	
	// zero is just 0 followed by a blank
	const int tape0 = builder.tapeIndex("variables");
	const int tapeRax = builder.tapeIndex("rax");
	
	// q1: if see 1 or _, not a zero
	std::vector<std::pair<int, std::string> > reads;
	std::vector<std::pair<int, std::string> > writes;
	std::vector<std::pair<int, int> > shifts;
	
	reads.emplace_back(tape0, "[1_]");
	writes.emplace_back(tapeRax, "0");
	shifts.emplace_back(tapeRax, 1);
		
	builder.addTransition(q1, penultimateNode, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();
	
	// q1: otherwise saw a 0...move right to check if it blank
	const int q2 = builder.newNode();
	builder.add1TapeTransition(q1, q2, tape0, "0", ".", 1);

	// q2: if see a blank, write a 1 to rax, and move left
	reads.emplace_back(tape0, "_");
	writes.emplace_back(tapeRax, "1");
	shifts.emplace_back(tape0, -1);
	shifts.emplace_back(tapeRax, 1);
	
	builder.addTransition(q2, penultimateNode, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();
	
	// q2: else if see a [01], write a 0 to rax, and move left
	reads.emplace_back(tape0, "[01]");
	writes.emplace_back(tapeRax, "0");
	shifts.emplace_back(tape0, -1);
	shifts.emplace_back(tapeRax, 1);
	
	builder.addTransition(q2, penultimateNode, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();
	
	// penultimateNode: rax head currently one right than original. write a _, then move back left
	builder.add1TapeTransition(penultimateNode, endNode, tapeRax, ".", "_", -1);
}

/**
 * handle assembly code of checking whether value at paramStack is positive or not
 * write 0 into rax if not positive; write a 1 into rax if it is positive
 */
void handleIsPos(MultiTapeBuilder &builder, const int startNode, const int endNode) {
	// must pop from paramStack
	const int q0 = builder.newNode();
	const int q1 = builder.newNode();
	const int penultimateNode = builder.newNode();

	copyBetweenTapes(builder, builder.tapeIndex("paramStack"), builder.tapeIndex("variables"), startNode, q0);
	popOffTop(builder, builder.tapeIndex("paramStack"), q0, q1);

	// positive is when bits start with 0 then a [01]
	const int tape0 = builder.tapeIndex("variables");
	const int tapeRax = builder.tapeIndex("rax");

	// q1: if see 1 or _, not positive
	std::vector<std::pair<int, std::string> > reads;
	std::vector<std::pair<int, std::string> > writes;
	std::vector<std::pair<int, int> > shifts;

	reads.emplace_back(tape0, "[1_]");
	writes.emplace_back(tapeRax, "0");
	shifts.emplace_back(tapeRax, 1);
		
	builder.addTransition(q1, penultimateNode, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();

	// q1: else see 0: that's fine, move right
	const int q2 = builder.newNode();
	builder.add1TapeTransition(q1, q2, tape0, "0", ".", 1);
	
	// q2: if it's "_", not positive... write a 0 and move back left
	reads.emplace_back(tape0, "_");
	writes.emplace_back(tapeRax, "0");
	shifts.emplace_back(tape0, -1);
	shifts.emplace_back(tapeRax, 1);
	
	builder.addTransition(q2, penultimateNode, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();
	
	// q2: else reading a [01]: is positive. Write a 1 in rax, move back left
	reads.emplace_back(tape0, "[01]");
	writes.emplace_back(tapeRax, "1");
	shifts.emplace_back(tape0, -1);
	shifts.emplace_back(tapeRax, 1);
	
	builder.addTransition(q2, penultimateNode, reads, writes, shifts);
		
	// penultimateNode: currently shifted 1 right over from original on tapeRax. 
	// Write a blank, and move back left
	builder.add1TapeTransition(penultimateNode, endNode, tapeRax, ".", "_", -1);
}

/**
 * handle assembly code of checking whether value at paramStack is negative or not
 * write 0 into rax if not negative; write a 1 into rax if it is negative
 */
void handleIsNeg(MultiTapeBuilder &builder, const int startNode, const int endNode) {
	// must pop from paramStack
	const int q0 = builder.newNode();
	const int q1 = builder.newNode();
	const int penultimateNode = builder.newNode();

	copyBetweenTapes(builder, builder.tapeIndex("paramStack"), builder.tapeIndex("variables"), startNode, q0);
	popOffTop(builder, builder.tapeIndex("paramStack"), q0, q1);

	// positive is when bits start with 0 then a [01]
	const int tape0 = builder.tapeIndex("variables");
	const int tapeRax = builder.tapeIndex("rax");

	// q1: if see 0 or _, not negative
	std::vector<std::pair<int, std::string> > reads;
	std::vector<std::pair<int, std::string> > writes;
	std::vector<std::pair<int, int> > shifts;
	
	reads.emplace_back(tape0, "[0_]");
	writes.emplace_back(tapeRax, "0");
	shifts.emplace_back(tapeRax, 1);
		
	builder.addTransition(q1, penultimateNode, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();
	
	// q1: else see 1. Is negative!
	reads.emplace_back(tape0, "1");
	writes.emplace_back(tapeRax, "1");
	shifts.emplace_back(tapeRax, 1);
		
	builder.addTransition(q1, penultimateNode, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();
	
	// penultimateNode: since 1 off to the right of original index of tapeRax,
	// fill in a blank, and move left
	builder.add1TapeTransition(penultimateNode, endNode, tapeRax, ".", "_", -1);
}

/**
 * handle assembly code of adding top 2 values of paramStack
 * put sum in rax tape
 * No chance of having leading 0's in result
 * Make sure to put blank after answer in rax tho
 */
void handleBasicAdd(MultiTapeBuilder &builder, const int startNode, const int endNode) {
	const int q0 = builder.newNode();
	const int q1 = builder.newNode();
	const int q2 = builder.newNode();
	const int q3 = builder.newNode();

	const int tapeStack = builder.tapeIndex("paramStack");
	const int tape0 = builder.tapeIndex("variables");
	const int tape1 = tape0 + 1;
	const int tapeRax = builder.tapeIndex("rax");

	copyBetweenTapes(builder, tapeStack, tape0, startNode, q0);
	popOffTop(builder, tapeStack, q0, q1);
	copyBetweenTapes(builder, tapeStack, tape0 + 1, q1, q2);
	popOffTop(builder, tapeStack, q2, q3);

	// add the values in the two tapes. both are positive.
	const int carryOff = q3;
	const int carryOn = builder.newNode();

	std::vector<std::pair<int, std::string> > reads;
	std::vector<std::pair<int, std::string> > writes;
	std::vector<std::pair<int, int> > shifts;
	
	shifts.emplace_back(tape0, 1);
	shifts.emplace_back(tape1, 1);
	shifts.emplace_back(tapeRax, 1);

	const std::vector<std::string> symbols {"0", "1", "_"};
	const std::unordered_map<std::string, int> inherentValue {{"0", 0}, {"1", 1}, {"_", 0}};
	const std::vector<int> nodes {carryOff, carryOn};

	for(const std::string &s1 : symbols) {
		for(const std::string &s2 : symbols) {
			if(s1 == "_" && s2 == "_") {
				continue;
			}

			for(size_t i = 0; i < nodes.size(); ++i) {
				const int sum = ((int) i) + inherentValue.at(s1) + inherentValue.at(s2);	

				reads.emplace_back(tape0, s1);
				reads.emplace_back(tape1, s2);
				writes.emplace_back(tapeRax, std::to_string(sum % 2));

				const int toNode = (sum >= 2) ? carryOn : carryOff;
				builder.addTransition(nodes[i], toNode, reads, writes, shifts);

				reads.clear();
				writes.clear();
			}
		}
	}
	
	// now handle reading both _ and _ : reached end of input
	shifts.clear();

	// need to write _ when overwriting a tape
	reads.emplace_back(tape0, "_");
	reads.emplace_back(tape1, "_");
	writes.emplace_back(tapeRax, "_");
	shifts.emplace_back(tape0, -1);
	shifts.emplace_back(tape1, -1);
	shifts.emplace_back(tapeRax, -1);
		
	const int q4 = builder.newNode();
	builder.addTransition(carryOff, q4, reads, writes, shifts);
	builder.addTransition(carryOn, q4, reads, writes, shifts);
	reads.clear();
	writes.clear();
	
	// now keep moving left until read "_" and "_" again
	reads.emplace_back(tape0, "[01]");
	reads.emplace_back(tape1, "[01_]");

	builder.addTransition(q4, q4, reads, writes, shifts);
	
	reads.clear();
	reads.emplace_back(tape0, "[01_]");
	reads.emplace_back(tape1, "[01]");

	builder.addTransition(q4, q4, reads, writes, shifts);
	
	reads.clear();
	reads.emplace_back(tape0, "_");
	reads.emplace_back(tape1, "_");
	shifts.clear();
	shifts.emplace_back(tape0, 1);
	shifts.emplace_back(tape1, 1);
	shifts.emplace_back(tapeRax, 1);

	builder.addTransition(q4, endNode, reads, writes, shifts);
}

/**
 * handle assembly code of doing (A - B),
 * where A is first value popped,
 * B is second value popped
 */
void handleBasicSub(MultiTapeBuilder &builder, const int startNode, const int endNode) {
	const int q0 = builder.newNode();
	const int q1 = builder.newNode();
	const int q2 = builder.newNode();
	const int q3 = builder.newNode();

	const int tapeStack = builder.tapeIndex("paramStack");
	const int tape0 = builder.tapeIndex("variables");
	const int tape1 = tape0 + 1;
	const int tapeRax = builder.tapeIndex("rax");

	copyBetweenTapes(builder, tapeStack, tape0, startNode, q0);
	popOffTop(builder, tapeStack, q0, q1);
	copyBetweenTapes(builder, tapeStack, tape0 + 1, q1, q2);
	popOffTop(builder, tapeStack, q2, q3);

	std::vector<std::pair<int, std::string> > reads;
	std::vector<std::pair<int, std::string> > writes;
	std::vector<std::pair<int, int> > shifts;

	// pad end of shorter number with 0's
	shifts.emplace_back(tape0, 1);
	shifts.emplace_back(tape1, 1);

	reads.emplace_back(tape0, "[01]");
	reads.emplace_back(tape1, "[01]");

	builder.addTransition(q3, q3, reads, writes, shifts);
	reads.clear();
	
	reads.emplace_back(tape0, "[01]");
	reads.emplace_back(tape1, "_");
	writes.emplace_back(tape1, "0");
	
	builder.addTransition(q3, q3, reads, writes, shifts);
	reads.clear();
	writes.clear();

	reads.emplace_back(tape0, "_");
	reads.emplace_back(tape1, "[01]");
	writes.emplace_back(tape0, "0");

	builder.addTransition(q3, q3, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();

	// q3: if see _ and _, end. start going back left.
	const int q4 = builder.newNode();

	reads.emplace_back(tape0, "_");
	reads.emplace_back(tape1, "_");
	shifts.emplace_back(tape0, -1);
	shifts.emplace_back(tape1, -1);

	builder.addTransition(q3, q4, reads, writes, shifts);
	reads.clear();
	writes.clear();

	reads.emplace_back(tape0, "[01]");
	reads.emplace_back(tape1, "[01]");
	
	builder.addTransition(q4, q4, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();

	const int q5 = builder.newNode();
	reads.emplace_back(tape0, "_");
	reads.emplace_back(tape1, "_");
	shifts.emplace_back(tape0, 1);
	shifts.emplace_back(tape1, 1);

	builder.addTransition(q4, q5, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();
	
	// ok now back where we started from. now subtract!
	const int borrowOff = q5;
	const int borrowOn = builder.newNode();
	
	shifts.emplace_back(tape0, 1);
	shifts.emplace_back(tape1, 1);
	shifts.emplace_back(tapeRax, 1);

	// borrowOff: if see 0 and 0, answer is 0, go to borrowOff
	reads.emplace_back(tape0, "0");
	reads.emplace_back(tape1, "0");
	writes.emplace_back(tapeRax, "0");

	builder.addTransition(borrowOff, borrowOff, reads, writes, shifts);
	reads.clear();
	writes.clear();

	// borrowOff: if see 0 and 1, answer is 1, go to borrowOn
	reads.emplace_back(tape0, "0");
	reads.emplace_back(tape1, "1");
	writes.emplace_back(tapeRax, "1");
	
	builder.addTransition(borrowOff, borrowOn, reads, writes, shifts);
	reads.clear();
	writes.clear();
	
	// borrowOff: if see 1 and 0, answer is 1, go to borrowOff
	reads.emplace_back(tape0, "1");
	reads.emplace_back(tape1, "0");
	writes.emplace_back(tapeRax, "1");

	builder.addTransition(borrowOff, borrowOff, reads, writes, shifts);
	reads.clear();
	writes.clear();
	
	// borrowOff: if see 1 and ;, answer is 0, go to borrowOn
	reads.emplace_back(tape0, "1");
	reads.emplace_back(tape1, "1");
	writes.emplace_back(tapeRax, "0");

	builder.addTransition(borrowOff, borrowOff, reads, writes, shifts);
	reads.clear();
	writes.clear();
	
	// borrowOn: if see 0 and 0, answer is 1, go to borrowOn
	reads.emplace_back(tape0, "0");
	reads.emplace_back(tape1, "0");
	writes.emplace_back(tapeRax, "1");

	builder.addTransition(borrowOn, borrowOn, reads, writes, shifts);
	reads.clear();
	writes.clear();
	
	// borrowOn: if see 0 and 1, answer is 0, go to borrowOn
	reads.emplace_back(tape0, "0");
	reads.emplace_back(tape1, "1");
	writes.emplace_back(tapeRax, "0");

	builder.addTransition(borrowOn, borrowOn, reads, writes, shifts);
	reads.clear();
	writes.clear();
	
	// borrowOn: if see 1 and 0, answer is 0, go to borrowOff
	reads.emplace_back(tape0, "1");
	reads.emplace_back(tape1, "0");
	writes.emplace_back(tapeRax, "0");

	builder.addTransition(borrowOn, borrowOff, reads, writes, shifts);
	reads.clear();
	writes.clear();
	
	// borrowOn: if see 1 and 1, answer is 1, go to borrowOn
	reads.emplace_back(tape0, "1");
	reads.emplace_back(tape1, "1");
	writes.emplace_back(tapeRax, "1");

	builder.addTransition(borrowOn, borrowOn, reads, writes, shifts);
	reads.clear();
	writes.clear();
	
	shifts.clear();

	// now what if you read _ and _ from either borrowOn or borrowOff?
	const int q6 = builder.newNode();
	reads.emplace_back(tape0, "_");
	reads.emplace_back(tape1, "_");
	writes.emplace_back(tapeRax, "_");	// necessary to delimit ans
	shifts.emplace_back(tape0, -1);
	shifts.emplace_back(tape1, -1);
	shifts.emplace_back(tapeRax, -1);

	builder.addTransition(borrowOff, q6, reads, writes, shifts);
	builder.addTransition(borrowOn, q6, reads, writes, shifts);
	reads.clear();
	writes.clear();

	// ok now traverse all the way to the back again
	// IMPORTANT: must remove leading 0's on the right
	const int encountered1 = builder.newNode();
	reads.emplace_back(tape0, "[01]");
	reads.emplace_back(tape1, "[01]");
	reads.emplace_back(tapeRax, "0");
	writes.emplace_back(tapeRax, "_");
	
	builder.addTransition(q6, q6, reads, writes, shifts);
	reads.clear();
	writes.clear();
	
	reads.emplace_back(tape0, "[01]");
	reads.emplace_back(tape1, "[01]");
	reads.emplace_back(tapeRax, "1");

	builder.addTransition(q6, encountered1, reads, writes, shifts);
	reads.clear();
	writes.clear();

	reads.emplace_back(tape0, "[01]");
	reads.emplace_back(tape1, "[01]");

	builder.addTransition(encountered1, encountered1, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();

	reads.emplace_back(tape0, "_");
	reads.emplace_back(tape1, "_");
	shifts.emplace_back(tape0, 1);
	shifts.emplace_back(tape1, 1);
	shifts.emplace_back(tapeRax, 1);

	builder.addTransition(encountered1, endNode, reads, writes, shifts);
}

/**
 * handle assembly code of doing A xor B
 * where A is first value popped, B is second value popped
 */
void handleBasicXor(MultiTapeBuilder &builder, const int startNode, const int endNode) {
	const int q0 = builder.newNode();
	const int q1 = builder.newNode();
	const int q2 = builder.newNode();
	const int q3 = builder.newNode();

	const int tapeStack = builder.tapeIndex("paramStack");
	const int tape0 = builder.tapeIndex("variables");
	const int tape1 = tape0 + 1;
	const int tapeRax = builder.tapeIndex("rax");

	copyBetweenTapes(builder, tapeStack, tape0, startNode, q0);
	popOffTop(builder, tapeStack, q0, q1);
	copyBetweenTapes(builder, tapeStack, tape0 + 1, q1, q2);
	popOffTop(builder, tapeStack, q2, q3);

	std::vector<std::pair<int, std::string> > reads;
	std::vector<std::pair<int, std::string> > writes;
	std::vector<std::pair<int, int> > shifts;
	
	// pad end of shorter number with 0's
	shifts.emplace_back(tape0, 1);
	shifts.emplace_back(tape1, 1);

	reads.emplace_back(tape0, "[01]");
	reads.emplace_back(tape1, "[01]");

	builder.addTransition(q3, q3, reads, writes, shifts);
	reads.clear();
	
	reads.emplace_back(tape0, "[01]");
	reads.emplace_back(tape1, "_");
	writes.emplace_back(tape1, "0");
	
	builder.addTransition(q3, q3, reads, writes, shifts);
	reads.clear();
	writes.clear();

	reads.emplace_back(tape0, "_");
	reads.emplace_back(tape1, "[01]");
	writes.emplace_back(tape0, "0");

	builder.addTransition(q3, q3, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();
	
	// q3: if see _ and _, end. start going back left.
	const int q4 = builder.newNode();

	reads.emplace_back(tape0, "_");
	reads.emplace_back(tape1, "_");
	shifts.emplace_back(tape0, -1);
	shifts.emplace_back(tape1, -1);

	builder.addTransition(q3, q4, reads, writes, shifts);
	reads.clear();
	writes.clear();

	reads.emplace_back(tape0, "[01]");
	reads.emplace_back(tape1, "[01]");
	
	builder.addTransition(q4, q4, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();

	const int q5 = builder.newNode();
	reads.emplace_back(tape0, "_");
	reads.emplace_back(tape1, "_");
	shifts.emplace_back(tape0, 1);
	shifts.emplace_back(tape1, 1);

	builder.addTransition(q4, q5, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();
	
	// ok now back where we started from. now do bit-wise xor!
	shifts.emplace_back(tape0, 1);
	shifts.emplace_back(tape1, 1);
	shifts.emplace_back(tapeRax, 1);

	// if see 0 and 0, write a 0
	reads.emplace_back(tape0, "0");
	reads.emplace_back(tape1, "0");
	writes.emplace_back(tapeRax, "0");

	builder.addTransition(q5, q5, reads, writes, shifts);
	reads.clear();
	writes.clear();
	
	// if see 0 and 1, write a 1
	reads.emplace_back(tape0, "0");
	reads.emplace_back(tape1, "1");
	writes.emplace_back(tapeRax, "1");

	builder.addTransition(q5, q5, reads, writes, shifts);
	reads.clear();
	writes.clear();
	
	// if see 1 and 0, write a 1
	reads.emplace_back(tape0, "1");
	reads.emplace_back(tape1, "0");
	writes.emplace_back(tapeRax, "1");

	builder.addTransition(q5, q5, reads, writes, shifts);
	reads.clear();
	writes.clear();
	
	// if see 1 and 1, write a 0
	reads.emplace_back(tape0, "1");
	reads.emplace_back(tape1, "1");
	writes.emplace_back(tapeRax, "0");

	builder.addTransition(q5, q5, reads, writes, shifts);
	reads.clear();
	writes.clear();

	shifts.clear();
	// if see _ and _, write a _, and move left
	const int q6 = builder.newNode();

	reads.emplace_back(tape0, "_");
	reads.emplace_back(tape1, "_");
	writes.emplace_back(tapeRax, "_"); // to delimit answer
	shifts.emplace_back(tape0, -1);
	shifts.emplace_back(tape1, -1);
	shifts.emplace_back(tapeRax, -1);

	builder.addTransition(q5, q6, reads, writes, shifts);
	reads.clear();
	writes.clear();
		
	// now move back left. also turn trailing 0's on the right into blanks
	// this time tho, answer could be just 0...
	const int encountered1 = builder.newNode();
	const int penultimateNode = builder.newNode();

	// while see both 0/1, and haven't encountered 1 in tapeRax, replace 0 with _
	reads.emplace_back(tape0, "[01]");
	reads.emplace_back(tape1, "[01]");
	reads.emplace_back(tapeRax, "0");
	writes.emplace_back(tapeRax, "_");
		
	builder.addTransition(q6, q6, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();
	
	// if see 1 in tapeRax tho, go to state encountered1
	reads.emplace_back(tape0, "[01]");
	reads.emplace_back(tape1, "[01]");
	reads.emplace_back(tapeRax, "1");
		
	builder.addTransition(q6, encountered1, reads, writes, shifts);
	reads.clear();
	writes.clear();

	// encountered1: while both see 0/1, go left
	reads.emplace_back(tape0, "[01]");
	reads.emplace_back(tape1, "[01]");
		
	builder.addTransition(encountered1, encountered1, reads, writes, shifts);
	reads.clear();
	writes.clear();
	
	// if q6 or encountered1 sees _ and _, go to node penultimateNode
	shifts.clear();
	shifts.emplace_back(tape0, 1);
	shifts.emplace_back(tape1, 1);
	shifts.emplace_back(tapeRax, 1);

	reads.emplace_back(tape0, "_");
	reads.emplace_back(tape1, "_");
	
	builder.addTransition(q6, penultimateNode, reads, writes, shifts);
	builder.addTransition(encountered1, penultimateNode, reads, writes, shifts);
	
	// now all heads in original spot. one last thing tho:
	// what if tapeRax head currently shows blank, cuz we removed leading 0's?
	// write a 0!
	// (otherwise don't write)
	builder.add1TapeTransition(penultimateNode, endNode, tapeRax, "_", "0", 0);
	builder.add1TapeTransition(penultimateNode, endNode, tapeRax, "[01]", ".", 0);
}

/**
 * handle assembly code of doing (A == B)
 * where A is first value popped, B is second value popped
 * can assume both have no leading 0's, and both positive
 */
void handleBasicEq(MultiTapeBuilder &builder, const int startNode, const int endNode) {
	const int q0 = builder.newNode();
	const int q1 = builder.newNode();
	const int q2 = builder.newNode();
	const int q3 = builder.newNode();

	const int tapeStack = builder.tapeIndex("paramStack");
	const int tape0 = builder.tapeIndex("variables");
	const int tape1 = tape0 + 1;
	const int tapeRax = builder.tapeIndex("rax");

	copyBetweenTapes(builder, tapeStack, tape0, startNode, q0);
	popOffTop(builder, tapeStack, q0, q1);
	copyBetweenTapes(builder, tapeStack, tape0 + 1, q1, q2);
	popOffTop(builder, tapeStack, q2, q3);

	std::vector<std::pair<int, std::string> > reads;
	std::vector<std::pair<int, std::string> > writes;
	std::vector<std::pair<int, int> > shifts;
	
	// ok now back where we started from. now check for equality!

	// when have written ans, but tape0 and tape1 heads need to go back left
	const int moveBackLeft = builder.newNode();

	// have to do all 9 cases for reading "0","1",or "_" on both tapes
	// if current bits same, move on. otherwise, right a 0 in rax,
	// and go to moveBackLeft
	
	const std::vector<std::string> symbols {"0", "1", "_"};
	
	for(size_t i = 0; i < symbols.size(); ++i) {
		for(size_t j = 0; j < symbols.size(); ++j) {
			const std::string s1 = symbols[i];
			const std::string s2 = symbols[j];

			reads.emplace_back(tape0, s1);
			reads.emplace_back(tape1, s2);

			if(s1 == "_" && s2 == "_") {
				// if you made it here, all bits same. is equal!
				writes.emplace_back(tapeRax, "1");
				shifts.emplace_back(tape0, -1);
				shifts.emplace_back(tape1, -1);

				builder.addTransition(q3, moveBackLeft, reads, writes, shifts);
			}
			else if(s1 == s2) {
				// these two bits are the same, continue on
				shifts.emplace_back(tape0, 1);
				shifts.emplace_back(tape1, 1);

				builder.addTransition(q3, q3, reads, writes, shifts);
			}
			else {
				// these two bits are not the same... not equal!
				// since both values positive, won't shift out of bounds
				writes.emplace_back(tapeRax, "0");
				shifts.emplace_back(tape0, -1);
				shifts.emplace_back(tape1, -1);

				builder.addTransition(q3, moveBackLeft, reads, writes, shifts);
			}

			reads.clear();
			writes.clear();
			shifts.clear();
		}
	}
	
	// moveBackLeft: while see [01] on both tapes, go back left
	reads.emplace_back(tape0, "[01]");
	reads.emplace_back(tape1, "[01]");
	shifts.emplace_back(tape0, -1);
	shifts.emplace_back(tape1, -1);
	
	builder.addTransition(moveBackLeft, moveBackLeft, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();

	// moveBackLeft: when see _ on both tapes, go back right to original spot
	reads.emplace_back(tape0, "_");
	reads.emplace_back(tape1, "_");
	shifts.emplace_back(tape0, 1);
	shifts.emplace_back(tape1, 1);
	
	builder.addTransition(moveBackLeft, endNode, reads, writes, shifts);
}

/**
 * handle assembly code of doing (A < B)
 * where A is first value popped, B is second value popped
 * can assume both have no leading 0's, and both positive
 */
void handleBasicLt(MultiTapeBuilder &builder, const int startNode, const int endNode) {
	const int q0 = builder.newNode();
	const int q1 = builder.newNode();
	const int q2 = builder.newNode();
	const int q3 = builder.newNode();

	const int tapeStack = builder.tapeIndex("paramStack");
	const int tape0 = builder.tapeIndex("variables");
	const int tape1 = tape0 + 1;
	const int tapeRax = builder.tapeIndex("rax");

	copyBetweenTapes(builder, tapeStack, tape0, startNode, q0);
	popOffTop(builder, tapeStack, q0, q1);
	copyBetweenTapes(builder, tapeStack, tape0 + 1, q1, q2);
	popOffTop(builder, tapeStack, q2, q3);

	std::vector<std::pair<int, std::string> > reads;
	std::vector<std::pair<int, std::string> > writes;
	std::vector<std::pair<int, int> > shifts;
	
	// pad end of shorter number with 0's
	shifts.emplace_back(tape0, 1);
	shifts.emplace_back(tape1, 1);

	reads.emplace_back(tape0, "[01]");
	reads.emplace_back(tape1, "[01]");

	builder.addTransition(q3, q3, reads, writes, shifts);
	reads.clear();
	
	reads.emplace_back(tape0, "[01]");
	reads.emplace_back(tape1, "_");
	writes.emplace_back(tape1, "0");
	
	builder.addTransition(q3, q3, reads, writes, shifts);
	reads.clear();
	writes.clear();

	reads.emplace_back(tape0, "_");
	reads.emplace_back(tape1, "[01]");
	writes.emplace_back(tape0, "0");

	builder.addTransition(q3, q3, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();
	
	// q3: if see _ and _, end going right. Then do comparisons!
	const int q4 = builder.newNode();

	reads.emplace_back(tape0, "_");
	reads.emplace_back(tape1, "_");
	shifts.emplace_back(tape0, -1);
	shifts.emplace_back(tape1, -1);

	builder.addTransition(q3, q4, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();

	// ok now back where we started from. now do less-than!
	// starting from right side: that's where significant bits are
	// so start comparison here
	
	// node to signal when to start moving both tape heads back left
	const int moveBackLeft = builder.newNode();

	// read 0 and 0: keep going left
	reads.emplace_back(tape0, "0");
	reads.emplace_back(tape1, "0");
	shifts.emplace_back(tape0, -1);
	shifts.emplace_back(tape1, -1);

	builder.addTransition(q4, q4, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();

	// read 0 and 1: A < B, so write 1 to rax. Go to moveBackLeft
	reads.emplace_back(tape0, "0");
	reads.emplace_back(tape1, "1");
	writes.emplace_back(tapeRax, "1");

	builder.addTransition(q4, moveBackLeft, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();
	
	// read 1 and 0: A > B, so write 0 to rax. Go to moveBackLeft
	reads.emplace_back(tape0, "1");
	reads.emplace_back(tape1, "0");
	writes.emplace_back(tapeRax, "0");

	builder.addTransition(q4, moveBackLeft, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();

	// read 1 and 1: keep going left
	reads.emplace_back(tape0, "1");
	reads.emplace_back(tape1, "1");
	shifts.emplace_back(tape0, -1);
	shifts.emplace_back(tape1, -1);

	builder.addTransition(q4, q4, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();

	// read _ and _: well A == B. Write 0 to rax, go to endNode
	reads.emplace_back(tape0, "_");
	reads.emplace_back(tape1, "_");
	writes.emplace_back(tapeRax, "0");
	shifts.emplace_back(tape0, 1);
	shifts.emplace_back(tape1, 1);

	builder.addTransition(q4, endNode, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();

	// moveBackLeft: while reading [01] on both tapes, move left
	reads.emplace_back(tape0, "[01]");
	reads.emplace_back(tape1, "[01]");
	shifts.emplace_back(tape0, -1);
	shifts.emplace_back(tape1, -1);
	
	builder.addTransition(moveBackLeft, moveBackLeft, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();

	// when go to far left, move back right 1
	reads.emplace_back(tape0, "_");
	reads.emplace_back(tape1, "_");
	shifts.emplace_back(tape0, 1);
	shifts.emplace_back(tape1, 1);

	builder.addTransition(moveBackLeft, endNode, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();
}

/**
 * handle assembly code of doing (-x)
 * where x is value popped from paramStack
 * Can assume x is non-zero
 */
void handleBasicNeg(MultiTapeBuilder &builder, const int startNode, const int endNode) {
	const int q0 = builder.newNode();
	const int q1 = builder.newNode();

	const int tapeStack = builder.tapeIndex("paramStack");
	const int tape0 = builder.tapeIndex("variables");
	const int tapeRax = builder.tapeIndex("rax");

	copyBetweenTapes(builder, tapeStack, tape0, startNode, q0);
	popOffTop(builder, tapeStack, q0, q1);

	std::vector<std::pair<int, std::string> > reads;
	std::vector<std::pair<int, std::string> > writes;
	std::vector<std::pair<int, int> > shifts;
	
	// ok now back where we started from. now flip sign bit
	// sign bit is the cell the head is currently on!
	const int q2 = builder.newNode();
	builder.add1TapeTransition(q1, q2, tape0, "0", "1", 0);
	builder.add1TapeTransition(q1, q2, tape0, "1", "0", 0);

	copyBetweenTapes(builder, tape0, tapeRax, q2, endNode);
}

/**
 * handle assembly code of putting tape bitIndex value into rax
 */
void handleGetMemBitIndex(MultiTapeBuilder &builder, const int startNode, const int endNode) {
	const int tapeBitIndex = builder.tapeIndex("bitIndex");
	const int tapeRax = builder.tapeIndex("rax");

	copyBetweenTapes(builder, tapeBitIndex, tapeRax, startNode, endNode);
}

/**
 * handle assembly code of putting a value into tape bitIndex
 */
void handleSetMemBitIndex(MultiTapeBuilder &builder, const int startNode, const int endNode) {
	const int q0 = builder.newNode();

	const int tapeStack = builder.tapeIndex("paramStack");
	const int tapeBitIndex = builder.tapeIndex("bitIndex");
	const int tapeRax = builder.tapeIndex("rax");

	copyBetweenTapes(builder, tapeStack, tapeBitIndex, startNode, q0);
	popOffTop(builder, tapeStack, q0, endNode);
}

/**
 * handle assembly code of moving head of tape "bits" right
 */
void handleMoveMemHeadRight(MultiTapeBuilder &builder, const int startNode, const int endNode) {
	builder.add1TapeTransition(startNode, endNode, builder.tapeIndex("bitIndex"), ".", ".", 1);
}

/**
 * handle assembly code of moving head of tape "bits" left
 */
void handleMoveMemHeadLeft(MultiTapeBuilder &builder, const int startNode, const int endNode) {
	builder.add1TapeTransition(startNode, endNode, builder.tapeIndex("bitIndex"), ".", ".", -1);
}

/**
 * handle assembly code of moving head of setting bit of tape "bits" to 0
 */
void handleSetMemBitZero(MultiTapeBuilder &builder, const int startNode, const int endNode) {
	builder.add1TapeTransition(startNode, endNode, builder.tapeIndex("bitIndex"), ".", "0", 0);
}

/**
 * handle assembly code of moving head of setting bit of tape "bits" to 1
 */
void handleSetMemBitOne(MultiTapeBuilder &builder, const int startNode, const int endNode) {
	builder.add1TapeTransition(startNode, endNode, builder.tapeIndex("bitIndex"), ".", "1", 0);
}

/**
 * handle assembly code of moving head of setting bit of tape "bits" to blank
 */
void handleSetMemBitBlank(MultiTapeBuilder &builder, const int startNode, const int endNode) {
	builder.add1TapeTransition(startNode, endNode, builder.tapeIndex("bitIndex"), ".", "_", 0);
}

/**
 * handle assembly code of checking if bit at head of tape "bits" is 0
 */
void handleMemBitIsZero(MultiTapeBuilder &builder, const int startNode, const int endNode) {
	const int tapeBitIndex = builder.tapeIndex("bitIndex");
	const int tapeRax = builder.tapeIndex("rax");

	std::vector<std::pair<int, std::string> > reads;
	std::vector<std::pair<int, std::string> > writes;
	std::vector<std::pair<int, int> > shifts;
	
	const int q = builder.newNode();

	reads.emplace_back(tapeBitIndex, "0");
	writes.emplace_back(tapeRax, "1");
	shifts.emplace_back(tapeRax, 1);
	
	builder.addTransition(startNode, q, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();
	
	reads.emplace_back(tapeBitIndex, "[1_]");
	writes.emplace_back(tapeRax, "0");
	shifts.emplace_back(tapeRax, 1);
	
	builder.addTransition(startNode, q, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();
	
	// have shifted 1 right from original on tapeRax. Write a blank and move left
	builder.add1TapeTransition(q, endNode, tapeRax, ".", "_", -1);
}

/**
 * handle assembly code of checking if bit at head of tape "bits" is 1
 */
void handleMemBitIsOne(MultiTapeBuilder &builder, const int startNode, const int endNode) {
	const int tapeBitIndex = builder.tapeIndex("bitIndex");
	const int tapeRax = builder.tapeIndex("rax");

	std::vector<std::pair<int, std::string> > reads;
	std::vector<std::pair<int, std::string> > writes;
	std::vector<std::pair<int, int> > shifts;
	
	const int q = builder.newNode();

	reads.emplace_back(tapeBitIndex, "1");
	writes.emplace_back(tapeRax, "1");
	shifts.emplace_back(tapeRax, 1);
	
	builder.addTransition(startNode, q, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();
	
	reads.emplace_back(tapeBitIndex, "[0_]");
	writes.emplace_back(tapeRax, "0");
	shifts.emplace_back(tapeRax, 1);
	
	builder.addTransition(startNode, q, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();
	
	// have moved right 1 from original on tapeRax. Write blank, then move left.
	builder.add1TapeTransition(q, endNode, tapeRax, ".", "_", -1);
}

/**
 * handle assembly code of checking if bit at head of tape "bits" is bank
 */
void handleMemBitIsBlank(MultiTapeBuilder &builder, const int startNode, const int endNode) {
	const int tapeBitIndex = builder.tapeIndex("bitIndex");
	const int tapeRax = builder.tapeIndex("rax");

	std::vector<std::pair<int, std::string> > reads;
	std::vector<std::pair<int, std::string> > writes;
	std::vector<std::pair<int, int> > shifts;
	
	const int q = builder.newNode();

	reads.emplace_back(tapeBitIndex, "_");
	writes.emplace_back(tapeRax, "1");
	shifts.emplace_back(tapeRax, 1);
	
	builder.addTransition(startNode, q, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();
	
	reads.emplace_back(tapeBitIndex, "[01]");
	writes.emplace_back(tapeRax, "0");
	shifts.emplace_back(tapeRax, 1);
	
	builder.addTransition(startNode, q, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();
	
	// have moved 1 right from original on tapeRax. Write blank, then move left
	builder.add1TapeTransition(q, endNode, tapeRax, ".", "_", -1);
}

/**
 * handle assembly code of getting next integer from input tape
 */
void handleNextInt(MultiTapeBuilder &builder, const int startNode, const int endNode) {
	const int tapeInput = builder.tapeIndex("input");
	const int tapeRax = builder.tapeIndex("rax");

	std::vector<std::pair<int, std::string> > reads;
	std::vector<std::pair<int, std::string> > writes;
	std::vector<std::pair<int, int> > shifts;
	
	reads.emplace_back(tapeInput, "0");
	writes.emplace_back(tapeRax, "0");
	shifts.emplace_back(tapeInput, 1);
	shifts.emplace_back(tapeRax, 1);

	builder.addTransition(startNode, startNode, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();
	
	reads.emplace_back(tapeInput, "1");
	writes.emplace_back(tapeRax, "1");
	shifts.emplace_back(tapeInput, 1);
	shifts.emplace_back(tapeRax, 1);

	builder.addTransition(startNode, startNode, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();
	
	// once reach blank in input, tapeInput goes right; rax goes left
	reads.emplace_back(tapeInput, "_");
	writes.emplace_back(tapeRax, "_");
	shifts.emplace_back(tapeInput, 1);
	shifts.emplace_back(tapeRax, -1);

	const int q0 = builder.newNode();
	builder.addTransition(startNode, q0, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();

	// now move back left for rax
	builder.add1TapeTransition(q0, q0, tapeRax, "[01]", ".", -1);
	builder.add1TapeTransition(q0, endNode, tapeRax, "_", ".", 1);
}

/**
 * handle assembly code of printing a space
 */
void handlePrintSpace(MultiTapeBuilder &builder, const int startNode, const int endNode) {
	builder.add1TapeTransition(startNode, endNode, builder.tapeIndex("output"), ".", "_", 1);
}

/**
 * handle assembly code of printing an int
 */
void handlePrintInt(MultiTapeBuilder &builder, const int startNode, const int endNode) {
	const int q0 = builder.newNode();
	const int q1 = builder.newNode();

	const int tapeStack = builder.tapeIndex("paramStack");
	const int tapeOutput = builder.tapeIndex("output");

	copyBetweenTapes(builder, tapeStack, tapeOutput, startNode, q0);
	popOffTop(builder, tapeStack, q0, q1);

	// currently at left of num in output. move right until blank
	builder.add1TapeTransition(q1, endNode, tapeOutput, "[01]", ".", 1);
}

/**
 * handle assembly code of call to a line number
 */
void handleCallNum(MultiTapeBuilder &builder, const size_t currIP, const std::vector<std::string> &words) {
	const int lineNum = std::stoi(words[1]);

	const int q0 = builder.newNode();
	handleIPTransition(builder, currIP, builder.node("after"), q0);

	int prevState = q0;
	// then push new stack frames to all varTapes
	for(size_t i = 0; i < builder.numVars; ++i) {
		const int tape = builder.tapeIndex("variables") + i;
		const int q = builder.newNode();
		pushEmptyFrame(builder, tape, prevState, q);
		prevState = q;
	}

	// store currIP into ipStack
	const int q1 = builder.newNode();
	const int q2 = builder.newNode();
	pushEmptyFrame(builder, builder.tapeIndex("ipStack"), prevState, q1);
	copyBetweenTapes(builder, builder.tapeIndex("ip"), builder.tapeIndex("ipStack"), q1, q2);
	
	// set new ip
	std::vector<std::string> bits;
	size_t val = currIP;
	for(size_t i = 0; i < builder.ipSize; ++i) {
		bits.push_back(std::to_string(val % 2));
		val /= 2;
	}
	
	std::reverse(bits.begin(), bits.end());
		
	// write bits into ip tape
	prevState = q2;
	int tapeIP = builder.tapeIndex("ip");
	for(size_t i = 0; i < builder.ipSize; ++i) {
		const int q = builder.newNode();
		builder.add1TapeTransition(prevState, q, tapeIP, ".", bits[i], 1);
		prevState = q;
	}
	
	// well now move head of ip tape back
	for(size_t i = 0; i < builder.ipSize; ++i) {
		const int q = builder.newNode();
		builder.add1TapeTransition(prevState, q, tapeIP, ".", ".", -1);
		prevState = q;
	}
	
	// now connect with node "sideways"
	builder.add1TapeTransition(prevState, builder.tapeIndex("sideways"), tapeIP, ".", ".", 0);
}

/**
 * handle assembly code of calling "jf <tape> <line#>": 
 * jump to line# if tape value is false (which is bit 0)
 */
void handleJf(MultiTapeBuilder &builder, const size_t currIP, const std::vector<std::string> &words) {
	const int q0 = builder.newNode();
	handleIPTransition(builder, currIP, builder.node("after"), q0);

	int valTape;
	if(words[1] == "!TAPE_RAX") {
		valTape = builder.tapeIndex("rax");
	}
	else if(words[1].size() >= 10 && words[1].substr(0, 10) == "!TAPE_tape") {
		valTape = builder.tapeIndex("variables") + parseTapeNum(words[1]);
	}
	else {
		throw std::invalid_argument("Invalid line " + std::to_string(currIP));
	}
	
	const int lineNum = std::stoi(words[2]);
	
	// q0: if read a 0, go to q1
	const int q1 = builder.newNode();
	builder.add1TapeTransition(q0, q1, valTape, "0", ".", 0);
	
	// q0: if u read a 1 (or blank), go to node "before"
	builder.add1TapeTransition(q0, builder.node("before"), valTape, "[1_]", ".", 0);
	
	// q1: well valTape had a false. transition to new line!
	// aka we set ipTape to lineNum
	
	// set new ip
	std::vector<std::string> bits;
	size_t val = currIP;
	for(size_t i = 0; i < builder.ipSize; ++i) {
		bits.push_back(std::to_string(val % 2));
		val /= 2;
	}
	
	std::reverse(bits.begin(), bits.end());
		
	// write bits into ip tape
	int prevState = q1;
	const int tapeIP = builder.tapeIndex("ip");
	for(size_t i = 0; i < builder.ipSize; ++i) {
		const int q = builder.newNode();
		builder.add1TapeTransition(prevState, q, tapeIP, ".", bits[i], 1);
		prevState = q;
	}
	
	// well now move head of ip tape back
	for(size_t i = 0; i < builder.ipSize; ++i) {
		const int q = builder.newNode();
		builder.add1TapeTransition(prevState, q, tapeIP, ".", ".", -1);
		prevState = q;
	}
	
	// now connect with node "sideways"
	builder.add1TapeTransition(prevState, builder.tapeIndex("sideways"), tapeIP, ".", ".", 0);
}

/**
 * handle assembly code of calling return
 */
void handleReturn(MultiTapeBuilder &builder, const size_t currIP) {
	// first transition by reading ip
	const int q0 = builder.newNode();
	handleIPTransition(builder, currIP, builder.node("after"), q0);

	int prevState = q0;

	// pop off all the pushed stack frames
	for(size_t i = 0; i < builder.numVars; ++i) {
		const int tape = builder.tapeIndex("variables") + i;
		const int q = builder.newNode();
		popOffTop(builder, tape, prevState, q);
		prevState = q;
	}

	// pop last value of ipStack into ip
	const int tapeIPStack = builder.tapeIndex("ipStack");
	const int tapeIP = builder.tapeIndex("ip");
	const int q1 = builder.newNode();

	copyBetweenTapes(builder, tapeIPStack, tapeIP, prevState, q1);
	popOffTop(builder, tapeIPStack, q1, builder.node("before"));

	// ip must be incremented, so connected to node "before"
}

/**
 * handle assembly code of not of a boolean
 */
void handleNot(MultiTapeBuilder &builder, const size_t currIP, const std::vector<std::string> words) {
	// arg ! = dest ; 
	const int argTape = parseTapeNum(words[0]);
	const int destTape = parseTapeNum(words[3]);

	// first transition by reading ip
	const int q0 = builder.newNode();
	handleIPTransition(builder, currIP, builder.node("after"), q0);

	std::vector<std::pair<int, std::string> > reads;
	std::vector<std::pair<int, std::string> > writes;
	std::vector<std::pair<int, int> > shifts;

	const int penultimateNode = builder.newNode();

	reads.emplace_back(argTape, "0");
	writes.emplace_back(destTape, "1");
	shifts.emplace_back(destTape, 1);
	
	builder.addTransition(q0, penultimateNode, reads, writes, shifts);

	reads.clear();
	writes.clear();
	shifts.clear();

	reads.emplace_back(argTape, "1");
	writes.emplace_back(destTape, "0");
	shifts.emplace_back(destTape, 1);

	builder.addTransition(q0, penultimateNode, reads, writes, shifts);

	reads.clear();
	writes.clear();
	shifts.clear();

	const int endNode = builder.node("before");
	
	// once written answer to dest, write a blank after, then move back left
	builder.add1TapeTransition(penultimateNode, endNode, destTape, ".", "_", -1);
}

/**
 * handle assembly code of AND of 2 booleans
 */
void handleAnd(MultiTapeBuilder &builder, const size_t currIP, const std::vector<std::string> words) {
	// arg1 arg2 && = dest ; 
	const int arg1Tape = parseTapeNum(words[0]);
	const int arg2Tape = parseTapeNum(words[1]);
	const int destTape = parseTapeNum(words[4]);

	// first transition by reading ip
	const int q0 = builder.newNode();
	handleIPTransition(builder, currIP, builder.node("after"), q0);

	std::vector<std::pair<int, std::string> > reads;
	std::vector<std::pair<int, std::string> > writes;
	std::vector<std::pair<int, int> > shifts;

	const int penultimateNode = builder.newNode();

	reads.emplace_back(arg1Tape, "0");
	reads.emplace_back(arg2Tape, "0");
	writes.emplace_back(destTape, "0");
	shifts.emplace_back(destTape, 1);
	
	builder.addTransition(q0, penultimateNode, reads, writes, shifts);

	reads.clear();
	writes.clear();
	shifts.clear();

	reads.emplace_back(arg1Tape, "0");
	reads.emplace_back(arg2Tape, "1");
	writes.emplace_back(destTape, "0");
	shifts.emplace_back(destTape, 1);
	
	builder.addTransition(q0, penultimateNode, reads, writes, shifts);

	reads.clear();
	writes.clear();
	shifts.clear();

	reads.emplace_back(arg1Tape, "1");
	reads.emplace_back(arg2Tape, "0");
	writes.emplace_back(destTape, "0");
	shifts.emplace_back(destTape, 1);
	
	builder.addTransition(q0, penultimateNode, reads, writes, shifts);

	reads.clear();
	writes.clear();
	shifts.clear();
	
	reads.emplace_back(arg1Tape, "1");
	reads.emplace_back(arg2Tape, "1");
	writes.emplace_back(destTape, "1");
	shifts.emplace_back(destTape, 1);
	
	builder.addTransition(q0, penultimateNode, reads, writes, shifts);

	reads.clear();
	writes.clear();
	shifts.clear();

	const int endNode = builder.node("before");
	
	// once written answer to dest, write a blank after, then move back left
	builder.add1TapeTransition(penultimateNode, endNode, destTape, ".", "_", -1);
}

/**
 * handle assembly code of OR of 2 booleans
 */
void handleOr(MultiTapeBuilder &builder, const size_t currIP, const std::vector<std::string> &words) {
	// arg1 arg2 && = dest ; 
	const int arg1Tape = parseTapeNum(words[0]);
	const int arg2Tape = parseTapeNum(words[1]);
	const int destTape = parseTapeNum(words[4]);

	// first transition by reading ip
	const int q0 = builder.newNode();
	handleIPTransition(builder, currIP, builder.node("after"), q0);

	std::vector<std::pair<int, std::string> > reads;
	std::vector<std::pair<int, std::string> > writes;
	std::vector<std::pair<int, int> > shifts;

	const int penultimateNode = builder.newNode();

	reads.emplace_back(arg1Tape, "0");
	reads.emplace_back(arg2Tape, "0");
	writes.emplace_back(destTape, "0");
	shifts.emplace_back(destTape, 1);
	
	builder.addTransition(q0, penultimateNode, reads, writes, shifts);

	reads.clear();
	writes.clear();
	shifts.clear();

	reads.emplace_back(arg1Tape, "0");
	reads.emplace_back(arg2Tape, "1");
	writes.emplace_back(destTape, "1");
	shifts.emplace_back(destTape, 1);
	
	builder.addTransition(q0, penultimateNode, reads, writes, shifts);

	reads.clear();
	writes.clear();
	shifts.clear();

	reads.emplace_back(arg1Tape, "1");
	reads.emplace_back(arg2Tape, "0");
	writes.emplace_back(destTape, "1");
	shifts.emplace_back(destTape, 1);
	
	builder.addTransition(q0, penultimateNode, reads, writes, shifts);

	reads.clear();
	writes.clear();
	shifts.clear();
	
	reads.emplace_back(arg1Tape, "1");
	reads.emplace_back(arg2Tape, "1");
	writes.emplace_back(destTape, "1");
	shifts.emplace_back(destTape, 1);
	
	builder.addTransition(q0, penultimateNode, reads, writes, shifts);

	reads.clear();
	writes.clear();
	shifts.clear();

	const int endNode = builder.node("before");
	
	// once written answer to dest, write a blank after, then move back left
	builder.add1TapeTransition(penultimateNode, endNode, destTape, ".", "_", -1);
}

/**
 * handle assignment, but between 2 tapes. Like "tape1 tape2 = ;", 
 * meaning copy value of tape2 to tape1
 */
void handleAssignmentBetweenTapes(MultiTapeBuilder &builder, const size_t currIP, const std::vector<std::string> &words) {
	// first transition by reading ip
	const int q0 = builder.newNode();
	handleIPTransition(builder, currIP, builder.node("after"), q0);

	int fromTape;
	if(words[1] == "!TAPE_RAX") {
		fromTape = builder.tapeIndex("rax");
	}
	else if(words[1].size() >= 10 && words[1].substr(0, 10) == "!TAPE_tape") {
		fromTape = builder.tapeIndex("variables") + parseTapeNum(words[1]);
	}
	else {
		throw std::invalid_argument("Invalid line " + std::to_string(currIP));
	}

	int toTape;
	if(words[0] == "!TAPE_RAX") {
		toTape = builder.tapeIndex("rax");
	}
	else if(words[0].size() >= 10 && words[0].substr(0, 10) == "!TAPE_tape") {
		toTape = builder.tapeIndex("variables") + parseTapeNum(words[0]);
	}
	else{
		throw std::invalid_argument("Invalid line " + std::to_string(currIP));
	}

	const int endNode = builder.node("before");
	copyBetweenTapes(builder, fromTape, toTape, q0, endNode);
}

/**
 * handle assignment from "true" to a tape
 */
void handleAssignmentTrue(MultiTapeBuilder &builder, const size_t currIP, const std::vector<std::string> &words) {
	// first transition by reading ip
	const int q0 = builder.newNode();
	handleIPTransition(builder, currIP, builder.node("after"), q0);

	int toTape;
	if(words[0] == "!TAPE_RAX") {
		toTape = builder.tapeIndex("rax");
	}
	else if(words[0].size() >= 10 && words[0].substr(0, 10) == "!TAPE_tape") {
		toTape = builder.tapeIndex("variables") + parseTapeNum(words[0]);
	}
	else {
		throw std::invalid_argument("Invalid line " + std::to_string(currIP));
	}

	const int q1 = builder.newNode();
	const int endNode = builder.node("before");

	builder.add1TapeTransition(q0, q1, toTape, ".", "1", 1);
	builder.add1TapeTransition(q1, endNode, toTape, ".", "_", -1);
}

/**
 * handle assignment from "false" to a tape
 */
void handleAssignmentFalse(MultiTapeBuilder &builder, const size_t currIP, const std::vector<std::string> &words) {
	// first transition by reading ip
	const int q0 = builder.newNode();
	handleIPTransition(builder, currIP, builder.node("after"), q0);

	int toTape;
	if(words[0] == "!TAPE_RAX") {
		toTape = builder.tapeIndex("rax");
	}
	else if(words[0].size() >= 10 && words[0].substr(0, 10) == "!TAPE_tape") {
		toTape = builder.tapeIndex("variables") + parseTapeNum(words[0]);
	}
	else {
		throw std::invalid_argument("Invalid line " + std::to_string(currIP));
	}

	const int q1 = builder.newNode();
	const int endNode = builder.node("before");

	builder.add1TapeTransition(q0, q1, toTape, ".", "0", 1);
	builder.add1TapeTransition(q1, endNode, toTape, ".", "_", -1);
}

/**
 * handle assignment from integer literal to tape
 */
void handleAssignmentIntegerLiteral(MultiTapeBuilder &builder, const size_t currIP, const std::vector<std::string> &words) {
	// first transition by reading ip
	const int q0 = builder.newNode();
	handleIPTransition(builder, currIP, builder.node("after"), q0);

	int toTape;
	if(words[0] == "!TAPE_RAX") {
		toTape = builder.tapeIndex("rax");
	}
	else if(words[0].size() >= 10 && words[0].substr(0, 10) == "!TAPE_tape") {
		toTape = builder.tapeIndex("variables") + parseTapeNum(words[0]);
	}
	else {
		throw std::invalid_argument("Invalid line " + std::to_string(currIP));
	}
	
	const std::string bits = convertIntegerToBits(words[1]);
	int prevState = q0;

	for(size_t i = 0; i < bits.size(); ++i) {
		const int q = builder.newNode();
		builder.add1TapeTransition(prevState, q, toTape, ".", std::to_string(bits[i]), 1);
		prevState = q;
	}

	const int q1 = builder.newNode();
	builder.add1TapeTransition(prevState, q1, toTape, ".", "_", 0);
	prevState = q1;

	// move left
	for(size_t i = 0; i < bits.size(); ++i) {
		const int q = builder.newNode();
		builder.add1TapeTransition(prevState, q, toTape, ".", ".", -1);
		prevState = q;
	}
	
	builder.add1TapeTransition(prevState, builder.node("before"), toTape, ".", ".", 0);
}

MultiTapeTuringMachine assemblyToMultiTapeTuringMachine(const std::vector<std::string> &assembly) {
	const int numVars = countTapeVariables(assembly);

	size_t ipSize = 1;
	int s = ((int) assembly.size()) - 1;	
	while(s > 1) {
		++ipSize;
		s /= 2;
	}
	if(ipSize < 2) {
		ipSize = 2;
	}

	// initialize tapes with names
	std::vector<std::pair<std::string, int> > tapeCounts;
	
	tapeCounts.emplace_back("input", 1);
	tapeCounts.emplace_back("output", 1);
	tapeCounts.emplace_back("ipStack", 1);
	tapeCounts.emplace_back("ip", 1);
	tapeCounts.emplace_back("ipSideways", ipSize);
	tapeCounts.emplace_back("paramStack", 1);
	tapeCounts.emplace_back("bitIndex", 1);
	tapeCounts.emplace_back("bits", 1);
	tapeCounts.emplace_back("variables", numVars);
	tapeCounts.emplace_back("rax", 1);

	MultiTapeBuilder builder(tapeCounts, ipSize, numVars);
	
	initialize(builder);

	for(size_t i = 0; i < assembly.size(); ++i) {
		const std::vector<std::string> words = getWords(assembly[i]);
		
		if(words[0] == "nop") {
			handleNop(builder, i);
		}
		else if(words[0] == "jmp") {
			handleJump(builder, i, words);
		}
		else if(words[0] == "pop") {
			handlePop(builder, i, words);			
		}
		else if(words[0] == "push") {
			handlePush(builder, i, words);
		}
		else if(words[0] == "call" && words[1].size() >= 10 && words[1].substr(0, 10) == "!FUNC_LIB_") {
			// first transition by reading ip
			const int q0 = builder.newNode();
			handleIPTransition(builder, i, builder.node("after"), q0);

			int prevState = q0;
			// then push new stack frames to all varTapes
			for(size_t i = 0; i < builder.numVars; ++i) {
				const int tape = builder.tapeIndex("variables") + i;
				const int q = builder.newNode();
				pushEmptyFrame(builder, tape, prevState, q);
				prevState = q;
			}

			const std::string func = words[1].substr(10, words[1].size() - 10);

			int q1 = builder.newNode();
			if(func == "isZero") {
				handleIsZero(builder, prevState, q1);
			}
			else if(func == "isPos") {
				handleIsPos(builder, prevState, q1);	
			}
			else if(func == "isNeg") {
				handleIsNeg(builder, prevState, q1);	
			}
			else if(func == "basic_add") {
				handleBasicAdd(builder, prevState, q1);
			}
			else if(func == "basic_sub") {
				handleBasicSub(builder, prevState, q1);
			}
			else if(func == "basic_xor") {
				handleBasicXor(builder, prevState, q1);
			}
			else if(func == "basic_eq") {
				handleBasicEq(builder, prevState, q1);
			}
			else if(func == "basic_lt") {
				handleBasicLt(builder, prevState, q1);
			}
			else if(func == "basic_neg") {
				handleBasicNeg(builder, prevState, q1);
			}
			else if(func == "getMemBitIndex") {
				handleGetMemBitIndex(builder, prevState, q1);
			}
			else if(func == "setMemBitBlank") {
				handleSetMemBitBlank(builder, prevState, q1);
			}
			else if(func == "moveMemHeadRight") {
				handleMoveMemHeadRight(builder, prevState, q1);
			}
			else if(func == "moveMemHeadLeft") {
				handleMoveMemHeadLeft(builder, prevState, q1);
			}
			else if(func == "setMemBitZero") {
				handleSetMemBitZero(builder, prevState, q1);
			}
			else if(func == "setMemBitOne") {
				handleSetMemBitOne(builder, prevState, q1);
			}
			else if(func == "setMemBitBlank") {
				handleSetMemBitBlank(builder, prevState, q1);
			}
			else if(func == "memBitIsZero") {
				handleMemBitIsZero(builder, prevState, q1);
			}
			else if(func == "memBitIsOne") {
				handleMemBitIsOne(builder, prevState, q1);
			}
			else if(func == "memBitIsBlank") {
				handleMemBitIsBlank(builder, prevState, q1);
			}
			else if(func == "nextInt") {
				handleNextInt(builder, prevState, q1);
			}
			else if(func == "printSpace") {
				handlePrintSpace(builder, prevState, q1);
			}
			else if(func == "printInt") {
				handlePrintInt(builder, prevState, q1);
			}
			else {
				throw std::invalid_argument("Invalid line " + std::to_string(i));
			}
			
			prevState = q1;

			// then pop off all the pushed stack frames
			for(size_t i = 0; i < builder.numVars; ++i) {
				const int tape = builder.tapeIndex("variables") + i;
				const int q = builder.newNode();
				popOffTop(builder, tape, prevState, q);
				prevState = q;
			}

			// now connect from prevState to node "before"
			builder.add1TapeTransition(prevState, builder.node("before"), builder.tapeIndex("variables"), ".", ".", 0);
		}
		else if(words[0] == "call") {
			handleCallNum(builder, i, words);	
		}
		else if(words[0] == "jf") {
			handleJf(builder, i, words);
		}
		else if(words[0] == "return") {
			handleReturn(builder, i);
		}
		else if(words[1] == "!") {
			handleNot(builder, i, words);
		}
		else if(words[2] == "&&") {
			handleAnd(builder, i, words);
		}
		else if(words[2] == "||") {
			handleOr(builder, i, words);
		}
		else if(words.size() >= 2 && words[words.size() - 2] == "=") {
			if(words[1].size() >= 5 && words[1].substr(0, 5) == "!TAPE") {
				handleAssignmentBetweenTapes(builder, i, words);
			}
			else if(words[1] == "true") {
				handleAssignmentTrue(builder, i, words);
			}
			else if(words[1] == "false") {
				handleAssignmentFalse(builder, i, words);
			}
			else {
				// tape1 9001 = ; 
				handleAssignmentIntegerLiteral(builder, i, words);
			}
		}
	}

	/*
	std::vector<Transition> transitions;
	transitions.emplace_back(1, "what", 1, "lmao", std::vector<int> {0, 0, 0});
	MultiTapeTuringMachine mttm(3, 5, 0, 1, transitions);
	return mttm;
	*/

	return builder.generateMTTM(builder.node("start"), builder.node("end"));
}

// just a placeholder
int main() {
	std::cout << "Hello, World!" << std::endl;
}

