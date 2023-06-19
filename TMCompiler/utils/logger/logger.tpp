#include <iostream>

/**
 * Output operator, like std::cout's.
 * @param val: any value that std::cout's << operator takes in
 * @return Logger object again, so chaining << is possible
 */
template <typename T>
auto Logger::operator<<(T val) -> Logger& {
	// do not log messages beneath the desired level
	if(desired_output_level > Logger::get_importance(message_level)) {
		return *this;
	}

	std::cout << level_mapping.at(message_level).color_info;
	std::cout << val;
	std::cout << reset_color;
	return *this;
}
