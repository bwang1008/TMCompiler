#ifndef ARGPARSE_HPP
#define ARGPARSE_HPP

#include <fstream>	// std::ifstream
#include <string>	// std::string

#include "TMCompiler/utils/argparse/argument.hpp"  // Argument

class ArgumentParser {
public:
	ArgumentParser();
	explicit ArgumentParser(const std::ifstream& argumentsFile);
	void addArgument(Argument argument);
	void parseCommandLine(int argc, const char** argv);
	auto get(std::string name) -> Argument;
};

#endif
