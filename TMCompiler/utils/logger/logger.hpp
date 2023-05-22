#ifndef LOGGER_HPP
#define LOGGER_HPP

/**
 * Logging system for use in development and debugging.
 *
 * Use the macro LOG in source code to output a color-coded custom message.
 * There are 5 levels of logging, following Python's logging module, in
 * increasing importance: DEBUG, INFO, WARNING, ERROR, CRITICAL.
 *
 * Set the current level of logging with set_level(new_level).
 */

#include <string>  // std::string

/**
 * Macro wrapper that calls Logger::log, along with additional variables:
 * current file, the line number, and time where the logging happens.
 *
 * @param level: one of DEBUG, INFO, WARNING, ERROR, CRITICAL
 * @param message: custom string to be printed out
 * @return void
 *
 * Example usage:
 * Logger::set_level("DEBUG");
 * LOG("INFO", "BEGIN");
 * LOG("DEBUG", "this is debug");
 * LOG("WARNING", "this is warning");
 * LOG("ERROR", "this is error");
 * LOG("CRITICAL", "this is critical");
 * LOG("INFO", "END");
 *
 * Example output:
 * [22:54:10 INFO         logging.cpp:84   ] BEGIN
 * [22:54:10 WARNING      logging.cpp:86   ] this is warning
 * [22:54:10 ERROR        logging.cpp:87   ] this is error
 * [22:54:10 CRITICAL     logging.cpp:88   ] this is critical
 * [22:54:10 INFO         logging.cpp:89   ] END
 */
#define LOG(level, message) \
	Logger::log(level, message, __FILE__, __LINE__, __TIME__);

namespace Logger {

/**
 * Prints out custom message at specified logging level.
 * Do not use this directly; instead use the macro LOG above.
 *
 * @param level: one of DEBUG, INFO, WARNING, ERROR, CRITICAL
 * @param message: custom string to be printed
 * @param file_name: name of the file in which logging macro appears in
 * @param line_number: line number in file of logging call
 * @param time: time of execution of logging in format hh:mm:ss
 * @return void
 */
auto log(const std::string& level, const std::string& message,
		 const char* file_name, int line_number, const char* time) -> void;

/**
 * Sets the current logging level. Logging calls that are below the
 * current logging level are ignored. For instance, if the current logging
 * level is WARNING, then logging calls to DEBUG and INFO are ignored. If
 * NONE is specified, all logging calls are ignored.
 *
 * @param level: one of DEBUG, INFO, WARNING, ERROR, CRITICAL, NONE
 * @return void
 */
auto set_level(const std::string& level) -> void;

}  // namespace Logger
#endif
