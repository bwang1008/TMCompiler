#include "logger.hpp"

/**
 * Implementation file of logger.hpp, which specifies a logging system
 * for use in development and debugging in lieu of print statements.
 */

#include <iomanip>	 // std::setw, std::left, std::right
#include <iostream>	 // std::cout, std::endl
#include <map>		 // std::map
#include <string>	 // std::string

namespace Logger {

/**
 * Information associated with each logging level.
 * pretty_name: name of the level, like "WARNING"
 * importance: integer of how important. Higher values are more important.
 * color_info: terminal codes for printing out color
 */
struct LevelInfo {
	std::string pretty_name;
	int importance;
	std::string color_info;
};

/**
 * List out what logging levels are allowed, as well as each level of
 * importance and color information for printing
 */
const std::map<std::string, LevelInfo> level_mapping{
	{"DEBUG", LevelInfo{"DEBUG", 10, "\033[34m"}},
	{"INFO", LevelInfo{"INFO", 20, ""}},
	{"WARNING", LevelInfo{"WARNING", 30, "\033[33m"}},
	{"ERROR", LevelInfo{"ERROR", 40, "\033[1;31m"}},
	{"CRITICAL", LevelInfo{"CRITICAL", 50, "\033[1;31;46m"}},
	{"NONE", LevelInfo{"NONE", 1000, ""}},
};

/**
 * Set the default logging level at INFO.
 */
int current_level = level_mapping.at("INFO").importance;

/**
 * Getter function for retrieving the relative importance of
 * a given logging level.
 *
 * @param level_name: one of DEBUG, INFO, WARNING, ERROR, CRITICAL, NONE
 * @return integer of relative importance of level
 */
auto get_importance(const std::string& level_name) -> int {
	return level_mapping.at(level_name).importance;
}

/**
 * Setter function for the current logging level. Setting "WARNING"
 * will show all "WARNING", "ERROR", and "CRITICAL" logs.
 *
 * @param level_name: one of DEBUG, INFO, WARNING, ERROR, CRITICAL, NONE
 * @return void
 */
auto set_level(const std::string& level_name) -> void {
	current_level = get_importance(level_name);
}

/**
 * Prints out formatted custom message at specified logging level.
 *
 * @param level: one of DEBUG, INFO, WARNING, ERROR, CRITICAL
 * @param message: custom string to be printed
 * @param file_name: name of the file in which logging macro appears in
 * @param line_number: line number in file of logging call
 * @param time: time of execution of logging in format hh:mm:ss
 * @return void
 */
auto log(const std::string& level, const std::string& message,
		 const char* file_name, const int line_number, const char* time)
	-> void {
	// ignore low-level logs
	if(current_level > get_importance(level)) {
		return;
	}

	// widths for each section to maintain constant beginning width of each log
	const std::size_t max_level_name_size = 8;
	const std::size_t max_file_name_size = 15;
	const std::size_t max_line_number_size = 5;

	// output only the file_name, without parent directories
	const std::string nice_file_name = std::string(file_name);
	const std::size_t last_slash = nice_file_name.rfind('/');
	const std::string truncated_file_name =
		(last_slash == std::string::npos)
			? nice_file_name
			: nice_file_name.substr(1 + last_slash);

	// terminal code for resetting color/formatting to normal
	const std::string reset{"\033[0m"};

	// print message with color and formatting
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
