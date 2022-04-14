#include "compile.hpp"

#include <iostream>		// std::cout, std::endl	
#include <fstream>		// std::ifstream
#include <stdexcept>	// std::runtime_error
#include <string>		// std::string
#include <vector>		// std::vector	

#include "TMCompiler/compilation/unit1.hpp"							// assemblyToMultiTapeTuringMachine
#include "TMCompiler/compilation/unit2.hpp"							// sourceToAssembly
#include "TMCompiler/tm_definition/multi_tape_turing_machine.hpp"	// MultiTapeTuringMachine	
#include "TMCompiler/utils/helper.hpp"								// checkCompilation

MultiTapeTuringMachine compile(const std::string &fileName) {
	std::ifstream file(fileName);

	if(!file.is_open()) {
		throw std::runtime_error("Could not open file " + fileName);
	}

	const int compilationStatus = checkCompilation(fileName);	
	if(compilationStatus != 0) {
		throw std::runtime_error("Error compiling source code");
	}
	else {
		std::cout << "Initial compilation OK" << std::endl;
	}

	std::vector<std::string> program;
	std::string programLine;
	while(std::getline(file, programLine)) {
		program.push_back(programLine);
	}

	const std::vector<std::string> assembly = sourceToAssembly(program);
	const MultiTapeTuringMachine mttm = assemblyToMultiTapeTuringMachine(assembly);

	return mttm;
}

