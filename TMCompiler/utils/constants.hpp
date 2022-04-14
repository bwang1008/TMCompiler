#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

namespace Constants {

	namespace Symbol {
		const char zero = '0';
		const char one = '1';
		const char blank = '_';
	}

	namespace Shift {
		const int left = -1;
		const int none = 0;
		const int right = 1;
	}

	namespace StateStatus {
		const int halted = 1;
		const int ongoing = 0;
	}
}

#endif
