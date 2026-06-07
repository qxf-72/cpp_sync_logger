#include "Logger.h"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <thread>

// 获取全局唯一的 Logger 实例
Logger& Logger::instance() {
  static Logger instance;
  return instance;
}

// 析构函数关闭文件
// 这里不必加锁，因为closeFile中加了锁
Logger::~Logger() {
  closeFile();
}

// 初始化日志系统
// filename: 日志文件名
// minLevel: 最低输出日志级别
bool Logger::init(const std::string& filename, LogLevel level) {
  std::lock_guard<std::mutex> lock(mtx_);

  if (out_.is_open()) {
    out_.close();
  }
  out_.flush();
  out_.clear();
  out_.open(filename, std::ios::out | std::ios::app);
  if (!out_.is_open()) {
    std::cerr << "Fail to open log file: " << filename << std::endl;
    return false;
  }

  minLogLevel_ = level;
  return true;
}

// 动态设置日志级别
void Logger::setLevel(LogLevel level) {
  std::lock_guard<std::mutex> lock(mtx_);
  minLogLevel_ = level;
}

// 动态设置是否输出到控制台
void Logger::setConsoleOutput(bool enabled) {
  std::lock_guard<std::mutex> lock(mtx_);
  consoleOutput_ = enabled;
}

// 写日志接口
void Logger::log(LogLevel level, const char* file, int line, const std::string& message) {
  std::lock_guard<std::mutex> lock(mtx_);
  if (level < minLogLevel_) {
    return;
  }

  std::string formattedMessage = formatMessage(level, file, line, message);

  // 输出到控制台
  if (consoleOutput_) {
    std::cout << formattedMessage << '\n';
  }

  // 输出到文件
  if (out_.is_open()) {
    out_ << formattedMessage << '\n';
  }
}

void Logger::closeFile() {
  std::lock_guard<std::mutex> lock(mtx_);
  if (out_.is_open()) {
    out_.flush();
    out_.close();
  }
}

// 格式化一条日志消息
// 把日志级别、时间、线程 ID、文件名、行号和正文拼接成一个字符串
std::string Logger::formatMessage(LogLevel level, const char* file, int line,
                                  const std::string& message) {
  std::ostringstream oss;
  oss << "[" << currentTime() << "]"
      << "[" << levelToString(level) << "]"
      << "[" << std::this_thread::get_id() << "]"
      << "[" << file << ":" << line << "] " << message;
  return oss.str();
}

std::string Logger::levelToString(LogLevel level) const {
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

std::string Logger::currentTime() const {
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
