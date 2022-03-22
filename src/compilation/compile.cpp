// takes in a file path for higher-level code and turns into a TM file
#include "unit1.h"
#include "unit2.h"
#include "../tm_definition/multi_tape_turing_machine.h"

#include <iostream>		// std::cout, std::endl	
#include <fstream>		// std::ifstream, std::ofstream
#include <string>		// std::string
#include <vector>		// std::vector	

int checkCompilation(std::string &fileName) {
	int periodIndex = fileName.size() - 1;
	while(0 <= periodIndex && fileName[periodIndex] != '.') {
		--periodIndex;
	}

	std::string prefix = "";

	if(periodIndex == -1) {
		prefix = fileName;
	}
	else {
		prefix = fileName.substr(0, periodIndex);
	}

	std::string tempFileName = prefix + "_temp.cpp";
	
	std::ofstream file;
	file.open(tempFileName);
	file << "int MEM[5];" << std::endl;
	file << "int nextInt();" << std::endl;
	file << "void printInt(int x);" << std::endl;
	file << "void printSpace();" << std::endl;

	std::ifstream origFile;
	origFile.open(fileName);
	std::string line;
	while(std::getline(origFile, line)) {
		file << line << std::endl;
	}

	origFile.close();
	file.close();

	std::string command = std::string("g++ -fsyntax-only ") + tempFileName;
	std::cout << "Execute: " << command << std::endl;
	int status = std::system(command.c_str());

	return status;
}

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
