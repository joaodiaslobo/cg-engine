#include "Logger.hpp"

#include <iostream>

using namespace debug;

/**
 * @brief Logs a message with a specified log level.
 *
 * This function outputs a message to the standard output or error stream
 * based on the provided log level. The message is prefixed with a label
 * indicating the log level.
 *
 * @param message The message to be logged.
 */
void Logger::log(LogLevel level, const std::string& message) {
  switch (level) {
    case LogLevel::print:
      std::cout << message << std::endl;
      break;
    case LogLevel::debug:
      std::cout << "[DEBUG] " << message << std::endl;
      break;
    case LogLevel::info:
      std::cout << "[INFO] " << message << std::endl;
      break;
    case LogLevel::warning:
      std::cout << "[WARNING] " << message << std::endl;
      break;
    case LogLevel::error:
      std::cerr << "[ERROR] " << message << std::endl;
      break;
  }
}

void Logger::error(const std::string& message) {
  log(LogLevel::error, message);
}

void Logger::info(const std::string& message) { log(LogLevel::info, message); }