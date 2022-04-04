#include "unit1.h"

#include <algorithm>		// std::reverse
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
int parseTapeNum(std::string &word) {
	return std::stoi(word.substr(10, word.size() - 10));
}

/**
 * Return number of distinct variables of form "!TAPE_tapeX" for some integer X
 */
int countTapeVariables(std::vector<std::string> &assembly) {
	int numVars = 0;
	for(size_t i = 0; i < assembly.size(); ++i) {
		std::vector<std::string> words = getWords(assembly[i]);
		for(size_t j = 0; j < words.size(); ++j) {
			std::string word = words[j];
			if(word.substr(0, 10) == "!TAPE_tape") {
				int tapeIndex = std::stoi(word.substr(10, word.size() - 10));
				if(tapeIndex > numVars) {
					numVars = tapeIndex;
				}
			}
		}
	}

	return numVars + 1;
}

/**
 * add nodes and transitions to increment ip
 * should be called only once to construct
 */
void addIncrementIP(MultiTapeBuilder &builder) {
	// ip is in 2's complement
	// worked out on paper
	
	int q0 = builder.newNode("before");
	int q1 = builder.newNode();
	int q2 = builder.newNode();
	int q3 = builder.node("sideways");

	int ipTapeIndex = builder.tapeIndex("ip");

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
	int sidewaysTapeIndex = builder.tapeIndex("ipSideways");
	int prevState = q3;

	for(size_t i = 0; i < builder.ipSize; ++i) {
		int toState = builder.newNode();

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
		int toState = builder.newNode();
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
	int ipStackTapeIndex = builder.tapeIndex("output");

	// for a given program, ipSize is constant
	// so add a constant number of transitions to palce in -2

	int prevState = builder.node("start");
	for(size_t i = 0; i < builder.ipSize; ++i) {
		int toState = builder.newNode();
		int bit = (i == builder.ipSize - 1) ? 0 : 1;
		builder.add1TapeTransition(prevState, toState, ipStackTapeIndex, ".", std::to_string(bit), 1);

		prevState = toState;
	}
	
	// move head back to start of -2
	for(size_t i = 0; i < builder.ipSize; ++i) {
		int toState = builder.newNode();
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
	int ipTapeIndex = builder.tapeIndex("ip");
	for(size_t i = 0; i < builder.ipSize; ++i) {
		int toState = builder.newNode();
		builder.add1TapeTransition(prevState, toState, ipTapeIndex, ".", "0", 1);
		prevState = toState;
	}

	// now move head back to beginning of IP (to the left)
	for(size_t i = 0; i < builder.ipSize; ++i) {
		int toState = builder.newNode();
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
	int q0 = builder.node("after");
	int q1 = builder.node("end");
	
	std::vector<std::pair<int, std::string> > reads;
	std::vector<std::pair<int, std::string> > writes;
	std::vector<std::pair<int, int> > shifts;
	
	int sidewaysTapeIndex = builder.tapeIndex("ipSideways");
	
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
	int prevState = addNeg2ToIpStack(builder);

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
void handleIPTransition(MultiTapeBuilder &builder, size_t currIP, int fromState, int toState) {
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
	
	int sidewaysTapeIndex = builder.tapeIndex("ipSideways");
	for(size_t i = 0; i < builder.ipSize; ++i) {
		reads.emplace_back(sidewaysTapeIndex + i, bits[i]);
	}

	// no writes, no shifts, just read the sideways tapes
	builder.addTransition(fromState, toState, reads, writes, shifts);
}

/**
 * Copy sth from fromTape toTape. Assumes there's space in toTape
 */
void copyBetweenTapes(MultiTapeBuilder &builder, int fromTape, int toTape, int startNode, int endNode) {
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
	int q2 = builder.newNode();
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

/*
// Remove one thing from fromTape, into toTape
void pop1ThingFromStack(MultiTapeBuilder &builder, int fromTape, int toTape, int startNode, int endNode) {
	std::vector<std::pair<int, std::string> > reads;
	std::vector<std::pair<int, std::string> > writes;
	std::vector<std::pair<int, int> > shifts;

	// while read 0 from fromTape, write 0 to toTape, move right, stay at q0
	reads.emplace_back(fromTape, "0");
	writes.emplace_back(toTape, "0");
	shifts.emplace_back(fromTape, 1);
	shifts.emplace_back(toTape, 1);
	
	builder.addTransition(startNode, startNode, reads, writes, shifts);
	
	reads.clear();
	writes.clear();
	shifts.clear();

	// same if read 1 from fromTape
	reads.emplace_back(fromTape, "1");
	writes.emplace_back(toTape, "1");
	shifts.emplace_back(fromTape, 1);
	shifts.emplace_back(toTape, 1);
	
	builder.addTransition(startNode, startNode, reads, writes, shifts);
	
	reads.clear();
	writes.clear();
	shifts.clear();

	// if read _ from fromTape, don't write, move left. go to new node
	int q2 = builder.newNode();
	reads.emplace_back(fromTape, "_");
	writes.emplace_back(fromTape, "_");
	shifts.emplace_back(fromTape, -1);
	shifts.emplace_back(toTape, -1);

	builder.addTransition(startNode, q2, reads, writes, shifts);

	reads.clear();
	writes.clear();
	shifts.clear();

	// erase from stack, also move toTape's copy left
	reads.emplace_back(fromTape, "[01]");
	writes.emplace_back(fromTape, "_");
	shifts.emplace_back(fromTape, -1);
	shifts.emplace_back(toTape, -1);

	builder.addTransition(q2, q2, reads, writes, shifts);

	reads.clear();
	writes.clear();
	shifts.clear();

	// both fromTape and toTape are reading a blank
	int q3 = builder.newNode();
	
	// since popping off from stack, move fromTape left 1, toTape right
	reads.emplace_back(fromTape, "_");
	shifts.emplace_back(fromTape, -1);
	shifts.emplace_back(toTape, 1);

	builder.addTransition(q2, q3, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();

	// if stack was empty, ur seeing a blank at q3. transition to endNode (but shift right 2 first)
	int q4 = builder.newNode();
	builder.add1TapeTransition(q3, q4, fromTape, "_", ".", 1);
	builder.add1TapeTransition(q4, endNode, fromTape, "_", ".", 1);

	// but if stack is not empty now, you're reading [01] at q3. Move to a new node.
	int q5 = builder.newNode();
	builder.add1TapeTransition(q3, q5, fromTape, "[01]", ".", 0);
	// now move left to beginning of the number at top of stack
	builder.add1TapeTransition(q5, q5, fromTape, "[01]", ".", -1);
	// once hit blank, move right one. end
	builder.add1TapeTransition(q5, endNode, fromTape, "_", ".", 1);
}

// Add one thing from fromTape, to the stack toTape
void push1ThingToStack(MultiTapeBuilder &builder, int fromTape, int toTape, int startNode, int endNode) {
	// first, move head of toTape to a blank space to put an integer. be careful if it's already empty!
	
	// if stack already at a blank, don't do anything, just go to qPut
	int qPut = builder.newNode();
	builder.add1TapeTransition(startNode, qPut, toTape, "_", ".", 0);
	// but if stack wasn't empty, first transition to a state where u keep going right
	int moveRight = builder.newNode();
	builder.add1TapeTransition(startNode, moveRight, toTape, "[01]", ".", 0);
	builder.add1TapeTransition(moveRight, moveRight, toTape, "[01]", ".", 1);
	builder.add1TapeTransition(moveRight, qPut, toTape, "_", ".", 1);
	
	// ok now at state qPut. stack toTape on a blank part, ready to put an integer. Copy!
	copyBetweenTapes(builder, fromTape, toTape, qPut, endNode);
}
*/

/**
 * Goes to end of current num, and goes right 2 (so 1 blank in between)
 * Assume tape is already not empty
 */
void pushEmptyFrame(MultiTapeBuilder &builder, int tape, int startNode, int endNode) {
	//builder.add1TapeTransition(startNode, startNode, tape, "[01]", ".", 1);
	//builder.add1TapeTransition(startNode, endNode, tape, "_", ".", 1);
	
	int q1 = builder.newNode();
	int q2 = builder.newNode();
	
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
void popOffTop(MultiTapeBuilder &builder, int tape, int startNode, int endNode) {
	// startNode: current tape could be blank! so just move back two
	int q1 = builder.newNode();
	int mid = builder.newNode();
	
	builder.add1TapeTransition(startNode, q1, tape, "_", ".", -1);
	builder.add1TapeTransition(q1, mid, tape, ".", ".", -1);
	
	// startNode: otherwise, erase currentNum
	int q3 = builder.newNode();
	builder.add1TapeTransition(startNode, q3, tape, "[01]", ".", 0);
	// q3 is where we go to end of num
	int q4 = builder.newNode();
	builder.add1TapeTransition(q3, q3, tape, "[01]", ".", 1);
	builder.add1TapeTransition(q3, q4, tape, "_", ".", -1);
	// q4 is where we erase the num, and move left
	builder.add1TapeTransition(q4, q4, tape, "[01]", "_", -1);
	builder.add1TapeTransition(q4, mid, tape, "_", ".", -1);
	
	// ok now we are two left from where we first started on the tape
	// mid: current cell could be blank! if so, just move to endNode
	builder.add1TapeTransition(mid, endNode, tape, "_", ".", 0);
	
	// mid: else read a [01]. gotta go to beginning of num
	int q5 = builder.newNode();
	builder.add1TapeTransition(mid, q5, tape, "[01]", ".", 0);
	builder.add1TapeTransition(q5, q5, tape, "[01]", ".", -1);
	builder.add1TapeTransition(q5, endNode, tape, "_", ".", 1);
}

/**
 * If assembly line is nop, and reach ip == currIP,
 * then connect node "after" to "before"
 */
void handleNop(MultiTapeBuilder &builder, size_t currIP) {
	int q0 = builder.node("before");
	int q1 = builder.node("after");
	handleIPTransition(builder, currIP, q1, q0);
}

/**
 * Jmp statement.
 */
void handleJump(MultiTapeBuilder &builder, size_t currIP, std::vector<std::string> &words) {
	int q0 = builder.newNode();
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
	int ipTapeIndex = builder.tapeIndex("ip");
	for(size_t i = 0; i < builder.ipSize; ++i) {
		int q = builder.newNode();
		builder.add1TapeTransition(prevState, q, ipTapeIndex, ".", std::to_string(bits[i]), 1);
		prevState = q0;
	}
	
	// move head of ipTapeIndex back to start
	for(size_t i = 0; i < builder.ipSize; ++i) {
		int q = (i + 1 == builder.ipSize) ? builder.node("sideways") : builder.newNode();
		int shift = (i + 1 == builder.ipSize) ? 0 : -1;
		builder.add1TapeTransition(prevState, q, ipTapeIndex, ".", ".", shift);
		prevState = q;
	}
	
	// now connected to sideways too!
}

/**
 * Pop either from PARAMS or RAX; moved to a varTape
 */
void handlePop(MultiTapeBuilder &builder, size_t currIP, std::vector<std::string> &words) {
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
		std::cout << "Unhandled request: ";
		for(size_t i = 0; i < words.size(); ++i) {
			std::cout << words[i];
			if(i + 1 < words.size()) {
				std::cout << " ";
			}
		}
		std::cout << std::endl;

		return;
	}
	
	int toTape = builder.tapeIndex("variables") + parseTapeNum(words[2]);
	
	// transition from node "after" to a new node, by reading currIP
	int q0 = builder.node("after");
	int q1 = builder.newNode();
	int q2 = builder.newNode();
	int endNode = builder.node("before");

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
void handlePush(MultiTapeBuilder &builder, size_t currIP, std::vector<std::string> &words) {
	int q0 = builder.newNode();
	int q1 = builder.newNode();
	int endNode = builder.node("before");
	handleIPTransition(builder, currIP, builder.node("after"), q0);

	int fromTape;
	/*
	if(words[1] == "TAPE_RAX") {
		fromTape = builder.tapeIndex("rax");
	}
	*/
	if(words[1].size() >= 10 && words[1].substr(0, 10) == "!TAPE_tape") {
		fromTape = builder.tapeIndex("variables") + parseTapeNum(words[1]);
	}
	else {
		std::cout << "Push statement not recognized: ";
		for(size_t i = 0; i < words.size(); ++i) {
			std::cout << words[i];
			if(i + 1 < words.size()) {
				std::cout << " ";
			}
		}
		std::cout << std::endl;
		return;
	}
	
	int toTape;
	if(words[2] == "!TAPE_PARAMS") {
		toTape = builder.tapeIndex("paramStack");
	}
	else {
		std::cout << "Push statement not recognized: ";
		for(size_t i = 0; i < words.size(); ++i) {
			std::cout << words[i];
			if(i + 1 < words.size()) {
				std::cout << " ";
			}
		}
		std::cout << std::endl;
		return;
	}

	//push1ThingToStack(builder, fromTape, toTape, q0, builder.node("before"));
	pushEmptyFrame(builder, toTape, q0, q1);
	copyBetweenTapes(builder, fromTape, toTape, q1, endNode);
}

/**
 * handle assembly code of checking whether value at tape is zero or not.
 * write 0 into rax if not zero; write a 1 into rax if it zero
 */
void handleIsZero(MultiTapeBuilder &builder, int startNode, int endNode) {
	// must pop from paramStack
	int q0 = builder.newNode();
	int q1 = builder.newNode();
	int penultimateNode = builder.newNode();

	copyBetweenTapes(builder, builder.tapeIndex("paramStack"), builder.tapeIndex("variables"), startNode, q0);
	popOffTop(builder, builder.tapeIndex("paramStack"), q0, q1);
	
	// zero is just 0 followed by a blank
	int tape0 = builder.tapeIndex("variables");
	int tapeRax = builder.tapeIndex("rax");
	
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
	int q2 = builder.newNode();
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
void handleIsPos(MultiTapeBuilder &builder, int startNode, int endNode) {
	// must pop from paramStack
	int q0 = builder.newNode();
	int q1 = builder.newNode();
	int penultimateNode = builder.newNode();

	copyBetweenTapes(builder, builder.tapeIndex("paramStack"), builder.tapeIndex("variables"), startNode, q0);
	popOffTop(builder, builder.tapeIndex("paramStack"), q0, q1);

	// positive is when bits start with 0 then a [01]
	int tape0 = builder.tapeIndex("variables");
	int tapeRax = builder.tapeIndex("rax");

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
	int q2 = builder.newNode();
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
void handleIsNeg(MultiTapeBuilder &builder, int startNode, int endNode) {
	// must pop from paramStack
	int q0 = builder.newNode();
	int q1 = builder.newNode();
	int penultimateNode = builder.newNode();

	copyBetweenTapes(builder, builder.tapeIndex("paramStack"), builder.tapeIndex("variables"), startNode, q0);
	popOffTop(builder, builder.tapeIndex("paramStack"), q0, q1);

	// positive is when bits start with 0 then a [01]
	int tape0 = builder.tapeIndex("variables");
	int tapeRax = builder.tapeIndex("rax");

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
void handleBasicAdd(MultiTapeBuilder &builder, int startNode, int endNode) {
	int q0 = builder.newNode();
	int q1 = builder.newNode();
	int q2 = builder.newNode();
	int q3 = builder.newNode();

	int tapeStack = builder.tapeIndex("paramStack");
	int tape0 = builder.tapeIndex("variables");
	int tape1 = tape0 + 1;
	int tapeRax = builder.tapeIndex("rax");

	copyBetweenTapes(builder, tapeStack, tape0, startNode, q0);
	popOffTop(builder, tapeStack, q0, q1);
	copyBetweenTapes(builder, tapeStack, tape0 + 1, q1, q2);
	popOffTop(builder, tapeStack, q2, q3);

	// add the values in the two tapes. both are positive.
	int carryOff = q3;
	int carryOn = builder.newNode();

	std::vector<std::pair<int, std::string> > reads;
	std::vector<std::pair<int, std::string> > writes;
	std::vector<std::pair<int, int> > shifts;
	
	shifts.emplace_back(tape0, 1);
	shifts.emplace_back(tape1, 1);
	shifts.emplace_back(tapeRax, 1);

	std::vector<std::string> symbols {"0", "1", "_"};
	std::unordered_map<std::string, int> inherentValue {{"0", 0}, {"1", 1}, {"_", 0}};
	std::vector<int> nodes {carryOff, carryOn};

	for(std::string s1 : symbols) {
		for(std::string s2: symbols) {
			if(s1 == "_" && s2 == "_") {
				continue;
			}

			for(size_t i = 0; i < nodes.size(); ++i) {
				int sum = ((int) i) + inherentValue[s1] + inherentValue[s2];	

				reads.emplace_back(tape0, s1);
				reads.emplace_back(tape1, s2);
				writes.emplace_back(tapeRax, std::to_string(sum % 2));

				int toNode = (sum >= 2) ? carryOn : carryOff;
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
		
	int q4 = builder.newNode();
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
void handleBasicSub(MultiTapeBuilder &builder, int startNode, int endNode) {
	int q0 = builder.newNode();
	int q1 = builder.newNode();
	int q2 = builder.newNode();
	int q3 = builder.newNode();

	int tapeStack = builder.tapeIndex("paramStack");
	int tape0 = builder.tapeIndex("variables");
	int tape1 = tape0 + 1;
	int tapeRax = builder.tapeIndex("rax");

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
	int q4 = builder.newNode();

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

	int q5 = builder.newNode();
	reads.emplace_back(tape0, "_");
	reads.emplace_back(tape1, "_");
	shifts.emplace_back(tape0, 1);
	shifts.emplace_back(tape1, 1);

	builder.addTransition(q4, q5, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();
	
	// ok now back where we started from. now subtract!
	int borrowOff = q5;
	int borrowOn = builder.newNode();
	
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
	int q6 = builder.newNode();
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
	int encountered1 = builder.newNode();
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

	int q7 = endNode;
	builder.addTransition(encountered1, q7, reads, writes, shifts);
}

/**
 * handle assembly code of doing A xor B
 * where A is first value popped, B is second value popped
 */
void handleBasicXor(MultiTapeBuilder &builder, int startNode, int endNode) {
	int q0 = builder.newNode();
	int q1 = builder.newNode();
	int q2 = builder.newNode();
	int q3 = builder.newNode();

	int tapeStack = builder.tapeIndex("paramStack");
	int tape0 = builder.tapeIndex("variables");
	int tape1 = tape0 + 1;
	int tapeRax = builder.tapeIndex("rax");

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
	int q4 = builder.newNode();

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

	int q5 = builder.newNode();
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
	int q6 = builder.newNode();

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
	int encountered1 = builder.newNode();
	int penultimateNode = builder.newNode();

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
void handleBasicEq(MultiTapeBuilder &builder, int startNode, int endNode) {
	int q0 = builder.newNode();
	int q1 = builder.newNode();
	int q2 = builder.newNode();
	int q3 = builder.newNode();

	int tapeStack = builder.tapeIndex("paramStack");
	int tape0 = builder.tapeIndex("variables");
	int tape1 = tape0 + 1;
	int tapeRax = builder.tapeIndex("rax");

	copyBetweenTapes(builder, tapeStack, tape0, startNode, q0);
	popOffTop(builder, tapeStack, q0, q1);
	copyBetweenTapes(builder, tapeStack, tape0 + 1, q1, q2);
	popOffTop(builder, tapeStack, q2, q3);

	std::vector<std::pair<int, std::string> > reads;
	std::vector<std::pair<int, std::string> > writes;
	std::vector<std::pair<int, int> > shifts;
	
	// ok now back where we started from. now check for equality!

	// when have written ans, but tape0 and tape1 heads need to go back left
	int moveBackLeft = builder.newNode();

	// have to do all 9 cases for reading "0","1",or "_" on both tapes
	// if current bits same, move on. otherwise, right a 0 in rax,
	// and go to moveBackLeft
	
	std::vector<std::string> symbols {"0", "1", "_"};
	
	for(size_t i = 0; i < symbols.size(); ++i) {
		for(size_t j = 0; j < symbols.size(); ++j) {
			std::string s1 = symbols[i];
			std::string s2 = symbols[j];

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
void handleBasicLt(MultiTapeBuilder &builder, int startNode, int endNode) {
	int q0 = builder.newNode();
	int q1 = builder.newNode();
	int q2 = builder.newNode();
	int q3 = builder.newNode();

	int tapeStack = builder.tapeIndex("paramStack");
	int tape0 = builder.tapeIndex("variables");
	int tape1 = tape0 + 1;
	int tapeRax = builder.tapeIndex("rax");

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
	int q4 = builder.newNode();

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
	int moveBackLeft = builder.newNode();

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
void handleBasicNeg(MultiTapeBuilder &builder, int startNode, int endNode) {
	int q0 = builder.newNode();
	int q1 = builder.newNode();

	int tapeStack = builder.tapeIndex("paramStack");
	int tape0 = builder.tapeIndex("variables");
	int tapeRax = builder.tapeIndex("rax");

	copyBetweenTapes(builder, tapeStack, tape0, startNode, q0);
	popOffTop(builder, tapeStack, q0, q1);

	std::vector<std::pair<int, std::string> > reads;
	std::vector<std::pair<int, std::string> > writes;
	std::vector<std::pair<int, int> > shifts;
	
	// ok now back where we started from. now flip sign bit
	// sign bit is the cell the head is currently on!
	int q2 = builder.newNode();
	builder.add1TapeTransition(q1, q2, tape0, "0", "1", 0);
	builder.add1TapeTransition(q1, q2, tape0, "1", "0", 0);

	copyBetweenTapes(builder, tape0, tapeRax, q2, endNode);
}

MultiTapeTuringMachine assemblyToMultiTapeTuringMachine(std::vector<std::string> &assembly) {
	int numVars = countTapeVariables(assembly);

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
		std::vector<std::string> words = getWords(assembly[i]);
		
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
			int q0 = builder.newNode();
			handleIPTransition(builder, i, builder.node("after"), q0);

			int prevState = q0;
			// then push new stack frames to all varTapes
			for(size_t i = 0; i < builder.numVars; ++i) {
				int tape = builder.tapeIndex("variables") + i;
				int q = builder.newNode();
				pushEmptyFrame(builder, tape, prevState, q);
				prevState = q;
			}

			std::string func = words[1].substr(10, words[1].size() - 10);

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

			}

			prevState = q1;


			// then pop off all the pushed stack frames
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

