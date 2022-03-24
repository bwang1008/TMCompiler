// takes in a file path for higher-level code and turns into a TM file
#include "unit1.h"
#include "unit2.h"
#include "utils.h"
#include "../tm_definition/multi_tape_turing_machine.h"

#include <iostream>		// std::cout, std::endl	
#include <fstream>		// std::ifstream, std::ofstream
#include <string>		// std::string
#include <vector>		// std::vector	

MultiTapeTuringMachine compile(std::vector<std::string> program) {
	std::vector<std::string> assembly = sourceToAssembly(program);
	MultiTapeTuringMachine mttm = assemblyToMultiTapeTuringMachine(assembly);

	return mttm;
}

int main() {
	std::string fileName = "../programs/example.cpp";
	std::ifstream myFile(fileName);

	if(!myFile.is_open()) {
		std::cout << "Could not open file " << fileName << std::endl;
		return -1;
	}

	int compilationStatus = checkCompilation(fileName);	

	if(compilationStatus != 0) {
		std::cout << "Error compiling source code: " << std::endl;
		return compilationStatus;
	}
	else {
		std::cout << "Compilation OK" << std::endl;
	}

	std::vector<std::string> program;
	std::string programLine;
	while(std::getline(myFile, programLine)) {
		program.push_back(programLine);
	}

	MultiTapeTuringMachine mttm = compile(program);

	return 0;
}
