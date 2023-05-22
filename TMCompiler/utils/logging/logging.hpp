#ifndef LOGGING_HPP
#define LOGGING_HPP

#include <string>

#define LOG(LEVEL, MESSAGE) \
	Logger::log(LEVEL, MESSAGE, __FILE__, __LINE__, __TIME__);

namespace Logger {
auto log(const std::string& level, const std::string& message,
		 const char* file_name, int line_number, const char* time) -> void;
auto set_level(const std::string& level) -> void;

}  // namespace Logger
#endif
