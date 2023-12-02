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

#include <ios>		// std::ios, std::ios_base
#include <ostream>	// std::ostream
#include <string>	// std::string

/**
 * Macro wrapper that outputs a logging prefix with the
 * current file and the line number where the logging happens.
 *
 * The logging prefix looks like [15:37:02 WARNING    main.cpp:15  ]
 *
 * @param level: one of DEBUG, INFO, WARNING, ERROR, CRITICAL
 * @return void
 *
 * Example usage:
 * logger.set_level("DEBUG");
 * LOG("DEBUG") << "this is debug" << std::endl;
 * LOG("INFO") << "this is info" << std::endl;
 * LOG("WARNING") << "this is warning" << std::endl;
 * LOG("ERROR") << "this is error" << std::endl;
 * LOG("CRITICAL") << "this is critical" << std::endl;
 *
 * Example output:
 * [22:54:10 DEBUG        logging.cpp:86   ] this is debug
 * [22:54:10 INFO         logging.cpp:86   ] this is info
 * [22:54:10 WARNING      logging.cpp:86   ] this is warning
 * [22:54:10 ERROR        logging.cpp:87   ] this is error
 * [22:54:10 CRITICAL     logging.cpp:88   ] this is critical
 */
#define LOG(level)                                                      \
	logger.log_prefix(level,                                            \
					  __FILE__,                                         \
					  __LINE__,                                         \
					  std::string(static_cast<const char*>(__func__))); \
	logger

class Logger {
public:
	/**
	 * Constructor. Default logging level at INFO.
	 */
	Logger();

	/**
	 * Pretty-print start of each log.
	 *
	 * Example: [15:37:02 WARNING    main.cpp:15  ]
	 * @param level: one of DEBUG, INFO, WARNING, ERROR, CRITICAL
	 * @param file_name: name of the file in which logging macro appears in
	 * @param line_number: line number in file of logging call
	 * @return void
	 */
	auto log_prefix(const std::string& level,
					const char* file_name,
					int line_number,
					const std::string& func_name) -> void;

	/**
	 * Output operator, like std::cout's.
	 * @param val: any value that std::cout's << operator takes in
	 * @return Logger object again, so chaining << is possible
	 */
	template <typename T>
	auto operator<<(T val) -> Logger&;

	// see
	// https://stackoverflow.com/questions/16444119/how-to-write-a-function-wrapper-for-cout-that-allows-for-expressive-syntax
	// ex: << takes in a parameter (like std::endl), which is a function pointer
	// f that takes in a parameter std::ios&, and retuns a std::ostream&
	auto operator<<(std::ostream& (*f)(std::ostream&)) -> Logger&;
	auto operator<<(std::ostream& (*f)(std::ios&)) -> Logger&;
	auto operator<<(std::ostream& (*f)(std::ios_base&)) -> Logger&;

	/**
	 * Getter function for retrieving the relative importance of
	 * a given logging level.
	 *
	 * @param level_name: one of DEBUG, INFO, WARNING, ERROR, CRITICAL, NONE
	 * @return integer of relative importance of level
	 */
	static auto get_importance(const std::string& level_name) -> int;

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

private:
	// all log levels below desired are not reported
	int desired_output_level;

	// temporary storage of each log's level
	std::string message_level;
};

// implementation of template functions
#include "logger.tpp"

extern Logger logger;

#endif
