#include "Logger.h"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <thread>

Logger& Logger::instance() {
  static Logger instance;
  return instance;
}

Logger::~Logger() {
  stop();
}

bool Logger::init(const std::string& filename, LogLevel level) {
  std::lock_guard<std::mutex> lock(mtx_);

  if (out_.is_open()) {
    out_.close();
  }

  out_.clear();
  out_.open(filename, std::ios::out | std::ios::app);
  if (!out_.is_open()) {
    std::cerr << "Fail to open log file: " << filename << std::endl;
    return false;
  }

  minLogLevel = level;
  return true;
}

void Logger::setLevel(LogLevel level) {
  std::lock_guard<std::mutex> lock(mtx_);
  minLogLevel = level;
}

void Logger::log(LogLevel level, const char* file, int line, const std::string& message) {
  std::lock_guard<std::mutex> lock(mtx_);
  if (level < minLogLevel)
    return;
  std::string formattedMessage = formatMessage(level, file, line, message);
  if (out_.is_open()) {
    out_ << formattedMessage << std::endl;
  } else {
    std::cerr << "Log file is not open!" << std::endl;
  }
}

void Logger::stop() {
  std::lock_guard<std::mutex> lock(mtx_);
  if (out_.is_open()) {
    out_.close();
  }
}

std::string Logger::formatMessage(LogLevel level, const char* file, int line,
                                  const std::string& message) {
  std::ostringstream oss;
  oss << "[" << currentTime() << "]"
      << "[" << levelToString(level) << "]"
      << "[" << std::this_thread::get_id() << "]"
      << "[" << file << ":" << line << "] " << message;
  return oss.str();
}

std::string Logger::levelToString(LogLevel level) {
  switch (level) {
    case LogLevel::DEBUG:
      return "DEBUG";
    case LogLevel::INFO:
      return "INFO";
    case LogLevel::WARN:
      return "WARN";
    case LogLevel::ERROR:
      return "ERROR";
    case LogLevel::FATAL:
      return "FATAL";
    default:
      return "UNKNOWN";
  }
}

std::string Logger::currentTime() {
  using namespace std::chrono;

  auto now = system_clock::now();
  auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;

  std::time_t t = system_clock::to_time_t(now);

  std::tm tm_time;
  localtime_r(&t, &tm_time);

  std::ostringstream oss;
  oss << std::put_time(&tm_time, "%Y-%m-%d %H:%M:%S") << "." << std::setfill('0') << std::setw(3)
      << ms.count();

  return oss.str();
}
