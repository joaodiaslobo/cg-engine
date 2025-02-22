#include "Logger.hpp"

#include <iostream>

using namespace debug;

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