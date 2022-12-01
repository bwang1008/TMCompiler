#ifndef ARGUMENT_HPP
#define ARGUMENT_HPP

#include <list>
#include <string>

struct Argument {
	std::list<std::string> aliases;
	std::string description;
	bool mandatory;
	bool hasValue;
	std::string value;
	std::string defaultValue;
};

#endif
