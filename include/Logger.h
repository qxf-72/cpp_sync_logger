#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>
#include <mutex>
#include <string>
#include <thread>

enum class LogLevel { DEBUG = 0, INFO, WARN, ERROR, FATAL };

class Logger {
 public:
  static Logger& instance();
  bool init(const std::string& filename, LogLevel level = LogLevel::DEBUG);
  void setLevel(LogLevel level);
  void setConsoleOutput(bool enabled);
  void log(LogLevel level, const char* file, int line, const std::string& message);
  void closeFile();

  Logger(const Logger&) = delete;
  Logger& operator=(const Logger&) = delete;

 private:
  Logger() = default;
  ~Logger();

  std::string formatMessage(LogLevel level, const char* file, int line, const std::string& message);
  std::string levelToString(LogLevel level) const;
  std::string currentTime() const;

 private:
  std::ofstream out_;
  std::mutex mtx_;
  LogLevel minLogLevel_{LogLevel::DEBUG};
  bool consoleOutput_{true};
};

#define LOG_DEBUG(msg) Logger::instance().log(LogLevel::DEBUG, __FILE__, __LINE__, (msg))
#define LOG_INFO(msg) Logger::instance().log(LogLevel::INFO, __FILE__, __LINE__, (msg))
#define LOG_WARN(msg) Logger::instance().log(LogLevel::WARN, __FILE__, __LINE__, (msg))
#define LOG_ERROR(msg) Logger::instance().log(LogLevel::ERROR, __FILE__, __LINE__, (msg))
#define LOG_FATAL(msg) Logger::instance().log(LogLevel::FATAL, __FILE__, __LINE__, (msg))

#endif  // LOGGER_H
