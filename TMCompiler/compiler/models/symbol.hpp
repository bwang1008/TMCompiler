#ifndef SYMBOL
#define SYMBOL

#include <string>	// std::string

// represent <abc> and "abc" in BNF file
struct Symbol {
	std::string value;
	bool terminal{false};
};

#endif
