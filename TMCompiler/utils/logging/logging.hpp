#ifndef LOGGING_HPP
#define LOGGING_HPP

#include <iostream>
#include <string>

namespace Logger {
auto log(const std::string& level, const std::string& message) -> void;
auto set_level(const std::string& level) -> void;

auto debug(const std::string& message) -> void;
auto info(const std::string& message) -> void;
auto warning(const std::string& message) -> void;
auto error(const std::string& message) -> void;
auto critical(const std::string& message) -> void;
}  // namespace Logger
#endif
