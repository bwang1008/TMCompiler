/** 
 * The single runner file that is first called.
 * Given a file name for a source file, compile it into a 
 * MultiTapeTuringMachine, and write it into a JSON file
 */

#include <iostream>

#include "TMCompiler/compilation/compile.hpp"
#include "TMCompiler/tm_definition/multi_tape_turing_machine.hpp"

int main(int argc, char* argv[]) {
	if(argc <= 1) {
		std::cout << "Please provide the name of a source file for compilation" << std::endl;
		return -1;
	}

	const std::string fileName(argv[1]);
	MultiTapeTuringMachine mttm = compile(fileName);

	std::cout << "Compile successful" << std::endl;

	mttm.displayProfile();
	
	int simulate = 1;

	if(simulate) {
		std::cout << "Begin simulating:" << std::endl;

		int debug = 0;
		int numSteps = 0;
		int limit = 0;

		while(!mttm.halted() && (limit <= 0 || numSteps < limit)) {
			mttm.step(debug);

			if(debug) {
				std::cout << "After step " << numSteps << std::endl;
				mttm.displayTapes();
			}

			if(numSteps % 10000 == 0) {
				std::cout << "Finished " << numSteps << " steps" << std::endl;
			}

			++numSteps;
		}

		std::cout << "Final: " << std::endl;
		mttm.displayTapes();
		std::cout << numSteps << " steps" << std::endl;

		std::cout << "halted ? " << mttm.halted() << std::endl;
	}

	return 0;
}
