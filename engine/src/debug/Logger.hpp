#pragma once

#include <string>

namespace debug {
enum class LogLevel { print, debug, info, warning, error };

class Logger {
  static void log(LogLevel level, const std::string& message);

 public:
  static void error(const std::string& message);
  static void info(const std::string& message);
};
}  // namespace debug