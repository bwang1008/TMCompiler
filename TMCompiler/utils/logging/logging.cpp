#include "logging.hpp"

#include <iomanip>	// setw, left, right
#include <iostream>
#include <map>
#include <string>

namespace Logger {

struct LevelInfo {
	std::string pretty_name;
	int level;
	std::string color_info;
};

int current_level = 1;

const std::map<std::string, LevelInfo> level_mapping{
	{"DEBUG", LevelInfo{"DEBUG   ", 10, "\033[34m"}},
	{"INFO", LevelInfo{"INFO    ", 20, ""}},
	{"WARNING", LevelInfo{"WARNING ", 30, "\033[33m"}},
	{"ERROR", LevelInfo{"ERROR   ", 40, "\033[1;31m"}},
	{"CRITICAL", LevelInfo{"CRITICAL", 50, "\033[1;31;46m"}},
	{"NONE", LevelInfo{"NONE    ", 1000, ""}},
};

auto get_level(const std::string& level_name) -> int {
	return level_mapping.at(level_name).level;
}

auto set_level(const std::string& level_name) -> void {
	current_level = get_level(level_name);
}

auto log(const std::string& level, const std::string& message,
		 const char* file_name, const int line_number, const char* time)
	-> void {
	if(current_level > get_level(level)) {
		return;
	}

	const std::size_t max_level_name_size = 8;
	const std::size_t max_file_name_size = 15;
	const std::size_t max_line_number_size = 5;

	const std::string nice_file_name = std::string(file_name);
	const std::string truncated_file_name =
		nice_file_name.substr(1 + nice_file_name.rfind('/'));
	const std::string reset{"\033[0m"};

	std::cout << "[";
	std::cout << time;
	std::cout << " ";
	std::cout << level_mapping.at(level).color_info;
	std::cout << std::left << std::setw(max_level_name_size)
			  << level_mapping.at(level).pretty_name;
	std::cout << " ";
	std::cout << std::right << std::setw(max_file_name_size)
			  << truncated_file_name;
	std::cout << ":";
	std::cout << std::left << std::setw(max_line_number_size) << line_number;
	std::cout << reset << "] ";
	std::cout << level_mapping.at(level).color_info;
	std::cout << message;
	std::cout << reset;
	std::cout << std::endl;
}

}  // namespace Logger

/*
Example usage:
Logger::set_level("DEBUG");
LOG("INFO", "BEGIN");
LOG("DEBUG", "this is debug");
LOG("WARNING", "this is warning");
LOG("ERROR", "this is error");
LOG("CRITICAL", "this is critical");
LOG("INFO", "END");
*/

auto main() -> int {
	Logger::set_level("DEBUG");
	LOG("INFO", "BEGIN");
	LOG("DEBUG", "this is debug");
	LOG("WARNING", "this is warning");
	LOG("ERROR", "this is error");
	LOG("CRITICAL", "this is critical");
	LOG("INFO", "END");

	return 0;
}
