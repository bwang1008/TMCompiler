#include "logging.hpp"

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

auto log(const std::string& level, const std::string& message) -> void {
	if(current_level > get_level(level)) {
		return;
	}

	std::cout << level_mapping.at(level).color_info;
	std::cout << "[" << level_mapping.at(level).pretty_name << ": " << __FILE__
			  << "] ";
	std::cout << message;
	std::cout << "\033[0m";
	std::cout << std::endl;
}

auto debug(const std::string& message) -> void {
	log("DEBUG", message);
}

auto info(const std::string& message) -> void {
	log("INFO", message);
}

auto warning(const std::string& message) -> void {
	log("WARNING", message);
}

auto error(const std::string& message) -> void {
	log("ERROR", message);
}

auto critical(const std::string& message) -> void {
	log("CRITICAL", message);
}
}  // namespace Logger

/*
Example usage:
Logger::set_level("DEBUG");
Logger::log("INFO", "BEGIN");
Logger::log("DEBUG", "this is debug");
Logger::log("WARNING", "this is warning");
Logger::log("ERROR", "this is error");
Logger::log("CRITICAL", "this is critical");
Logger::log("INFO", "END");
*/
