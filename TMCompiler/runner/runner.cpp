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
	const MultiTapeTuringMachine mttm = compile(fileName);

	std::cout << "Compile successful" << std::endl;

	mttm.displayProfile();
	

	return 0;
}
