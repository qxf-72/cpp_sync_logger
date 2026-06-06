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

// 析构函数中调用 stop()
// 目的是保证程序退出时，后台日志线程能够正常结束，剩余日志能够写入文件
Logger::~Logger() {
  stop();
}

// 初始化日志系统
// filename: 日志文件名
// minLevel: 最低输出日志级别
bool Logger::init(std::string& filename, LogLevel minLevel) {
  std::lock_guard<std::mutex> lock(initMutex_);
  out_.open(filename, std::ios::out | std::ios::app);
  if (!out_.is_open()) {
    std::cerr << "Failed to open log file: " << filename << std::endl;
    return false;
  }
  return true;
}

// 动态设置日志级别
void Logger::setLevel(LogLevel level) {
  std::lock_guard<std::mutex> lock(initMutex_);
  minLevel_ = level;
}

// 写日志接口
// 业务线程调用 LOG_INFO / LOG_WARN 等宏后，最终会进入这个函数
void Logger::log(LogLevel level, const char* file, int line, const std::string& message) {
  if (level < minLevel_) {
    return;
  }
  std::string formatted = formatMessage(level, file, line, message);
  {
    std::lock_guard<std::mutex> lock(initMutex_);
    if (out_.is_open()) {
      out_ << formatted << std::endl;
    } else {
      std::cerr << "Log file is not open. Message: " << formatted << std::endl;
    }
  }
}

// 停止日志系统
void Logger::stop() {
  std::lock_guard<std::mutex> lock(initMutex_);
  if (out_.is_open()) {
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
      << "[tid:" << std::this_thread::get_id() << "]"
      << "[" << file << ":" << line << "] " << message;

  return oss.str();
}

// 将日志级别枚举转换成字符串
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

// 获取当前时间字符串
// 格式示例：2026-05-25 12:00:00.123
std::string Logger::currentTime() const {
  auto now = std::chrono::system_clock::now();
  auto time = std::chrono::system_clock::to_time_t(now);
  auto milliseconds =
      std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
  std::tm tmTime{};
  localtime_r(&time, &tmTime);

  // 格式化年月日时分秒
  char buffer[32];
  std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &tmTime);

  // 拼接毫秒部分
  std::ostringstream oss;
  oss << buffer << "." << std::setw(3) << std::setfill('0') << milliseconds.count();

  return oss.str();
}
