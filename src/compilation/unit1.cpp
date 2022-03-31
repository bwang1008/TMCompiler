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
void addIncrementIP(MultiTapeBuilder &builder) {
	// ip is in 2's complement
	// worked out on paper
	
	int q0 = builder.node("before");
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
 * If assembly line is nop, and reach ip == currIP,
 * then connect node "after" to "before"
 */
void handleNop(MultiTapeBuilder &builder, size_t currIP) {
	int q0 = builder.node("before");
	int q1 = builder.node("after");
	handleIPTransition(builder, currIP, q1, q0);
}

/**
 * Pop either from PARAMS or RAX; moved to a varTape
 */
void handlePop(MultiTapeBuilder &builder, size_t currIP, std::string &line) {
	std::vector<std::string> words = getWords(line);

	int fromTape;
	if(words[1] == "!TAPE_PARAMS") {
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

	std::vector<std::pair<int, std::string> > reads;
	std::vector<std::pair<int, std::string> > writes;
	std::vector<std::pair<int, int> > shifts;






	// copy
	
	// if from paramStack, erase from stack; else just move left. also move toTape's copy left
	
	// connect to node "before"
}

MultiTapeTuringMachine assemblyToMultiTapeTuringMachine(std::vector<std::string> &assembly) {
	int numVars = countTapeVariables(assembly);

	// initialize tapes with names
	std::vector<std::pair<std::string, int> > tapeCounts;
	
	tapeCounts.push_back(std::make_pair("input", 1));
	tapeCounts.push_back(std::make_pair("output", 1));
	tapeCounts.push_back(std::make_pair("ipStack", 1));
	tapeCounts.push_back(std::make_pair("ip", 1));
	tapeCounts.push_back(std::make_pair("ipSideways", p));
	tapeCounts.push_back(std::make_pair("paramStack", 1));
	tapeCounts.push_back(std::make_pair("bitIndex", 1));
	tapeCounts.push_back(std::make_pair("bits", 1));
	tapeCounts.push_back(std::make_pair("variables", numVars));
	tapeCounts.push_back(std::make_pair("rax", 1));

	MultiTapeBuilder builder(tapeCounts, assembly.size());
	
	initialize(builder);

	for(size_t i = 0; i < assembly.size(); ++i) {
		std::vector<std::string> words = getWords(assembly[i]);
		
		if(words[0] == "nop") {
			handleNop(builder, i);
		}
		else if(words[0] == "pop") {
			
		}
	}

	/*
	std::vector<Transition> transitions;
	transitions.emplace_back(1, "what", 1, "lmao", std::vector<int> {0, 0, 0});
	MultiTapeTuringMachine mttm(3, 5, 0, 1, transitions);
	return mttm;
	*/
}
