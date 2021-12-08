#ifndef CONFIG_H
#define CONFIG_H

namespace Config {
	const int zero = '0';
	const int one = '1';
	const int blank = '_';

	namespace Shift {
		const int left = 0;
		const int right = 2;
	}

	namespace StateStatus {
		const int accept = 1;
		const int reject = -1;
		const int ongoing = 0;
	}
}

#endif
