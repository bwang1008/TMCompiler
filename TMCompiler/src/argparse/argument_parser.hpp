#ifndef ARGPARSE_HPP
#define ARGPARSE_HPP

#include <ifstream>
#include <string>

#include "TMCompiler/src/argparse/argument.hpp"

class ArgumentParser {
public:
	ArgumentParser();
	ArgumentParser(const std::ifstream& argumentsFile);
	void addArgument(const Argument argument);
	void parseCommandLine(const int argc, const char** argv);
	Argument get(std::string name);
}

#endif
