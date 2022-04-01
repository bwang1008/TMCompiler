#include "unit1.h"
#include "multi_tape_builder.h"
#include "utils.h"
#include "../tm_definition/transition.h"
#include "../tm_definition/multi_tape_turing_machine.h"

#include <algorithm>	// std::reverse
#include <pair>			// std::pair, std::make_pair
#include <string>		// std::string, std::stoi
#include <vector>		// std::vector

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
	for(size_t i = 0; i < assembly.size(); ++i+) {
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
void addIncrementIP(MultiTapeBuilder &builder, int prevState) {
	// ip is in 2's complement
	// worked out on paper
	
	int q0 = prevState;
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
	builder.add1TapeTransition(prevState, builder.node("after"), ".", ".", 0);
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

	// connect o node "sideways" to write 00000 in sideways
	builder.add1TapeTransition(prevState, builder.node("sideways"), ".", ".", 0);
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
	int prevState = builder.node("before");
	
	// add nodes for incrementing IP between before and after
	addIncrementIP(builder, prevState);

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

	// once see a blank, don't write, move back left
	int q2 = builder.newNode();
	reads.emplace_back(fromTape, "_");
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
void handleJump(MultiTapeBuilder &builder, size_t currIP, std::string &line) {
	int q0 = builder.newNode();
	handleIPTransition(builder, currIP, builder.node("after"), q0);
	
	std::vector<std::string> words = getWords(line);
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
void handlePop(MultiTapeBuilder &builder, size_t currIP, std::string &line) {
	std::vector<std::string> words = getWords(line);

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
		std::cout << "Unhandled request: " << line << std::endl;
		return;
	}
	
	int toTape = builder.tapeIndex("variables") + parseTapeNum(words[2]);
	
	// transition from node "after" to a new node, by reading currIP
	int q0 = builder.node("after");
	int q1 = builder.newNode();
	int q2 = builder.newNode();
	int endNode = builder.node("before");

	handleIPTransition(builder, currIP, q0, q1);
	
	// copy from fromTape to toTape
	std::vector<std::pair<int, std::string> > reads;
	std::vector<std::pair<int, std::string> > writes;
	std::vector<std::pair<int, int> > shifts;

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
 * Code for handling assembly code of pushing variable onto a stack
 */
void handlePush(MultiTapeBuilder &builder, size_t currIP, std::string &line) {
	std::vector<std::string> words = getWords(line);
	int q0 = builder.newNode();
	handleIPTransition(builder, currIP, builder.node("after"), q0);

	int fromTape;
	if(words[1] == "TAPE_RAX") {
		fromTape = builder.tapeIndex("rax");
	}
	else if(words[1].size() >= 10 && words[1].substr(0, 10) == "!TAPE_tape") {
		fromTape = builder.tapeIndex("variables") + parseTapeNum(words[1]);
	}
	else {
		std::cout << "Push statement not recognized: " << line << std::endl;
		return;
	}
	
	int toTape;
	if(words[2] == "!TAPE_PARAMS") {
		toTape = builder.tapeIndex("paramStack");
	}
	else {
		std::cout << "Push statement not recognized: " << line << std::endl;
		return;
	}

	push1ThingToStack(builder, fromTape, toTape, q0, builder.node("before"));
}

MultiTapeTuringMachine assemblyToMultiTapeTuringMachine(std::vector<std::string> &assembly) {
	int numVars = countTapeVariables(assembly);

	size_t ipSize = 1;
	int s = ((int) programSize) - 1;	
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
			handleJump(builder, i, assembly[i]);
		}
		else if(words[0] == "pop") {
			handlePop(builder, i, assembly[i]);			
		}
		else if(words[0] == "push") {

		}
	}

	/*
	std::vector<Transition> transitions;
	transitions.emplace_back(1, "what", 1, "lmao", std::vector<int> {0, 0, 0});
	MultiTapeTuringMachine mttm(3, 5, 0, 1, transitions);
	return mttm;
	*/
}
