/** 
 * The single runner file that is first called.
 * Given a file name for a source file, compile it into a 
 * MultiTapeTuringMachine, and write it into a JSON file
 */

#include <iostream>		// std::cout, std::endl
#include <fstream>		// std::ofstream

#include "TMCompiler/compilation/compile.hpp"
#include "TMCompiler/tm_definition/multi_tape_turing_machine.hpp"
#include "TMCompiler/utils/json.hpp"

int main(int argc, char* argv[]) {
	if(argc <= 1) {
		std::cout << "Please provide the name of a source file for compilation. Please try again. Ex: ./compile ../programs/example.cpp" << std::endl;
		return -1;
	}

	const std::string fileName(argv[1]);
	size_t lastDot = fileName.size() - 1;
	while(lastDot > 0 && fileName[lastDot] != '.') {
		--lastDot;
	}

	if(lastDot == 0) {
		std::cout << "Provided file has no extension; please try again" << std::endl;
		return -1;
	}

	MultiTapeTuringMachine mttm = compile(fileName);

	std::cout << "Compile successful" << std::endl;
	std::cout << std::endl;

	mttm.displayProfile();
	std::cout << std::endl;

	// write to JSON file
	nlohmann::json j;
	to_json(j, mttm);

	const std::string outFileName = fileName.substr(0, lastDot) + ".json";
	std::ofstream outFile(outFileName);
	outFile << j.dump(4) << std::endl;
	outFile.close();

	std::cout << "Results written to file " << outFileName << std::endl;

	return 0;
}
