#include "logger.hpp"

/**
 * Implementation file of logger.hpp, which specifies a logging system
 * for use in development and debugging in lieu of print statements.
 */

#include <chrono>	  // std::chrono
#include <cstddef>	  // std::size_t
#include <ctime>	  // std::ctime
#include <iomanip>	  // std::setw
#include <ios>		  // std::ios, std::ios_base, std::left, std::right
#include <iostream>	  // std::clog, std::endl
#include <ostream>	  // std::ostream
#include <stdexcept>  // std::invalid_argument
#include <string>	  // std::string

// one global logger
Logger logger{};

/**
 * Retrieve the current time as a string in format hh:mm:ss.
 */
auto get_current_time() -> std::string {
	const std::chrono::time_point<std::chrono::system_clock> current_time =
		std::chrono::system_clock::now();
	const std::time_t current_time_2 =
		std::chrono::system_clock::to_time_t(current_time);
	const std::string current_time_textual{std::ctime(&current_time_2)};

	// format like
	// Www Mmm dd hh:mm:ss yyyy\n
	// 0    5    0123456789
	// see https://en.cppreference.com/w/cpp/chrono/c/ctime

	const std::size_t hour_start_location = std::string("Www Mmm dd ").size();
	const std::size_t time_format_width = std::string("HH:MM:SS").size();

	// extract out HH:MM:SS as string
	return current_time_textual.substr(hour_start_location, time_format_width);
}

/**
 * Constructor. Default logging level at INFO.
 */
Logger::Logger()
	: desired_output_level(level_mapping.at("INFO").importance),
	  message_level(std::string("INFO")) {
}

/**
 * Pretty-print start of each log.
 *
 * Example: [15:37:02 WARNING    main.cpp:15  ]
 * @param level: one of DEBUG, INFO, WARNING, ERROR, CRITICAL
 * @param file_name: name of the file in which logging macro appears in
 * @param line_number: line number in file of logging call
 * @return void
 */
auto Logger::log_prefix(const std::string& level,
						const char* file_name,
						int line_number,
						const std::string& func_name) -> void {
	message_level = level;

	// do not log messages beneath the desired level
	if(desired_output_level > Logger::get_importance(message_level)) {
		return;
	}

	// widths for each section to maintain constant beginning width of each log
	const std::size_t max_level_name_size = 8;

	// output only the file_name, without parent directories
	const std::string nice_file_name = std::string(file_name);
	const std::size_t last_slash = nice_file_name.rfind('/');
	const std::string truncated_file_name =
		(last_slash == std::string::npos)
			? nice_file_name
			: nice_file_name.substr(1 + last_slash);

	// print message with color and formatting
	std::clog << reset_color;
	std::clog << "[ ";

	std::clog << time_color;
	std::clog << get_current_time();

	std::clog << reset_color;
	std::clog << " | ";

	std::clog << level_mapping.at(level).color_info;
	std::clog << std::left << std::setw(max_level_name_size)
			  << level_mapping.at(level).pretty_name;

	std::clog << reset_color;
	std::clog << " | ";

	std::clog << file_color;
	std::clog << truncated_file_name;
	std::clog << ":";
	std::clog << func_color;
	std::clog << func_name;
	std::clog << ":";
	std::clog << file_color;
	std::clog << line_number;

	std::clog << reset_color;
	std::clog << "] ";
	std::clog << level_mapping.at(level).color_info;
}

/**
 * Getter function for retrieving the relative importance of
 * a given logging level.
 *
 * @param level_name: one of DEBUG, INFO, WARNING, ERROR, CRITICAL, NONE
 * @return integer of relative importance of level
 */
auto Logger::get_importance(const std::string& level_name) -> int {
	if(level_mapping.find(level_name) == level_mapping.end()) {
		throw std::invalid_argument(std::string("No logging level named ") +
									level_name);
	}
	return level_mapping.at(level_name).importance;
}

/**
 * Setter function for the current logging level. For example, setting "WARNING"
 * will show all "WARNING", "ERROR", and "CRITICAL" logs.
 *
 * @param level_name: one of DEBUG, INFO, WARNING, ERROR, CRITICAL, NONE
 * @return void
 */
auto Logger::set_level(const std::string& level_name) -> void {
	desired_output_level = get_importance(level_name);
}

// special functions to allow something like "logger << std::endl;"
// see
// https://stackoverflow.com/questions/16444119/how-to-write-a-function-wrapper-for-cout-that-allows-for-expressive-syntax
auto Logger::operator<<(std::ostream& (*f)(std::ostream&)) -> Logger& {
	if(desired_output_level <= Logger::get_importance(message_level)) {
		f(std::clog);
	}

	return *this;
}

auto Logger::operator<<(std::ostream& (*f)(std::ios&)) -> Logger& {
	if(desired_output_level <= Logger::get_importance(message_level)) {
		f(std::clog);
	}

	return *this;
}

auto Logger::operator<<(std::ostream& (*f)(std::ios_base&)) -> Logger& {
	if(desired_output_level <= Logger::get_importance(message_level)) {
		f(std::clog);
	}

	return *this;
}
