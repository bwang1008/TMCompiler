#include "unit1.hpp"

#include <iostream>			// std::cout, std::endl
#include <unordered_set>	// std::unordered_set
#include <stdexcept>		// std::invalid_argument
#include <string>			// std::string, std::stoi
#include <utility>			// std::pair, std::make_pair
#include <vector>			// std::vector

#include "TMCompiler/compilation/multi_tape_builder.hpp"
#include "TMCompiler/tm_definition/multi_tape_turing_machine.hpp"
#include "TMCompiler/tm_definition/transition.hpp"
#include "TMCompiler/utils/helper.hpp"  // lineNumToBits, convertIntegerToBits

/**
 * add nodes and transitions to increment ip
 * should be called only once to construct
 */
void addIncrementIP(MultiTapeBuilder &builder) {
	// ip is in 2's complement
	// worked out on paper
	
	const size_t q0 = builder.node("before");
	const size_t q1 = builder.newNode();
	const size_t q2 = builder.newNode();
	const size_t q3 = builder.node("sideways");

	const size_t ipTapeIndex = builder.tapeIndex("ip");

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
	const size_t sidewaysTapeIndex = builder.tapeIndex("ipSideways");
	size_t prevNode = q3;

	for(size_t i = 0; i < builder.ipSize; ++i) {
		const size_t toNode = builder.newNode();

		// read a 0? write a 0. read 1 ? write a 1. Can't use . because only for same tapes
		for(size_t j = 0; j < 2; ++j) {
			std::vector<std::pair<size_t, std::string> > reads;
			std::vector<std::pair<size_t, std::string> > writes;
			std::vector<std::pair<size_t, int> > shifts;

			reads.emplace_back(ipTapeIndex, std::to_string(j));
			writes.emplace_back(sidewaysTapeIndex + i, std::to_string(j));
			shifts.emplace_back(ipTapeIndex, 1);

			builder.addTransition(prevNode, toNode, reads, writes, shifts);
		}

		prevNode = toNode;
	}

	// but now head of ip tape at the end. move back left to beginning of ip
	for(size_t i = 0; i < builder.ipSize; ++i) {
		const size_t toNode = builder.newNode();
		// whatever you read, don't change it, but move left
		builder.add1TapeTransition(prevNode, toNode, ipTapeIndex, ".", ".", -1);
		prevNode = toNode;
	}

	// go from this last state, into the "after" state: since now 
	// we are after incrementing IP
	builder.add1TapeTransition(prevNode, builder.node("after"), ipTapeIndex, ".", ".", 0);
}

/**
 * put 0 in tape bitIndex
 */
void add0IntoBitIndex(MultiTapeBuilder &builder, const size_t startNode, const size_t endNode) {
	const size_t tapeBitIndex = builder.tapeIndex("bitIndex");

	const size_t q0 = builder.newNode();
	builder.add1TapeTransition(startNode, q0, tapeBitIndex, ".", "0", 1);
	builder.add1TapeTransition(q0, endNode, tapeBitIndex, ".", "_", -1);
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
void addNeg2ToIpStack(MultiTapeBuilder &builder, const size_t startNode, const size_t endNode) {
	const size_t ipStackTapeIndex = builder.tapeIndex("ipStack");

	// for a given program, ipSize is constant
	// so add a constant number of transitions to palce in -2

	size_t prevNode = startNode;
	for(size_t i = 0; i < builder.ipSize; ++i) {
		const size_t toNode = builder.newNode();
		const int bit = (i == builder.ipSize - 1) ? 0 : 1;
		builder.add1TapeTransition(prevNode, toNode, ipStackTapeIndex, ".", std::to_string(bit), 1);

		prevNode = toNode;
	}
	
	// move head back to start of -2
	for(size_t i = 0; i < builder.ipSize; ++i) {
		const size_t toNode = builder.newNode();
		builder.add1TapeTransition(prevNode, toNode, ipStackTapeIndex, ".", ".", -1);
		prevNode = toNode;
	}

	builder.add1TapeTransition(prevNode, endNode, ipStackTapeIndex, ".", ".", 0);
}

/**
 * Set all (ipSize) bits in IP to 0
 * Connect to node "sideways"
 */
void setInitialIP(MultiTapeBuilder &builder, const size_t startNode, const size_t endNode) {
	size_t prevNode = startNode;
	const size_t ipTapeIndex = builder.tapeIndex("ip");
	for(size_t i = 0; i < builder.ipSize; ++i) {
		const size_t toNode = builder.newNode();
		builder.add1TapeTransition(prevNode, toNode, ipTapeIndex, ".", "0", 1);
		prevNode = toNode;
	}

	// now move head back to beginning of IP (to the left)
	for(size_t i = 0; i < builder.ipSize; ++i) {
		const size_t toNode = builder.newNode();
		builder.add1TapeTransition(prevNode, toNode, ipTapeIndex, ".", ".", -1);
		prevNode = toNode;
	}

	// connect node "sideways" to write 00000 in sideways
	builder.add1TapeTransition(prevNode, endNode, ipTapeIndex, ".", ".", 0);
}

/**
 * When IP == -1, (aka IP in bits == 11111), 
 * transition from node "after" to node "end"
 */
void addExitClause(MultiTapeBuilder &builder) {	
	const size_t q0 = builder.node("after");
	const size_t q1 = builder.node("end");
	
	std::vector<std::pair<size_t, std::string> > reads;
	std::vector<std::pair<size_t, std::string> > writes;
	std::vector<std::pair<size_t, int> > shifts;
	
	const size_t sidewaysTapeIndex = builder.tapeIndex("ipSideways");
	
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
	int prevNode = builder.node("start");
	for(size_t i = 0; i < builder.numVars; ++i) {
		int tape = builder.tapeIndex("variables") + i;
		int q = builder.newNode();
		builder.add1TapeTransition(prevNode, q, tape, ".", "0", 0);
		prevNode = q;
	}
	*/
	
	// add nodes for incrementing IP between before and after
	addIncrementIP(builder);

	const size_t q0 = builder.newNode();
	const size_t q1 = builder.newNode();

	add0IntoBitIndex(builder, builder.node("start"), q0);

	// put -2 into ip stack: so when main returns,
	// ip incremented from -2 to -1, which then we exit
	addNeg2ToIpStack(builder, q0, q1);

	// push 0 into IP, using p bits. Just 00000.
	// push 0 into ipSideways: just all 0 in each one.
	setInitialIP(builder, q1, builder.node("sideways"));	
	
	// exit condition, aka when the TM halts, is when IP == -1:
	// in bits, IP is all 1: IP == 11111 (cuz 2's complement)
	addExitClause(builder);
}

/**
 * Add a transition from fromNode to toNode if IP == currIP
 */
void handleIPTransition(MultiTapeBuilder &builder, const size_t currIP, const size_t fromNode, const size_t toNode) {
	std::vector<std::pair<size_t, std::string> > reads;
	std::vector<std::pair<size_t, std::string> > writes;
	std::vector<std::pair<size_t, int> > shifts;

	std::vector<std::string> bits = lineNumToBits(currIP, builder.ipSize);
	
	const size_t sidewaysTapeIndex = builder.tapeIndex("ipSideways");
	for(size_t i = 0; i < builder.ipSize; ++i) {
		reads.emplace_back(sidewaysTapeIndex + i, bits[i]);
	}

	// no writes, no shifts, just read the sideways tapes
	builder.addTransition(fromNode, toNode, reads, writes, shifts);
}

/**
 * Copy sth from fromTape toTape. Assumes there's space in toTape
 */
void copyBetweenTapes(MultiTapeBuilder &builder, const size_t fromTape, const size_t toTape, const size_t startNode, const size_t endNode) {
	std::vector<std::pair<size_t, std::string> > reads;
	std::vector<std::pair<size_t, std::string> > writes;
	std::vector<std::pair<size_t, int> > shifts;

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
	const size_t q2 = builder.newNode();
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
void pushEmptyFrame(MultiTapeBuilder &builder, const size_t tape, const size_t startNode, const size_t endNode) {
	const size_t q1 = builder.newNode();
	const size_t q2 = builder.newNode();
	
	// if see [01], go to q1. else see blank: go to q2, and move right
	builder.add1TapeTransition(startNode, q1, tape, "[01]", ".", 0);
	builder.add1TapeTransition(startNode, q2, tape, "_", ".", 1);
	
	// q1: while see [01], move right. when see blank, move right, 
	// write a blank, then stay: cuz what if pop (and no erase),
	// then push empty (no copy), then push again? it would mistakenly
	// think there was a value there (the old one) rather than an empty push
	const size_t q3 = builder.newNode();
	builder.add1TapeTransition(q1, q1, tape, "[01]", ".", 1);
	builder.add1TapeTransition(q1, q3, tape, "_", ".", 1);
	builder.add1TapeTransition(q3, endNode, tape, ".", "_", 0);
	
	// q2 just moves another right
	// EDIT: it doesn't necessarily read another blank. could be residual 
	// old memory. write a new blank
	builder.add1TapeTransition(q2, endNode, tape, ".", "_", 1);
}

/**
 * Pop off num from stack
 */
void popOffTop(MultiTapeBuilder &builder, const size_t tape, const size_t startNode, const size_t endNode) {
	// startNode: current tape could be blank! so just move back two
	const size_t q1 = builder.newNode();
	const size_t mid = builder.newNode();
	
	builder.add1TapeTransition(startNode, q1, tape, ".", "_", -1);
	builder.add1TapeTransition(q1, mid, tape, ".", "_", -1);
	
	// ok now we are two left from where we first started on the tape
	// mid: current cell could be blank! if so, just move to endNode
	builder.add1TapeTransition(mid, endNode, tape, "_", ".", 0);
	
	// mid: else read a [01]. gotta go to beginning of num
	const size_t q5 = builder.newNode();
	builder.add1TapeTransition(mid, q5, tape, "[01]", ".", 0);
	builder.add1TapeTransition(q5, q5, tape, "[01]", ".", -1);
	builder.add1TapeTransition(q5, endNode, tape, "_", ".", 1);
}

/**
 * If assembly line is nop, and reach ip == currIP,
 * then connect node "after" to "before"
 */
void handleNop(MultiTapeBuilder &builder, const size_t currIP) {
	const size_t q0 = builder.node("before");
	const size_t q1 = builder.node("after");

	handleIPTransition(builder, currIP, q1, q0);
}

/**
 * Jmp statement.
 */
void handleJump(MultiTapeBuilder &builder, const size_t currIP, const std::vector<std::string> &words) {
	const size_t q0 = builder.newNode();
	handleIPTransition(builder, currIP, builder.node("after"), q0);
	
	const size_t jumpLine = (size_t) std::stoi(words[1]);
	
	std::vector<std::string> bits = lineNumToBits(jumpLine, builder.ipSize);

	size_t prevNode = q0;
	const size_t ipTapeIndex = builder.tapeIndex("ip");
	for(size_t i = 0; i < builder.ipSize; ++i) {
		const size_t q = builder.newNode();
		builder.add1TapeTransition(prevNode, q, ipTapeIndex, ".", bits[i], 1);
		prevNode = q;
	}
	
	// move head of ipTapeIndex back to start
	for(size_t i = 0; i < builder.ipSize; ++i) {
		const size_t q = builder.newNode();
		const int shift = -1;
		builder.add1TapeTransition(prevNode, q, ipTapeIndex, ".", ".", shift);
		prevNode = q;
	}
	
	// now connect to sideways
	builder.add1TapeTransition(prevNode, builder.node("sideways"), ipTapeIndex, ".", ".", 0);
}

/**
 * Pop either from PARAMS or RAX; moved to a varTape
 */
void handlePop(MultiTapeBuilder &builder, const size_t currIP, const std::vector<std::string> &words) {
	size_t fromTape;
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
	
	const size_t toTape = builder.tapeIndex("variables") + parseTapeNum(words[2]);
	
	// transition from node "after" to a new node, by reading currIP
	const size_t q0 = builder.node("after");
	const size_t q1 = builder.newNode();
	const size_t q2 = builder.newNode();
	const size_t endNode = builder.node("before");

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
	const size_t q0 = builder.newNode();
	const size_t q1 = builder.newNode();
	const size_t endNode = builder.node("before");

	handleIPTransition(builder, currIP, builder.node("after"), q0);

	size_t fromTape;
	if(words[1].size() >= 10 && words[1].substr(0, 10) == "!TAPE_tape") {
		fromTape = builder.tapeIndex("variables") + parseTapeNum(words[1]);
	}
	else {
		throw std::invalid_argument("Invalid push at line " + std::to_string(currIP));
	}
	
	size_t toTape;
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
 * add padding of BLANKS to shorter of two tapes until they are of the same length
 * Blanks because adding 0's will change the parameters...for inlined functions, 
 * parameters should basically be const and not changed
 * Be careful of residual memory! guarantee one space at end of each word tho
 * (which is why we're padding it with blanks to erase residual memory)
 */
void handlePadding(MultiTapeBuilder &builder, const size_t tape0, const size_t tape1, const size_t startNode, const size_t endNode, const bool moveLeft = true) {
	std::vector<std::pair<size_t, std::string> > reads;
	std::vector<std::pair<size_t, std::string> > writes;
	std::vector<std::pair<size_t, int> > shifts;

	// pad end of shorter number with 0's
	// while both are not blank, move right
	reads.emplace_back(tape0, "[01]");
	reads.emplace_back(tape1, "[01]");
	shifts.emplace_back(tape0, 1);
	shifts.emplace_back(tape1, 1);

	builder.addTransition(startNode, startNode, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();

	const size_t qBlank0 = builder.newNode();
	const size_t qBlank1 = builder.newNode();
	const size_t qMoveLeft = (moveLeft) ? builder.newNode() : endNode;

	// startNode: if tape0 sees blank, move to new node
	reads.emplace_back(tape0, "_");
	reads.emplace_back(tape1, "[01]");

	builder.addTransition(startNode, qBlank0, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();

	// startNode: if tape1 sees blank, move to new node
	reads.emplace_back(tape0, "[01]");
	reads.emplace_back(tape1, "_");

	builder.addTransition(startNode, qBlank1, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();

	// startNode:  but if both tapes see _, go to node to move left
	reads.emplace_back(tape0, "_");
	reads.emplace_back(tape1, "_");
	shifts.emplace_back(tape0, -1);
	shifts.emplace_back(tape1, -1);

	builder.addTransition(startNode, qMoveLeft, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();

	// qBlank0: while tape1 isn't blank, write _ to tape0, move tapes right
	reads.emplace_back(tape1, "[01]");
	writes.emplace_back(tape0, "_");
	shifts.emplace_back(tape0, 1);
	shifts.emplace_back(tape1, 1);

	builder.addTransition(qBlank0, qBlank0, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();

	// qBlank0: but when tape1 is blank, go to qMoveLeft
	reads.emplace_back(tape1, "_");
	writes.emplace_back(tape0, "_");
	shifts.emplace_back(tape0, -1);
	shifts.emplace_back(tape1, -1);

	builder.addTransition(qBlank0, qMoveLeft, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();

	// qBlank1: while tape0 isn't blank, write _ to tape1, move tapes right
	reads.emplace_back(tape0, "[01]");
	writes.emplace_back(tape1, "_");
	shifts.emplace_back(tape0, 1);
	shifts.emplace_back(tape1, 1);

	builder.addTransition(qBlank1, qBlank1, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();

	// qBlank1: but when tape0 is blank, go to qMoveLeft
	reads.emplace_back(tape0, "_");
	writes.emplace_back(tape1, "_");
	shifts.emplace_back(tape0, -1);
	shifts.emplace_back(tape1, -1);

	builder.addTransition(qBlank1, qMoveLeft, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();

	if(!moveLeft) {
		return;
	}

	// ok so everyone at qMoveLeft. Let's move left

	// qMoveLeft: while both not blank, move back left
	reads.emplace_back(tape0, "[01_]");
	reads.emplace_back(tape1, "[01]");
	shifts.emplace_back(tape0, -1);
	shifts.emplace_back(tape1, -1);

	builder.addTransition(qMoveLeft, qMoveLeft, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();

	// again, but if other tape has blank: qMoveLeft: while both not blank, move back left
	reads.emplace_back(tape0, "[01]");
	reads.emplace_back(tape1, "[01_]");
	shifts.emplace_back(tape0, -1);
	shifts.emplace_back(tape1, -1);

	builder.addTransition(qMoveLeft, qMoveLeft, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();

	// qMoveLeft: but when encounter both blanks on the left, move 1 right, 
	// and transition to endNode
	reads.emplace_back(tape0, "_");
	reads.emplace_back(tape1, "_");
	shifts.emplace_back(tape0, 1);
	shifts.emplace_back(tape1, 1);

	builder.addTransition(qMoveLeft, endNode, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();
}

/**
 * handle assembly code of checking whether value at tape is zero or not.
 * write 0 into rax if not zero; write a 1 into rax if it zero
 * Function is inlined: no popping off from paramStack. Which tape argument
 * is on is given instead
 */
void handleIsZero(MultiTapeBuilder &builder, const size_t paramTape, const size_t startNode, const size_t endNode) {
	// zero is just 0 followed by a blank
	
	const size_t tape0 = paramTape;
	const size_t tapeRax = builder.tapeIndex("rax");
	const size_t q1 = startNode;
	const size_t penultimateNode = builder.newNode();
	
	// q1: if see 1 or _, not a zero
	std::vector<std::pair<size_t, std::string> > reads;
	std::vector<std::pair<size_t, std::string> > writes;
	std::vector<std::pair<size_t, int> > shifts;
	
	reads.emplace_back(tape0, "[1_]");
	writes.emplace_back(tapeRax, "0");
	shifts.emplace_back(tapeRax, 1);
		
	builder.addTransition(q1, penultimateNode, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();
	
	// q1: otherwise saw a 0...move right to check if it blank
	const size_t q2 = builder.newNode();
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
 * handle assembly code of checking whether value at paramTape is positive or not
 * write 0 into rax if not positive; write a 1 into rax if it is positive
 * Function is inlined: don't need to pop parameters from stack. 
 * Which tape parameter is on is given instead
 */
void handleIsPos(MultiTapeBuilder &builder, const size_t paramTape, const size_t startNode, const size_t endNode) {
	// positive is when bits start with 0 then a [01]
	const size_t tape0 = paramTape;
	const size_t tapeRax = builder.tapeIndex("rax");

	const size_t q1 = startNode;
	const size_t penultimateNode = builder.newNode();

	// q1: if see 1 or _, not positive
	std::vector<std::pair<size_t, std::string> > reads;
	std::vector<std::pair<size_t, std::string> > writes;
	std::vector<std::pair<size_t, int> > shifts;

	reads.emplace_back(tape0, "[1_]");
	writes.emplace_back(tapeRax, "0");
	shifts.emplace_back(tapeRax, 1);
		
	builder.addTransition(q1, penultimateNode, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();

	// q1: else see 0: that's fine, move right
	const size_t q2 = builder.newNode();
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
 * handle assembly code of checking whether value at paramTape is negative or not
 * write 0 into rax if not negative; write a 1 into rax if it is negative
 * Function is inlined
 */
void handleIsNeg(MultiTapeBuilder &builder, const size_t paramTape, const size_t startNode, const size_t endNode) {
	const size_t q1 = startNode;
	const size_t penultimateNode = builder.newNode();

	// positive is when bits start with 0 then a [01]
	const size_t tape0 = paramTape;
	const size_t tapeRax = builder.tapeIndex("rax");

	// q1: if see 0 or _, not negative
	std::vector<std::pair<size_t, std::string> > reads;
	std::vector<std::pair<size_t, std::string> > writes;
	std::vector<std::pair<size_t, int> > shifts;
	
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
 * handle assembly code of adding values at paramTape0 and paramTape1
 * put sum in rax tape
 * No chance of having leading 0's in result
 * Make sure to put blank after answer in rax tho
 */
void handleBasicAdd(MultiTapeBuilder &builder, const size_t paramTape0, const size_t paramTape1, const size_t startNode, const size_t endNode) {
	const size_t tape0 = paramTape0;
	const size_t tape1 = paramTape1;
	const size_t tapeRax = builder.tapeIndex("rax");

	std::vector<std::pair<size_t, std::string> > reads;
	std::vector<std::pair<size_t, std::string> > writes;
	std::vector<std::pair<size_t, int> > shifts;
	
	// pad shorter argument with blanks until both have same length
	const size_t q4 = builder.newNode();
	handlePadding(builder, tape0, tape1, startNode, q4, true);

	// add the values in the two tapes. both values are positive.
	// when reading a blank and a non-blank, the blank represents a 0
	const size_t carryOff = q4;
	const size_t carryOn = builder.newNode();
	
	// carryOff: if see 0 and 0, write 0. no carry
	reads.emplace_back(tape0, "[0_]");
	reads.emplace_back(tape1, "0");
	writes.emplace_back(tapeRax, "0");
	shifts.emplace_back(tape0, 1);
	shifts.emplace_back(tape1, 1);
	shifts.emplace_back(tapeRax, 1);

	builder.addTransition(carryOff, carryOff, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();
	
	// same thing, but handle other blank: carryOff: if see 0 and 0, write 0. no carry
	reads.emplace_back(tape0, "0");
	reads.emplace_back(tape1, "[0_]");
	writes.emplace_back(tapeRax, "0");
	shifts.emplace_back(tape0, 1);
	shifts.emplace_back(tape1, 1);
	shifts.emplace_back(tapeRax, 1);

	builder.addTransition(carryOff, carryOff, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();

	// carryOff: if see 0 and 1, write a 1. no carry
	reads.emplace_back(tape0, "[0_]");
	reads.emplace_back(tape1, "1");
	writes.emplace_back(tapeRax, "1");
	shifts.emplace_back(tape0, 1);
	shifts.emplace_back(tape1, 1);
	shifts.emplace_back(tapeRax, 1);

	builder.addTransition(carryOff, carryOff, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();

	// carryOff: is see 1 and 0, write a 1. no carry
	reads.emplace_back(tape0, "1");
	reads.emplace_back(tape1, "[0_]");
	writes.emplace_back(tapeRax, "1");
	shifts.emplace_back(tape0, 1);
	shifts.emplace_back(tape1, 1);
	shifts.emplace_back(tapeRax, 1);

	builder.addTransition(carryOff, carryOff, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();

	// carryOff: is see 1 and 1, write a 0. yes carry
	reads.emplace_back(tape0, "1");
	reads.emplace_back(tape1, "1");
	writes.emplace_back(tapeRax, "0");
	shifts.emplace_back(tape0, 1);
	shifts.emplace_back(tape1, 1);
	shifts.emplace_back(tapeRax, 1);

	builder.addTransition(carryOff, carryOn, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();

	// carryOn: if see 0 and 0, write 1. no carry
	reads.emplace_back(tape0, "[0_]");
	reads.emplace_back(tape1, "0");
	writes.emplace_back(tapeRax, "1");
	shifts.emplace_back(tape0, 1);
	shifts.emplace_back(tape1, 1);
	shifts.emplace_back(tapeRax, 1);

	builder.addTransition(carryOn, carryOff, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();

	// same thing, but handle other blank: carryOn: if see 0 and 0, write 1. no carry
	reads.emplace_back(tape0, "0");
	reads.emplace_back(tape1, "[0_]");
	writes.emplace_back(tapeRax, "1");
	shifts.emplace_back(tape0, 1);
	shifts.emplace_back(tape1, 1);
	shifts.emplace_back(tapeRax, 1);

	builder.addTransition(carryOn, carryOff, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();

	// carryOn: if see 0 and 1, write a 0. yes carry
	reads.emplace_back(tape0, "[0_]");
	reads.emplace_back(tape1, "1");
	writes.emplace_back(tapeRax, "0");
	shifts.emplace_back(tape0, 1);
	shifts.emplace_back(tape1, 1);
	shifts.emplace_back(tapeRax, 1);

	builder.addTransition(carryOn, carryOn, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();

	// carryOn: is see 1 and 0, write a 0. yes carry
	reads.emplace_back(tape0, "1");
	reads.emplace_back(tape1, "[0_]");
	writes.emplace_back(tapeRax, "0");
	shifts.emplace_back(tape0, 1);
	shifts.emplace_back(tape1, 1);
	shifts.emplace_back(tapeRax, 1);

	builder.addTransition(carryOn, carryOn, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();

	// carryOn: is see 1 and 1, write a 1. yes carry
	reads.emplace_back(tape0, "1");
	reads.emplace_back(tape1, "1");
	writes.emplace_back(tapeRax, "1");
	shifts.emplace_back(tape0, 1);
	shifts.emplace_back(tape1, 1);
	shifts.emplace_back(tapeRax, 1);

	builder.addTransition(carryOn, carryOn, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();

	// now handle reading both _ and _ : reached end of input
	// need to write _ when overwriting a tape
	const size_t q5 = builder.newNode();

	reads.emplace_back(tape0, "_");
	reads.emplace_back(tape1, "_");
	writes.emplace_back(tapeRax, "_");
	shifts.emplace_back(tape0, -1);
	shifts.emplace_back(tape1, -1);
	shifts.emplace_back(tapeRax, -1);
		
	builder.addTransition(carryOff, q5, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();

	// but for carryOn, when both read _ and _, need to write 1 instead
	// also write a blank to the right of the 1
	const size_t q35 = builder.newNode();
	const size_t q36 = builder.newNode();

	reads.emplace_back(tape0, "_");
	reads.emplace_back(tape1, "_");
	writes.emplace_back(tapeRax, "1");
	shifts.emplace_back(tape0, -1);
	shifts.emplace_back(tape1, -1);
	shifts.emplace_back(tapeRax, 1);

	builder.addTransition(carryOn, q35, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();

	builder.add1TapeTransition(q35, q36, tapeRax, ".", "_", -1);
	builder.add1TapeTransition(q36, q5, tapeRax, ".", ".", -1);

	shifts.emplace_back(tape0, -1);
	shifts.emplace_back(tape1, -1);
	shifts.emplace_back(tapeRax, -1);
	
	// now keep moving left until read "_" and "_" again
	reads.emplace_back(tape0, "[01]");
	reads.emplace_back(tape1, "[01_]");

	builder.addTransition(q5, q5, reads, writes, shifts);
	
	reads.clear();
	reads.emplace_back(tape0, "[01_]");
	reads.emplace_back(tape1, "[01]");

	builder.addTransition(q5, q5, reads, writes, shifts);
	reads.clear();
	shifts.clear();

	reads.emplace_back(tape0, "_");
	reads.emplace_back(tape1, "_");
	shifts.emplace_back(tape0, 1);
	shifts.emplace_back(tape1, 1);
	shifts.emplace_back(tapeRax, 1);

	builder.addTransition(q5, endNode, reads, writes, shifts);
}

/**
 * handle assembly code of doing (A - B),
 * where A is value in paramTape0,
 * B is value in paramTape1
 */
void handleBasicSub(MultiTapeBuilder &builder, const size_t paramTape0, const size_t paramTape1, const size_t startNode, const size_t endNode) {
	const size_t tape0 = paramTape0;
	const size_t tape1 = paramTape1;
	const size_t tapeRax = builder.tapeIndex("rax");

	std::vector<std::pair<size_t, std::string> > reads;
	std::vector<std::pair<size_t, std::string> > writes;
	std::vector<std::pair<size_t, int> > shifts;

	// pad shorter argument with blanks until both have same length
	const size_t q4 = builder.newNode();
	handlePadding(builder, tape0, tape1, startNode, q4, true);
	
	// ok now back where we started from. now subtract!
	const size_t borrowOff = q4;
	const size_t borrowOn = builder.newNode();
	
	shifts.emplace_back(tape0, 1);
	shifts.emplace_back(tape1, 1);
	shifts.emplace_back(tapeRax, 1);

	// borrowOff: if see 0 and 0, answer is 0, go to borrowOff
	reads.emplace_back(tape0, "[0_]");
	reads.emplace_back(tape1, "0");
	writes.emplace_back(tapeRax, "0");

	builder.addTransition(borrowOff, borrowOff, reads, writes, shifts);
	reads.clear();
	writes.clear();
	
	// same thing, but handle if other is blank: borrowOff: if see 0 and 0, answer is 0, go to borrowOff
	reads.emplace_back(tape0, "0");
	reads.emplace_back(tape1, "[0_]");
	writes.emplace_back(tapeRax, "0");

	builder.addTransition(borrowOff, borrowOff, reads, writes, shifts);
	reads.clear();
	writes.clear();

	// borrowOff: if see 0 and 1, answer is 1, go to borrowOn
	reads.emplace_back(tape0, "[0_]");
	reads.emplace_back(tape1, "1");
	writes.emplace_back(tapeRax, "1");
	
	builder.addTransition(borrowOff, borrowOn, reads, writes, shifts);
	reads.clear();
	writes.clear();
	
	// borrowOff: if see 1 and 0, answer is 1, go to borrowOff
	reads.emplace_back(tape0, "1");
	reads.emplace_back(tape1, "[0_]");
	writes.emplace_back(tapeRax, "1");

	builder.addTransition(borrowOff, borrowOff, reads, writes, shifts);
	reads.clear();
	writes.clear();
	
	// borrowOff: if see 1 and 1, answer is 0, go to borrowOn
	reads.emplace_back(tape0, "1");
	reads.emplace_back(tape1, "1");
	writes.emplace_back(tapeRax, "0");

	builder.addTransition(borrowOff, borrowOff, reads, writes, shifts);
	reads.clear();
	writes.clear();
	
	// borrowOn: if see 0 and 0, answer is 1, go to borrowOn
	reads.emplace_back(tape0, "[0_]");
	reads.emplace_back(tape1, "0");
	writes.emplace_back(tapeRax, "1");

	builder.addTransition(borrowOn, borrowOn, reads, writes, shifts);
	reads.clear();
	writes.clear();
	
	// same thing, but if other is blank: borrowOn: if see 0 and 0, answer is 1, go to borrowOn
	reads.emplace_back(tape0, "0");
	reads.emplace_back(tape1, "[0_]");
	writes.emplace_back(tapeRax, "1");

	builder.addTransition(borrowOn, borrowOn, reads, writes, shifts);
	reads.clear();
	writes.clear();

	// borrowOn: if see 0 and 1, answer is 0, go to borrowOn
	reads.emplace_back(tape0, "[0_]");
	reads.emplace_back(tape1, "1");
	writes.emplace_back(tapeRax, "0");

	builder.addTransition(borrowOn, borrowOn, reads, writes, shifts);
	reads.clear();
	writes.clear();
	
	// borrowOn: if see 1 and 0, answer is 0, go to borrowOff
	reads.emplace_back(tape0, "1");
	reads.emplace_back(tape1, "[0_]");
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
	const size_t q6 = builder.newNode();
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
	const size_t encountered1 = builder.newNode();
	const size_t penultimateNode = builder.newNode();
	reads.emplace_back(tapeRax, "0");
	writes.emplace_back(tapeRax, "_");
	
	builder.addTransition(q6, q6, reads, writes, shifts);
	reads.clear();
	writes.clear();
	
	reads.emplace_back(tapeRax, "1");

	builder.addTransition(q6, encountered1, reads, writes, shifts);
	reads.clear();
	writes.clear();

	reads.emplace_back(tapeRax, "[01]");

	builder.addTransition(encountered1, encountered1, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();

	reads.emplace_back(tapeRax, "_");
	shifts.emplace_back(tape0, 1);
	shifts.emplace_back(tape1, 1);
	shifts.emplace_back(tapeRax, 1);

	// when see blank, go right
	builder.addTransition(q6, penultimateNode, reads, writes, shifts);
	builder.addTransition(encountered1, penultimateNode, reads, writes, shifts);

	// penultimateNode: if current tape head is blank, write a 0
	builder.add1TapeTransition(penultimateNode, endNode, tapeRax, "_", "0", 0);
	builder.add1TapeTransition(penultimateNode, endNode, tapeRax, "[01]", ".", 0);
}

/**
 * handle assembly code of doing A xor B
 * where A is value in paramTape0, B is value in paramTape1
 */
void handleBasicXor(MultiTapeBuilder &builder, const size_t paramTape0, const size_t paramTape1, const size_t startNode, const size_t endNode) {
	const size_t tape0 = paramTape0;
	const size_t tape1 = paramTape1;
	const size_t tapeRax = builder.tapeIndex("rax");

	std::vector<std::pair<size_t, std::string> > reads;
	std::vector<std::pair<size_t, std::string> > writes;
	std::vector<std::pair<size_t, int> > shifts;
	
	// pad shorter argument with blanks until both have same length
	const size_t q4 = builder.newNode();
	handlePadding(builder, tape0, tape1, startNode, q4, true);
	
	// ok now back where we started from. now do bit-wise xor!
	shifts.emplace_back(tape0, 1);
	shifts.emplace_back(tape1, 1);
	shifts.emplace_back(tapeRax, 1);

	// if see 0 and 0, write a 0
	reads.emplace_back(tape0, "[0_]");
	reads.emplace_back(tape1, "0");
	writes.emplace_back(tapeRax, "0");

	builder.addTransition(q4, q4, reads, writes, shifts);
	reads.clear();
	writes.clear();
	
	// same but if other is blank: if see 0 and 0, write a 0
	reads.emplace_back(tape0, "0");
	reads.emplace_back(tape1, "[0_]");
	writes.emplace_back(tapeRax, "0");

	builder.addTransition(q4, q4, reads, writes, shifts);
	reads.clear();
	writes.clear();

	// if see 0 and 1, write a 1
	reads.emplace_back(tape0, "[0_]");
	reads.emplace_back(tape1, "1");
	writes.emplace_back(tapeRax, "1");

	builder.addTransition(q4, q4, reads, writes, shifts);
	reads.clear();
	writes.clear();
	
	// if see 1 and 0, write a 1
	reads.emplace_back(tape0, "1");
	reads.emplace_back(tape1, "[0_]");
	writes.emplace_back(tapeRax, "1");

	builder.addTransition(q4, q4, reads, writes, shifts);
	reads.clear();
	writes.clear();
	
	// if see 1 and 1, write a 0
	reads.emplace_back(tape0, "1");
	reads.emplace_back(tape1, "1");
	writes.emplace_back(tapeRax, "0");

	builder.addTransition(q4, q4, reads, writes, shifts);
	reads.clear();
	writes.clear();

	shifts.clear();
	// if see _ and _, write a _, and move left
	const size_t q5 = builder.newNode();

	reads.emplace_back(tape0, "_");
	reads.emplace_back(tape1, "_");
	writes.emplace_back(tapeRax, "_"); // to delimit answer
	shifts.emplace_back(tape0, -1);
	shifts.emplace_back(tape1, -1);
	shifts.emplace_back(tapeRax, -1);

	builder.addTransition(q4, q5, reads, writes, shifts);
	reads.clear();
	writes.clear();
		
	// now move back left. also turn trailing 0's on the right into blanks
	// this time tho, answer could be just 0...
	const size_t encountered1 = builder.newNode();
	const size_t penultimateNode = builder.newNode();

	// while see both 0/1, and haven't encountered 1 in tapeRax, replace 0 with _
	reads.emplace_back(tapeRax, "0");
	writes.emplace_back(tapeRax, "_");
		
	builder.addTransition(q5, q5, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();
	
	// if see 1 in tapeRax tho, go to state encountered1
	reads.emplace_back(tapeRax, "1");
		
	builder.addTransition(q5, encountered1, reads, writes, shifts);
	reads.clear();
	writes.clear();

	// encountered1: while answer sees 0/1, go left
	reads.emplace_back(tapeRax, "[01]");
	shifts.emplace_back(tape0, -1);
	shifts.emplace_back(tape1, -1);
	shifts.emplace_back(tapeRax, -1);
		
	builder.addTransition(encountered1, encountered1, reads, writes, shifts);
	reads.clear();
	writes.clear();
	
	// if q5 or encountered1 sees _ and _, go to node penultimateNode
	shifts.clear();
	shifts.emplace_back(tape0, 1);
	shifts.emplace_back(tape1, 1);
	shifts.emplace_back(tapeRax, 1);

	reads.emplace_back(tape0, "_");
	reads.emplace_back(tape1, "_");
	
	builder.addTransition(q5, penultimateNode, reads, writes, shifts);
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
void handleBasicEq(MultiTapeBuilder &builder, const size_t paramTape0, const size_t paramTape1, const size_t startNode, const size_t endNode) {
	const size_t tape0 = paramTape0;
	const size_t tape1 = paramTape1;
	const size_t tapeRax = builder.tapeIndex("rax");

	std::vector<std::pair<size_t, std::string> > reads;
	std::vector<std::pair<size_t, std::string> > writes;
	std::vector<std::pair<size_t, int> > shifts;
	

	const size_t q3 = startNode;

	// when have written ans, but tape0 and tape1 heads need to go back left
	const size_t moveBackLeft = builder.newNode();

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
 * can assume both have no leading 0's, and both non-negative
 */
void handleBasicLt(MultiTapeBuilder &builder, const size_t paramTape0, const size_t paramTape1, const size_t startNode, const size_t endNode) {
	const size_t tape0 = paramTape0;
	const size_t tape1 = paramTape1;
	const size_t tapeRax = builder.tapeIndex("rax");

	std::vector<std::pair<size_t, std::string> > reads;
	std::vector<std::pair<size_t, std::string> > writes;
	std::vector<std::pair<size_t, int> > shifts;
	
	// pad shorter argument with blanks until both have same length, but don't go left
	const size_t q4 = builder.newNode();
	handlePadding(builder, tape0, tape1, startNode, q4, false);

	// ok now back where we started from. now do less-than!
	// starting from right side: that's where significant bits are
	// so start comparison here
	
	// node to signal when to start moving both tape heads back left
	const size_t moveBackLeft = builder.newNode();
	// node that answer was written
	const size_t penultimateNode = builder.newNode();

	// read 0 and 0: keep going left
	reads.emplace_back(tape0, "[0_]");
	reads.emplace_back(tape1, "0");
	shifts.emplace_back(tape0, -1);
	shifts.emplace_back(tape1, -1);

	builder.addTransition(q4, q4, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();

	// same, but if other was blank: read 0 and 0: keep going left
	reads.emplace_back(tape0, "0");
	reads.emplace_back(tape1, "[0_]");
	shifts.emplace_back(tape0, -1);
	shifts.emplace_back(tape1, -1);

	builder.addTransition(q4, q4, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();

	// read 0 and 1: A < B, so write 1 to rax. Go to moveBackLeft
	reads.emplace_back(tape0, "[0_]");
	reads.emplace_back(tape1, "1");
	writes.emplace_back(tapeRax, "1");
	shifts.emplace_back(tapeRax, 1);

	builder.addTransition(q4, moveBackLeft, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();
	
	// read 1 and 0: A > B, so write 0 to rax. Go to moveBackLeft
	reads.emplace_back(tape0, "1");
	reads.emplace_back(tape1, "[0_]");
	writes.emplace_back(tapeRax, "0");
	shifts.emplace_back(tapeRax, 1);

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

	// read _ and _: well A == B. Write 0 to rax, go to penultimateNode
	reads.emplace_back(tape0, "_");
	reads.emplace_back(tape1, "_");
	writes.emplace_back(tapeRax, "0");
	shifts.emplace_back(tape0, 1);
	shifts.emplace_back(tape1, 1);
	shifts.emplace_back(tapeRax, 1);

	builder.addTransition(q4, penultimateNode, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();

	// moveBackLeft: while both tapes don't have blanks, move left
	reads.emplace_back(tape0, "[01_]");
	reads.emplace_back(tape1, "[01]");
	shifts.emplace_back(tape0, -1);
	shifts.emplace_back(tape1, -1);
	
	builder.addTransition(moveBackLeft, moveBackLeft, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();

	// same but if other had blank: moveBackLeft: while both tapes don't have blanks, move left
	reads.emplace_back(tape0, "[01]");
	reads.emplace_back(tape1, "[01_]");
	shifts.emplace_back(tape0, -1);
	shifts.emplace_back(tape1, -1);
	
	builder.addTransition(moveBackLeft, moveBackLeft, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();

	// when go too far left, move back right 1
	reads.emplace_back(tape0, "_");
	reads.emplace_back(tape1, "_");
	shifts.emplace_back(tape0, 1);
	shifts.emplace_back(tape1, 1);

	builder.addTransition(moveBackLeft, penultimateNode, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();
	
	// penultimateNode: have tapeRax write a blank, then move back left
	builder.add1TapeTransition(penultimateNode, endNode, tapeRax, ".", "_", -1);
}

/**
 * handle assembly code of doing (-x)
 * where x is value at paramTape
 * Can assume x is non-zero
 */
void handleBasicNeg(MultiTapeBuilder &builder, const size_t paramTape, const size_t startNode, const size_t endNode) {
	const size_t q1 = startNode;

	const size_t tape0 = paramTape;
	const size_t tapeRax = builder.tapeIndex("rax");

	std::vector<std::pair<size_t, std::string> > reads;
	std::vector<std::pair<size_t, std::string> > writes;
	std::vector<std::pair<size_t, int> > shifts;
	
	// now flip sign bit
	// sign bit is the cell the head is currently on!
	const size_t q2 = builder.newNode();
	builder.add1TapeTransition(q1, q2, tape0, "0", "1", 0);
	builder.add1TapeTransition(q1, q2, tape0, "1", "0", 0);

	copyBetweenTapes(builder, tape0, tapeRax, q2, endNode);
}

/**
 * handle assembly code of doing 2*x
 * where x is value at parmTape
 * Can assume x is positive
 */
void handleBasicMul2(MultiTapeBuilder &builder, const size_t paramTape, const size_t startNode, const size_t endNode) {
	const size_t q1 = startNode;

	const size_t tape0 = paramTape;
	const size_t tapeRax = builder.tapeIndex("rax");

	std::vector<std::pair<size_t, std::string> > reads;
	std::vector<std::pair<size_t, std::string> > writes;
	std::vector<std::pair<size_t, int> > shifts;
	
	// ok now back where we started from. now double: answer is argument but shifted up
	
	// copy the sign bit
	const size_t q2 = builder.newNode();
	reads.emplace_back(tape0, "0");
	writes.emplace_back(tapeRax, "0");
	shifts.emplace_back(tape0, 1);
	shifts.emplace_back(tapeRax, 1);

	builder.addTransition(q1, q2, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();

	reads.emplace_back(tape0, "1");
	writes.emplace_back(tapeRax, "1");
	shifts.emplace_back(tape0, 1);
	shifts.emplace_back(tapeRax, 1);

	builder.addTransition(q1, q2, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();

	// write a 0 into least-significant bit (cuz even)
	const size_t q3 = builder.newNode();
	builder.add1TapeTransition(q2, q3, tapeRax, ".", "0", 1);

	// now copy digits from tape0 into tapeRax
	reads.emplace_back(tape0, "0");
	writes.emplace_back(tapeRax, "0");
	shifts.emplace_back(tape0, 1);
	shifts.emplace_back(tapeRax, 1);

	builder.addTransition(q3, q3, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();

	reads.emplace_back(tape0, "1");
	writes.emplace_back(tapeRax, "1");
	shifts.emplace_back(tape0, 1);
	shifts.emplace_back(tapeRax, 1);

	builder.addTransition(q3, q3, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();

	// when tape0 encounters _, write a _ too
	const size_t q4 = builder.newNode();

	reads.emplace_back(tape0, "_");
	writes.emplace_back(tapeRax, "_");
	shifts.emplace_back(tapeRax, -1);

	builder.addTransition(q3, q4, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();

	// now move both heads back
	reads.emplace_back(tapeRax, "[01]");
	shifts.emplace_back(tape0, -1);
	shifts.emplace_back(tapeRax, -1);

	builder.addTransition(q4, q4, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();

	reads.emplace_back(tapeRax, "_");
	shifts.emplace_back(tape0, 1);
	shifts.emplace_back(tapeRax, 1);
	
	builder.addTransition(q4, endNode, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();
}

/**
 * handle assembly code of doing x/2 (floored)
 * where x is value at paramTape
 * Can assume x is positive
 */
void handleBasicDiv2(MultiTapeBuilder &builder, const size_t paramTape, const size_t startNode, const size_t endNode) {
	const size_t q1 = startNode;

	const size_t tape0 = paramTape;
	const size_t tapeRax = builder.tapeIndex("rax");

	std::vector<std::pair<size_t, std::string> > reads;
	std::vector<std::pair<size_t, std::string> > writes;
	std::vector<std::pair<size_t, int> > shifts;
	
	// ok now back where we started from. now divide: answer is argument but without least-significant bit
	
	// copy the sign bit, tho logically should always be 0 because assumed is positive)
	const size_t q2 = builder.newNode();
	reads.emplace_back(tape0, "0");
	writes.emplace_back(tapeRax, "0");
	shifts.emplace_back(tape0, 1);
	shifts.emplace_back(tapeRax, 1);

	builder.addTransition(q1, q2, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();

	reads.emplace_back(tape0, "1");
	writes.emplace_back(tapeRax, "1");
	shifts.emplace_back(tape0, 1);
	shifts.emplace_back(tapeRax, 1);

	builder.addTransition(q1, q2, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();

	// ignore least-significant bit of argument by moving right
	const size_t q3 = builder.newNode();
	builder.add1TapeTransition(q2, q3, tape0, ".", ".", 1);

	// now copy bits to tapeRax
	reads.emplace_back(tape0, "0");
	writes.emplace_back(tapeRax, "0");
	shifts.emplace_back(tape0, 1);
	shifts.emplace_back(tapeRax, 1);

	builder.addTransition(q3, q3, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();

	reads.emplace_back(tape0, "1");
	writes.emplace_back(tapeRax, "1");
	shifts.emplace_back(tape0, 1);
	shifts.emplace_back(tapeRax, 1);

	builder.addTransition(q3, q3, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();

	// when argument sees blank, also write blank
	const size_t q4 = builder.newNode();
	reads.emplace_back(tape0, "_");
	writes.emplace_back(tapeRax, "_");
	shifts.emplace_back(tape0, -1);

	builder.addTransition(q3, q4, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();

	// now move both heads back
	reads.emplace_back(tape0, "[01]");
	shifts.emplace_back(tape0, -1);
	shifts.emplace_back(tapeRax, -1);

	builder.addTransition(q4, q4, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();

	reads.emplace_back(tape0, "_");
	shifts.emplace_back(tape0, 1);
	shifts.emplace_back(tapeRax, 1);

	builder.addTransition(q4, endNode, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();
}

/**
 * handle assembly code of reporting if (x % 2) == 0
 * where x is value at paramTape
 */
void handleIsEven(MultiTapeBuilder &builder, const size_t paramTape, const size_t startNode, const size_t endNode) {
	const size_t q1 = startNode;

	const size_t tape0 = paramTape;
	const size_t tapeRax = builder.tapeIndex("rax");

	std::vector<std::pair<size_t, std::string> > reads;
	std::vector<std::pair<size_t, std::string> > writes;
	std::vector<std::pair<size_t, int> > shifts;
	
	// ok now back where we started from. now divide: answer is (1 - least-significant bit)
	
	// move to lsb of tape0
	const size_t q2 = builder.newNode();
	builder.add1TapeTransition(q1, q2, tape0, ".", ".", 1);

	// now do 1 - current thing in tape0
	const size_t q3 = builder.newNode();
	reads.emplace_back(tape0, "[0_]");
	writes.emplace_back(tapeRax, "1");
	shifts.emplace_back(tapeRax, 1);

	builder.addTransition(q2, q3, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();

	reads.emplace_back(tape0, "1");
	writes.emplace_back(tapeRax, "0");
	shifts.emplace_back(tapeRax, 1);

	builder.addTransition(q2, q3, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();
	
	// write a blank afterwards in tapeRax; move both left
	writes.emplace_back(tapeRax, "_");
	shifts.emplace_back(tape0, -1);
	shifts.emplace_back(tapeRax, -1);

	builder.addTransition(q3, endNode, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();
}

/**
 * handle assembly code of reporting if (x % 2) == 1
 * where x is value at paramTape
 */
void handleIsOdd(MultiTapeBuilder &builder, const size_t paramTape, const size_t startNode, const size_t endNode) {
	const size_t q1 = startNode;

	const size_t tape0 = paramTape;
	const size_t tapeRax = builder.tapeIndex("rax");

	std::vector<std::pair<size_t, std::string> > reads;
	std::vector<std::pair<size_t, std::string> > writes;
	std::vector<std::pair<size_t, int> > shifts;
	
	// ok now back where we started from. now divide: answer is (least-significant bit)
	
	// move to lsb of tape0
	const size_t q2 = builder.newNode();
	builder.add1TapeTransition(q1, q2, tape0, ".", ".", 1);

	// now write current thing in tape0
	const size_t q3 = builder.newNode();
	reads.emplace_back(tape0, "[0_]");
	writes.emplace_back(tapeRax, "0");
	shifts.emplace_back(tapeRax, 1);

	builder.addTransition(q2, q3, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();

	reads.emplace_back(tape0, "1");
	writes.emplace_back(tapeRax, "1");
	shifts.emplace_back(tapeRax, 1);

	builder.addTransition(q2, q3, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();
	
	// write a blank afterwards in tapeRax; move both left
	writes.emplace_back(tapeRax, "_");
	shifts.emplace_back(tape0, -1);
	shifts.emplace_back(tapeRax, -1);

	builder.addTransition(q3, endNode, reads, writes, shifts);
	reads.clear();
	writes.clear();
	shifts.clear();
}

/**
 * handle assembly code of putting tape bitIndex value into rax
 */
void handleGetMemBitIndex(MultiTapeBuilder &builder, const size_t startNode, const size_t endNode) {
	const size_t tapeBitIndex = builder.tapeIndex("bitIndex");
	const size_t tapeRax = builder.tapeIndex("rax");

	copyBetweenTapes(builder, tapeBitIndex, tapeRax, startNode, endNode);
}

/**
 * handle assembly code of putting a value into tape bitIndex
 */
void handleSetMemBitIndex(MultiTapeBuilder &builder, const size_t paramTape, const size_t startNode, const size_t endNode) {
	const size_t tapeBitIndex = builder.tapeIndex("bitIndex");
	copyBetweenTapes(builder, paramTape, tapeBitIndex, startNode, endNode);
}

/**
 * handle assembly code of moving head of tape "bits" right
 */
void handleMoveMemHeadRight(MultiTapeBuilder &builder, const size_t startNode, const size_t endNode) {
	builder.add1TapeTransition(startNode, endNode, builder.tapeIndex("bits"), ".", ".", 1);
}

/**
 * handle assembly code of moving head of tape "bits" left
 */
void handleMoveMemHeadLeft(MultiTapeBuilder &builder, const size_t startNode, const size_t endNode) {
	builder.add1TapeTransition(startNode, endNode, builder.tapeIndex("bits"), ".", ".", -1);
}

/**
 * handle assembly code of moving head of setting bit of tape "bits" to 0
 */
void handleSetMemBitZero(MultiTapeBuilder &builder, const size_t startNode, const size_t endNode) {
	builder.add1TapeTransition(startNode, endNode, builder.tapeIndex("bits"), ".", "0", 0);
}

/**
 * handle assembly code of moving head of setting bit of tape "bits" to 1
 */
void handleSetMemBitOne(MultiTapeBuilder &builder, const size_t startNode, const size_t endNode) {
	builder.add1TapeTransition(startNode, endNode, builder.tapeIndex("bits"), ".", "1", 0);
}

/**
 * handle assembly code of moving head of setting bit of tape "bits" to blank
 */
void handleSetMemBitBlank(MultiTapeBuilder &builder, const size_t startNode, const size_t endNode) {
	builder.add1TapeTransition(startNode, endNode, builder.tapeIndex("bits"), ".", "_", 0);
}

/**
 * handle assembly code of checking if bit at head of tape "bits" is 0
 */
void handleMemBitIsZero(MultiTapeBuilder &builder, const size_t startNode, const size_t endNode) {
	const size_t tapeBitIndex = builder.tapeIndex("bits");
	const size_t tapeRax = builder.tapeIndex("rax");

	std::vector<std::pair<size_t, std::string> > reads;
	std::vector<std::pair<size_t, std::string> > writes;
	std::vector<std::pair<size_t, int> > shifts;
	
	const size_t q = builder.newNode();

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
void handleMemBitIsOne(MultiTapeBuilder &builder, const size_t startNode, const size_t endNode) {
	const size_t tapeBitIndex = builder.tapeIndex("bits");
	const size_t tapeRax = builder.tapeIndex("rax");

	std::vector<std::pair<size_t, std::string> > reads;
	std::vector<std::pair<size_t, std::string> > writes;
	std::vector<std::pair<size_t, int> > shifts;
	
	const size_t q = builder.newNode();

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
void handleMemBitIsBlank(MultiTapeBuilder &builder, const size_t startNode, const size_t endNode) {
	const size_t tapeBitIndex = builder.tapeIndex("bits");
	const size_t tapeRax = builder.tapeIndex("rax");

	std::vector<std::pair<size_t, std::string> > reads;
	std::vector<std::pair<size_t, std::string> > writes;
	std::vector<std::pair<size_t, int> > shifts;
	
	const size_t q = builder.newNode();

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
void handleNextInt(MultiTapeBuilder &builder, const size_t startNode, const size_t endNode) {
	const size_t tapeInput = builder.tapeIndex("input");
	const size_t tapeRax = builder.tapeIndex("rax");

	std::vector<std::pair<size_t, std::string> > reads;
	std::vector<std::pair<size_t, std::string> > writes;
	std::vector<std::pair<size_t, int> > shifts;
	
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

	const size_t q0 = builder.newNode();
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
void handlePrintSpace(MultiTapeBuilder &builder, const size_t startNode, const size_t endNode) {
	builder.add1TapeTransition(startNode, endNode, builder.tapeIndex("output"), ".", "_", 1);
}

/**
 * handle assembly code of printing an int
 */
void handlePrintInt(MultiTapeBuilder &builder, const size_t paramTape, const size_t startNode, const size_t endNode) {
	const size_t q1 = builder.newNode();
	const size_t tapeOutput = builder.tapeIndex("output");

	copyBetweenTapes(builder, paramTape, tapeOutput, startNode, q1);

	// currently at left of num in output. move right until blank
	builder.add1TapeTransition(q1, q1, tapeOutput, "[01]", ".", 1);
	builder.add1TapeTransition(q1, endNode, tapeOutput, "_", ".", 0);
}

/**
 * handle assembly code of call to a line number
 */
void handleCallNum(MultiTapeBuilder &builder, const size_t currIP, const std::vector<std::string> &words) {
	const size_t lineNum = (size_t) std::stoi(words[1]);

	const size_t q0 = builder.newNode();
	handleIPTransition(builder, currIP, builder.node("after"), q0);

	size_t prevNode = q0;
	// then push new stack frames to all varTapes
	for(size_t i = 0; i < builder.numVars; ++i) {
		const size_t tape = builder.tapeIndex("variables") + i;
		const size_t q = builder.newNode();
		pushEmptyFrame(builder, tape, prevNode, q);
		prevNode = q;
	}

	// store currIP into ipStack
	const size_t q1 = builder.newNode();
	const size_t q2 = builder.newNode();
	pushEmptyFrame(builder, builder.tapeIndex("ipStack"), prevNode, q1);
	copyBetweenTapes(builder, builder.tapeIndex("ip"), builder.tapeIndex("ipStack"), q1, q2);
	
	// set new ip
	std::vector<std::string> bits = lineNumToBits(lineNum, builder.ipSize);
		
	// write bits into ip tape
	prevNode = q2;
	const size_t tapeIP = builder.tapeIndex("ip");
	for(size_t i = 0; i < builder.ipSize; ++i) {
		const size_t q = builder.newNode();
		builder.add1TapeTransition(prevNode, q, tapeIP, ".", bits[i], 1);
		prevNode = q;
	}
	
	// well now move head of ip tape back
	for(size_t i = 0; i < builder.ipSize; ++i) {
		const size_t q = builder.newNode();
		builder.add1TapeTransition(prevNode, q, tapeIP, ".", ".", -1);
		prevNode = q;
	}
	
	// now connect with node "sideways"
	builder.add1TapeTransition(prevNode, builder.tapeIndex("ipSideways"), tapeIP, ".", ".", 0);
}

/**
 * handle assembly code of calling "jf <tape> <line#>": 
 * jump to line# if tape value is false (which is bit 0)
 */
void handleJf(MultiTapeBuilder &builder, const size_t currIP, const std::vector<std::string> &words) {
	const size_t q0 = builder.newNode();
	handleIPTransition(builder, currIP, builder.node("after"), q0);

	size_t valTape;
	if(words[1] == "!TAPE_RAX") {
		valTape = builder.tapeIndex("rax");
	}
	else if(words[1].size() >= 10 && words[1].substr(0, 10) == "!TAPE_tape") {
		valTape = builder.tapeIndex("variables") + parseTapeNum(words[1]);
	}
	else {
		throw std::invalid_argument("Jf: Invalid line " + std::to_string(currIP));
	}
	
	const size_t lineNum = (size_t) std::stoi(words[2]);
	
	// q0: if read a 0, go to q1
	const size_t q1 = builder.newNode();
	builder.add1TapeTransition(q0, q1, valTape, "0", ".", 0);
	
	// q0: if u read a 1 (or blank), go to node "before"
	builder.add1TapeTransition(q0, builder.node("before"), valTape, "[1_]", ".", 0);
	
	// q1: well valTape had a false. transition to new line!
	// aka we set ipTape to lineNum
	
	// set new ip
	std::vector<std::string> bits = lineNumToBits(lineNum, builder.ipSize);
		
	// write bits into ip tape
	size_t prevNode = q1;
	const size_t tapeIP = builder.tapeIndex("ip");
	for(size_t i = 0; i < builder.ipSize; ++i) {
		const size_t q = builder.newNode();
		builder.add1TapeTransition(prevNode, q, tapeIP, ".", bits[i], 1);
		prevNode = q;
	}
	
	// well now move head of ip tape back
	for(size_t i = 0; i < builder.ipSize; ++i) {
		const size_t q = builder.newNode();
		builder.add1TapeTransition(prevNode, q, tapeIP, ".", ".", -1);
		prevNode = q;
	}
	
	// now connect with node "sideways"
	builder.add1TapeTransition(prevNode, builder.tapeIndex("ipSideways"), tapeIP, ".", ".", 0);
}

/**
 * handle assembly code of calling return
 */
void handleReturn(MultiTapeBuilder &builder, const size_t currIP) {
	// first transition by reading ip
	const size_t q0 = builder.newNode();
	handleIPTransition(builder, currIP, builder.node("after"), q0);

	size_t prevNode = q0;

	// pop off all the pushed stack frames
	for(size_t i = 0; i < builder.numVars; ++i) {
		const size_t tape = builder.tapeIndex("variables") + i;
		const size_t q = builder.newNode();
		popOffTop(builder, tape, prevNode, q);
		prevNode = q;
	}

	// pop last value of ipStack into ip
	const size_t tapeIPStack = builder.tapeIndex("ipStack");
	const size_t tapeIP = builder.tapeIndex("ip");
	const size_t q1 = builder.newNode();

	copyBetweenTapes(builder, tapeIPStack, tapeIP, prevNode, q1);
	popOffTop(builder, tapeIPStack, q1, builder.node("before"));

	// ip must be incremented, so connected to node "before"
}

/**
 * handle assembly code of not of a boolean
 */
void handleNot(MultiTapeBuilder &builder, const size_t currIP, const std::vector<std::string> &words) {
	// arg ! = dest ; 
	const size_t argTape = builder.tapeIndex("variables") + parseTapeNum(words[0]);
	const size_t destTape = builder.tapeIndex("variables") + parseTapeNum(words[3]);

	// first transition by reading ip
	const size_t q0 = builder.newNode();
	handleIPTransition(builder, currIP, builder.node("after"), q0);

	std::vector<std::pair<size_t, std::string> > reads;
	std::vector<std::pair<size_t, std::string> > writes;
	std::vector<std::pair<size_t, int> > shifts;

	const size_t penultimateNode = builder.newNode();

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

	const size_t endNode = builder.node("before");
	
	// once written answer to dest, write a blank after, then move back left
	builder.add1TapeTransition(penultimateNode, endNode, destTape, ".", "_", -1);
}

/**
 * handle assembly code of AND of 2 booleans
 */
void handleAnd(MultiTapeBuilder &builder, const size_t currIP, const std::vector<std::string> &words) {
	// arg1 arg2 && = dest ; 
	const size_t arg1Tape = builder.tapeIndex("variables") + parseTapeNum(words[0]);
	const size_t arg2Tape = builder.tapeIndex("variables") + parseTapeNum(words[1]);
	const size_t destTape = builder.tapeIndex("variables") + parseTapeNum(words[4]);

	// first transition by reading ip
	const size_t q0 = builder.newNode();
	handleIPTransition(builder, currIP, builder.node("after"), q0);

	std::vector<std::pair<size_t, std::string> > reads;
	std::vector<std::pair<size_t, std::string> > writes;
	std::vector<std::pair<size_t, int> > shifts;

	const size_t penultimateNode = builder.newNode();

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

	const size_t endNode = builder.node("before");
	
	// once written answer to dest, write a blank after, then move back left
	builder.add1TapeTransition(penultimateNode, endNode, destTape, ".", "_", -1);
}

/**
 * handle assembly code of OR of 2 booleans
 */
void handleOr(MultiTapeBuilder &builder, const size_t currIP, const std::vector<std::string> &words) {
	// arg1 arg2 && = dest ; 
	const size_t arg1Tape = builder.tapeIndex("variables") + parseTapeNum(words[0]);
	const size_t arg2Tape = builder.tapeIndex("variables") + parseTapeNum(words[1]);
	const size_t destTape = builder.tapeIndex("variables") + parseTapeNum(words[4]);

	// first transition by reading ip
	const size_t q0 = builder.newNode();
	handleIPTransition(builder, currIP, builder.node("after"), q0);

	std::vector<std::pair<size_t, std::string> > reads;
	std::vector<std::pair<size_t, std::string> > writes;
	std::vector<std::pair<size_t, int> > shifts;

	const size_t penultimateNode = builder.newNode();

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

	const size_t endNode = builder.node("before");
	
	// once written answer to dest, write a blank after, then move back left
	builder.add1TapeTransition(penultimateNode, endNode, destTape, ".", "_", -1);
}

/**
 * handle assignment, but between 2 tapes. Like "tape1 tape2 = ;", 
 * meaning copy value of tape2 to tape1
 */
void handleAssignmentBetweenTapes(MultiTapeBuilder &builder, const size_t currIP, const std::vector<std::string> &words) {
	// first transition by reading ip
	const size_t q0 = builder.newNode();
	handleIPTransition(builder, currIP, builder.node("after"), q0);

	size_t fromTape;
	if(words[1] == "!TAPE_RAX") {
		fromTape = builder.tapeIndex("rax");
	}
	else if(words[1].size() >= 10 && words[1].substr(0, 10) == "!TAPE_tape") {
		fromTape = builder.tapeIndex("variables") + parseTapeNum(words[1]);
	}
	else {
		throw std::invalid_argument("Assignment: Invalid line " + std::to_string(currIP));
	}

	size_t toTape;
	if(words[0] == "!TAPE_RAX") {
		toTape = builder.tapeIndex("rax");
	}
	else if(words[0].size() >= 10 && words[0].substr(0, 10) == "!TAPE_tape") {
		toTape = builder.tapeIndex("variables") + parseTapeNum(words[0]);
	}
	else{
		throw std::invalid_argument("Assignment: Invalid line " + std::to_string(currIP));
	}

	const size_t endNode = builder.node("before");
	copyBetweenTapes(builder, fromTape, toTape, q0, endNode);
}

/**
 * handle assignment from "true" to a tape
 */
void handleAssignmentTrue(MultiTapeBuilder &builder, const size_t currIP, const std::vector<std::string> &words) {
	// first transition by reading ip
	const size_t q0 = builder.newNode();
	handleIPTransition(builder, currIP, builder.node("after"), q0);

	size_t toTape;
	if(words[0] == "!TAPE_RAX") {
		toTape = builder.tapeIndex("rax");
	}
	else if(words[0].size() >= 10 && words[0].substr(0, 10) == "!TAPE_tape") {
		toTape = builder.tapeIndex("variables") + parseTapeNum(words[0]);
	}
	else {
		throw std::invalid_argument("Assign true: Invalid line " + std::to_string(currIP));
	}

	const size_t q1 = builder.newNode();
	const size_t endNode = builder.node("before");

	builder.add1TapeTransition(q0, q1, toTape, ".", "1", 1);
	builder.add1TapeTransition(q1, endNode, toTape, ".", "_", -1);
}

/**
 * handle assignment from "false" to a tape
 */
void handleAssignmentFalse(MultiTapeBuilder &builder, const size_t currIP, const std::vector<std::string> &words) {
	// first transition by reading ip
	const size_t q0 = builder.newNode();
	handleIPTransition(builder, currIP, builder.node("after"), q0);

	size_t toTape;
	if(words[0] == "!TAPE_RAX") {
		toTape = builder.tapeIndex("rax");
	}
	else if(words[0].size() >= 10 && words[0].substr(0, 10) == "!TAPE_tape") {
		toTape = builder.tapeIndex("variables") + parseTapeNum(words[0]);
	}
	else {
		throw std::invalid_argument("Assign false: Invalid line " + std::to_string(currIP));
	}

	const size_t q1 = builder.newNode();
	const size_t endNode = builder.node("before");

	builder.add1TapeTransition(q0, q1, toTape, ".", "0", 1);
	builder.add1TapeTransition(q1, endNode, toTape, ".", "_", -1);
}

/**
 * handle assignment from integer literal to tape
 */
void handleAssignmentIntegerLiteral(MultiTapeBuilder &builder, const size_t currIP, const std::vector<std::string> &words) {
	// first transition by reading ip
	const size_t q0 = builder.newNode();
	handleIPTransition(builder, currIP, builder.node("after"), q0);

	size_t toTape;
	if(words[0] == "!TAPE_RAX") {
		toTape = builder.tapeIndex("rax");
	}
	else if(words[0].size() >= 10 && words[0].substr(0, 10) == "!TAPE_tape") {
		toTape = builder.tapeIndex("variables") + parseTapeNum(words[0]);
	}
	else {
		throw std::invalid_argument("Assign integer literal: Invalid line " + std::to_string(currIP));
	}
	
	const std::string bits = convertIntegerToBits(words[1]);
	size_t prevNode = q0;

	for(size_t i = 0; i < bits.size(); ++i) {
		const size_t q = builder.newNode();
		const std::string writeBit = std::string(1, bits[i]);
		builder.add1TapeTransition(prevNode, q, toTape, ".", writeBit, 1);
		prevNode = q;
	}

	const size_t q1 = builder.newNode();
	builder.add1TapeTransition(prevNode, q1, toTape, ".", "_", 0);
	prevNode = q1;

	// move left
	for(size_t i = 0; i < bits.size(); ++i) {
		const size_t q = builder.newNode();
		builder.add1TapeTransition(prevNode, q, toTape, ".", ".", -1);
		prevNode = q;
	}
	
	builder.add1TapeTransition(prevNode, builder.node("before"), toTape, ".", ".", 0);
}

MultiTapeTuringMachine assemblyToMultiTapeTuringMachine(const std::vector<std::string> &assembly) {
	MultiTapeBuilder builder(assembly);
	
	initialize(builder);

	std::cout << "Initialization complete" << std::endl;

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
			const size_t q0 = builder.newNode();
			handleIPTransition(builder, i, builder.node("after"), q0);

			const std::string func = words[1].substr(10, words[1].size() - 10);

			size_t prevNode = q0;
			const size_t q1 = builder.newNode();	

			if(func == "isZero") {
				handleIsZero(builder, builder.tapeIndex("variables") + parseTapeNum(words[2]), prevNode, q1);
			}
			else if(func == "isPos") {
				handleIsPos(builder, builder.tapeIndex("variables") + parseTapeNum(words[2]), prevNode, q1);
			}
			else if(func == "isNeg") {
				handleIsNeg(builder, builder.tapeIndex("variables") + parseTapeNum(words[2]), prevNode, q1);
			}
			else if(func == "basic_add") {
				const size_t paramTape0 = builder.tapeIndex("variables") + parseTapeNum(words[2]);
				const size_t paramTape1 = builder.tapeIndex("variables") + parseTapeNum(words[3]);
				handleBasicAdd(builder, paramTape0, paramTape1, prevNode, q1);
			}
			else if(func == "basic_sub") {
				const size_t paramTape0 = builder.tapeIndex("variables") + parseTapeNum(words[2]);
				const size_t paramTape1 = builder.tapeIndex("variables") + parseTapeNum(words[3]);
				handleBasicSub(builder, paramTape0, paramTape1, prevNode, q1);
			}
			else if(func == "basic_xor") {
				const size_t paramTape0 = builder.tapeIndex("variables") + parseTapeNum(words[2]);
				const size_t paramTape1 = builder.tapeIndex("variables") + parseTapeNum(words[3]);
				handleBasicXor(builder, paramTape0, paramTape1, prevNode, q1);
			}
			else if(func == "basic_eq") {
				const size_t paramTape0 = builder.tapeIndex("variables") + parseTapeNum(words[2]);
				const size_t paramTape1 = builder.tapeIndex("variables") + parseTapeNum(words[3]);
				handleBasicEq(builder, paramTape0, paramTape1, prevNode, q1);
			}
			else if(func == "basic_lt") {
				const size_t paramTape0 = builder.tapeIndex("variables") + parseTapeNum(words[2]);
				const size_t paramTape1 = builder.tapeIndex("variables") + parseTapeNum(words[3]);
				handleBasicLt(builder, paramTape0, paramTape1, prevNode, q1);
			}
			else if(func == "basic_neg") {
				const size_t paramTape = builder.tapeIndex("variables") + parseTapeNum(words[2]);
				handleBasicNeg(builder, paramTape, prevNode, q1);
			}
			else if(func == "basic_mul2") {
				const size_t paramTape = builder.tapeIndex("variables") + parseTapeNum(words[2]);
				handleBasicMul2(builder, paramTape, prevNode, q1);
			}
			else if(func == "basic_div2") {
				const size_t paramTape = builder.tapeIndex("variables") + parseTapeNum(words[2]);
				handleBasicDiv2(builder, paramTape, prevNode, q1);
			}
			else if(func == "isEven") {
				const size_t paramTape = builder.tapeIndex("variables") + parseTapeNum(words[2]);
				handleIsEven(builder, paramTape, prevNode, q1);
			}
			else if(func == "isOdd") {
				const size_t paramTape = builder.tapeIndex("variables") + parseTapeNum(words[2]);
				handleIsOdd(builder, paramTape, prevNode, q1);
			}
			else if(func == "getMemBitIndex") {
				handleGetMemBitIndex(builder, prevNode, q1);
			}
			else if(func == "setMemBitIndex") {
				const size_t paramTape = builder.tapeIndex("variables") + parseTapeNum(words[2]);
				handleSetMemBitIndex(builder, paramTape, prevNode, q1);
			}
			else if(func == "moveMemHeadRight") {
				handleMoveMemHeadRight(builder, prevNode, q1);
			}
			else if(func == "moveMemHeadLeft") {
				handleMoveMemHeadLeft(builder, prevNode, q1);
			}
			else if(func == "setMemBitZero") {
				handleSetMemBitZero(builder, prevNode, q1);
			}
			else if(func == "setMemBitOne") {
				handleSetMemBitOne(builder, prevNode, q1);
			}
			else if(func == "setMemBitBlank") {
				handleSetMemBitBlank(builder, prevNode, q1);
			}
			else if(func == "memBitIsZero") {
				handleMemBitIsZero(builder, prevNode, q1);
			}
			else if(func == "memBitIsOne") {
				handleMemBitIsOne(builder, prevNode, q1);
			}
			else if(func == "memBitIsBlank") {
				handleMemBitIsBlank(builder, prevNode, q1);
			}
			else if(func == "nextInt") {
				handleNextInt(builder, prevNode, q1);
			}
			else if(func == "printSpace") {
				handlePrintSpace(builder, prevNode, q1);
			}
			else if(func == "printInt") {
				const size_t paramTape = builder.tapeIndex("variables") + parseTapeNum(words[2]);
				handlePrintInt(builder, paramTape, prevNode, q1);
			}
			else {
				throw std::invalid_argument("Parsing Invalid line " + std::to_string(i));
			}

			// now connect from q1 to node "before"
			builder.add1TapeTransition(q1, builder.node("before"), builder.tapeIndex("variables"), ".", ".", 0);
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

	std::cout << "Finished building" << std::endl;

	/*
	std::cout << "ipSize = " << builder.ipSize << std::endl;
	std::cout << "numVars = " << builder.numVars << std::endl;
	*/

	std::cout << "tapes:" << std::endl;
	std::vector<std::string> tapeNames {"input", "output", "ipStack", "ip", "ipSideways", "paramStack", "bitIndex", "bits", "variables", "rax"};
	for(std::string s : tapeNames) {
		size_t num = builder.tapeIndex(s);
		std::cout << "tape " << s << " = " << num << std::endl;
	}

	return builder.generateMTTM(builder.node("start"), builder.node("end"));
}

