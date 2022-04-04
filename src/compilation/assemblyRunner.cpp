/**
 * Run the assembly code generated.
 */

#include "utils.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

int DEBUG = 0;

int getTapeNum(std::string s) {
	return std::stoi(s.substr(10, s.size() - 10));
}

void simulateAssembly(std::vector<std::string> &program) {
	// determine how many variable tapes there are
	int numTapes = 0;
	for(size_t i = 0; i < program.size(); ++i) {
		std::vector<std::string> words = getWords(program[i]);
		for(size_t j = 0; j < words.size(); ++j) {
			if(words[j].substr(0, 10) == "!TAPE_tape") {
				int val = getTapeNum(words[j]);
				if(val > numTapes) {
					numTapes = val;
				}
			}
		}
	}
	
	++numTapes;
	std::cout << "numTapes = " << numTapes << std::endl;

	std::vector<int> ipStack {-2};
	int ip = 0;

	std::vector<int> paramStack;

	int bitIndex = 0;
	int bitHead = 0;
	std::vector<int> bits {2};

	/*
	// just for debugging: set MEM[2] = 5:     bits[9] = 0, bits[13] = 1, bits[18] = 0, bits[24] = 1
	std::vector<int> bits;
	for(size_t i = 0; i < 39; ++i) {
		bits.push_back(2);	
	}
	bits[9] = 0;
	bits[13] = 1;
	bits[18] = 0;
	bits[24] = 1;
	*/

	int rax = 0;

	std::vector<std::vector<int> > varTapes(numTapes, std::vector<int>(1, 0));

	int numSteps = 0;

	while(true) {
		if(DEBUG == 1) {
			std::cout << "\nWe just concluded the prev step. Summary:" << std::endl;
			std::cout << "params = [";
			for(size_t i = 0; i < paramStack.size(); ++i) {
				std::cout << paramStack[i];
				if(i + 1 < paramStack.size()) {
					std::cout << ", ";
				}
			}
			std::cout << "]" << std::endl;
			for(size_t i = 0; i < varTapes.size(); ++i) {
				std::cout << "tape " << i << ": ";
				for(size_t j = 0; j < varTapes[i].size(); ++j) {
					std::cout << varTapes[i][j] << " ";
				}
				std::cout << std::endl;
			}
			std::cout << "rax: " << rax << std::endl;

			std::cout << "\n\n\n";	
		}
		//std::cout << "IP = " << ip << std::endl;
		numSteps++;

		if(ip == -1) {
			break;
		}

		//std::cout << "line = " << program[ip] << std::endl;

		std::vector<std::string> words = getWords(program[ip]);

		if(words[0] == "nop") {

		}
		else if(words[0] == "jmp") {
			int dest = std::stoi(words[1]);
			ip = dest;
			continue;
		}
		else if(words[0] == "pop") {
			int val = 0;
			if(words[1] == "!TAPE_PARAMS") {
				val = paramStack[paramStack.size() - 1];
				paramStack.pop_back();
			}
			else if(words[1] == "!TAPE_RAX") {
				val = rax;
			}

			int dest = getTapeNum(words[2]);
			
			if(varTapes[dest].size() == 0) {
				varTapes[dest].push_back(val);
			}
			else {
				int s = varTapes[dest].size();
				varTapes[dest][s - 1] = val;
			}
		}
		else if(words[0] == "push") {
			int val = 0;
			if(words[1].substr(0, 10) == "!TAPE_tape") {
				int t = getTapeNum(words[1]);
				val = varTapes[t][varTapes[t].size() - 1];
			}
			else if(words[1] == "!TAPE_RAX") {
				val = rax;
			}

			if(words[2] == "!TAPE_PARAMS") {
				paramStack.push_back(val);
			}
		}
		else if(words[0] == "call" && words[1].substr(0, 10) == "!FUNC_LIB_") {
			// call implicitly pushes stack frames to varTapes
			for(size_t i = 0; i < varTapes.size(); ++i) {
				varTapes[i].push_back(0);
			}

			std::string func = words[1].substr(10, words[1].size() - 10); // cuz "!FUNC_LIB_isZero"
			if(func == "isZero") {
				int p1 = paramStack.back();
				paramStack.pop_back();

				rax = (p1 == 0) ? 1 : 0;
			}
			else if(func == "isPos") {
				int p1 = paramStack.back();
				paramStack.pop_back();
				rax = (p1 > 0) ? 1 : 0;
			}
			else if(func == "isNeg") {
				int p1 = paramStack.back();
				paramStack.pop_back();
				rax = (p1 < 0) ? 1 : 0;
			}
			else if(func == "basic_add") {
				int p1 = paramStack.back();
				paramStack.pop_back();
				
				int p2 = paramStack.back();
				paramStack.pop_back();

				rax = p1 + p2;
			}
			else if(func == "basic_sub") {
				int p1 = paramStack.back();
				paramStack.pop_back();
				
				int p2 = paramStack.back();
				paramStack.pop_back();

				rax = p1 - p2;
			}
			else if(func == "basic_xor") {
				int p1 = paramStack.back();
				paramStack.pop_back();
				
				int p2 = paramStack.back();
				paramStack.pop_back();

				rax = p1 ^ p2;
			}
			else if(func == "basic_eq") {
				int p1 = paramStack.back();
				paramStack.pop_back();
				
				int p2 = paramStack.back();
				paramStack.pop_back();

				rax = (p1 == p2) ? 1 : 0;
			}
			else if(func == "basic_lt") {
				int p1 = paramStack.back();
				paramStack.pop_back();
				
				int p2 = paramStack.back();
				paramStack.pop_back();

				rax = (p1 < p2) ? 1 : 0;
			}
			else if(func == "basic_neg") {
				int p1 = paramStack.back();
				paramStack.pop_back();
				rax = -p1;
			}
			else if(func == "getMemBitIndex") {
				rax = bitIndex;
			}
			else if(func == "setMemBitIndex") {
				int p1 = paramStack.back();
				paramStack.pop_back();
				bitIndex = p1;
			}
			else if(func == "moveMemHeadRight") {
				bitHead++;
				if(bitHead == bits.size()) {
					bits.push_back(2);
				}
			}
			else if(func == "moveMemHeadLeft") {
				bitHead--;
			}
			else if(func == "setMemBitZero") {
				bits[bitHead] = 0;
			}
			else if(func == "setMemBitOne") {
				bits[bitHead] = 1;
			}
			else if(func == "setMemBitBlank") {
				bits[bitHead] = 2;
			}
			else if(func == "memBitIsZero") {
				rax = (bits[bitHead] == 0) ? 1 : 0;
			}
			else if(func == "memBitIsOne") {
				rax = (bits[bitHead] == 1) ? 1 : 0;
			}
			else if(func == "memBitIsBlank") {
				rax = (bits[bitHead] == 2) ? 1 : 0;
			}
			else if(func == "nextInt") {
				std::cout << "Please input a number: ";
				int val;
				std::cin >> val;
				rax = val;
				std::cout << "You chose: " << val << std::endl;
			}
			else if(func == "printSpace") {
				std::cout << std::endl;
			}
			else if(func == "printInt") {
				int p1 = paramStack.back();
				paramStack.pop_back();
				std::cout << "printInt: " << p1 << std::endl;
			}

			// must pop off stack frames, since these functions implicitly have a return
			for(size_t i = 0; i < varTapes.size(); ++i) {
				varTapes[i].pop_back();
			}

		}
		else if(words[0] == "call") {
			// must be an integer!
			//std::cout << "Must be an integer? check : " << words[1] << std::endl;
			int lineNum = std::stoi(words[1]);

			// push stack frames to varTapes
			for(size_t i = 0; i < varTapes.size(); ++i) {
				varTapes[i].push_back(0);
			}

			// store ip
			ipStack.push_back(ip);

			// get new ip
			ip = lineNum;
			// don't increment ip
			continue;
		}
		else if(words[0] == "jf") {
			int val = 0;
			if(words[1].substr(0, 10) == "!TAPE_tape") {
				int t = getTapeNum(words[1]);
				val = varTapes[t].back();
			}
			else if(words[1] == "!TAPE_RAX") {
				val = rax;
			}

			//std::cout << "jf to " << words[2] << std::endl;
			int dest = std::stoi(words[2]);

			if(val == 0) {
				ip = dest;
				continue;
			}
		}
		else if(words[0] == "return") {
			for(size_t i = 0; i < varTapes.size(); ++i) {
				varTapes[i].pop_back();
			}

			// pop ipStack last into ip
			ip = ipStack.back();
			ipStack.pop_back();
			// must increment ip, so goes down til ip++
		}
		else if(words[1] == "!") {
			// arg1 ! = dest ;
			int t = getTapeNum(words[0]);
			int val = varTapes[t].back();

			t = getTapeNum(words[3]);
			size_t s = varTapes[t].size();
			varTapes[t][s - 1] = 1 - val;
		}
		else if(words[2] == "&&") {
			// arg1 arg2 && dest ;
			int t1 = getTapeNum(words[0]);
			int t2 = getTapeNum(words[1]);
			int val = t1 && t2;

			int t3 = getTapeNum(words[3]);
			size_t s = varTapes[t3].size();
			
			varTapes[t3][s - 1] = val;
		}
		else if(words[2] == "||") {
			// arg1 arg2 || dest ;
			int t1 = getTapeNum(words[0]);
			int t2 = getTapeNum(words[1]);
			int val = t1 || t2;

			int t3 = getTapeNum(words[3]);
			size_t s = varTapes[t3].size();
			
			varTapes[t3][s - 1] = val;
		}
		else if(words[words.size() - 2] == "=") {
			int p2 = 0;
			if(words[1].substr(0, 10) == "!TAPE_tape") {
				int t = getTapeNum(words[1]);
				p2 = varTapes[t].back();
			}
			else if(words[1] == "!TAPE_RAX") {
				p2 = rax;
			}
			else if(words[1] == "true") {
				p2 = 1;
			}
			else if(words[1] == "false") {
				p2 = 0;
			}
			else {
				//std::cout << "assignment... num ? " << words[1] << std::endl;
				p2 = std::stoi(words[1]);
			}

			if(words[0].substr(0, 10) == "!TAPE_tape") {
				int t = getTapeNum(words[0]);
				//std::cout << "yea t here = " << t << " with tapes size = " << std::endl;
				varTapes[t][varTapes[t].size() - 1] = p2;
			}
			else if(words[0] == "!TAPE_RAX") {
				rax = p2;
			}
		}
		else {
			std::cout << "Dunno what line this is..." << program[ip] << std::endl;
			break;
		}

		ip++;
	}

	std::cout << "NumTapes = " << numTapes << std::endl;
	std::cout << "ip = " << ip << std::endl;
	std::cout << "ipStack: ";
	for(size_t i = 0; i < ipStack.size(); ++i) {
		std::cout << ipStack[i] << " ";
	}
	std::cout << std::endl;

	std::cout << "bitIndex = " << bitIndex << std::endl;
	std::cout << "bitHead = " << bitHead << std::endl;
	std::cout << "bits: ";
	for(size_t i = 0; i < bits.size(); ++i) {
		if(bits[i] == 2) {
			std::cout << "_";
		}
		else {
			std::cout << bits[i];
		}
	}
	std::cout << std::endl;

	std::cout << "paramStack: ";
	for(size_t i = 0; i < paramStack.size(); ++i) {
		std::cout << paramStack[i] << " ";
	}
	std::cout << std::endl;

	std::cout << "rax = " << rax << std::endl;
	std::cout << "numSteps = " << numSteps << std::endl;
}


int main() {
	std::string fileName = "assembly.txt";
	std::ifstream file(fileName);

	std::vector<std::string> assembly;
	std::string line;
	while(std::getline(file, line)) {
		assembly.push_back(line);
	}

	simulateAssembly(assembly);	
	std::cout << "DONE" << std::endl;

	return 0;
}

// idea: return / instruction pointer is wrong.
// Need a stack. when 
// 50. call 18
// need to have instead
// 49. push 51 to ip stack
// 50. call 18
// 51. nop
//
//
//
// 21. return   -> means , A) pop off all variable stack frame, then 
// B) pop off the last thing in the ip stack (the 51) into actual instruction pointer
//
// the only actual change is in func pushAndPop: add the statement 
// before + after the call statement... ? but what about those funcLines...
//
// also don't forget to implement memget/memset
//
//
// or how about this:
// we leave assembly as is . call means, A) push new variable stack frames,
// B) store IP in an ip stack, then set the IP to whatever the # was for call #. 
// Then return just means restore IP by yea popping off ip stack.
// All implicit, rather than explicit.
//
// And TM first has to push -1 into IP stack; 0 into actual IP
//
// also: pushing into IP also means pushing into its sideways tapes too
//
// also: equality might be a little trickier...... consider 11 vs 110 vs 11000000...leading zeros :\
// don't forget: when handling like printInt(int x), must implicitly pop off x from params stack
// as well as implicitly pop off var stack frames
