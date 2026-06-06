#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include "Logger.h"

int main() {
  std::string filename = "test.log";
  if (!Logger::instance().init(filename)) {
    return -1;
  }

  Logger::instance().setLevel(LogLevel::DEBUG);

  LOG_INFO("Logger initialized successfully.");
  LOG_DEBUG("This is a debug message.");
  LOG_WARN("This is a warning message.");
  LOG_ERROR("This is an error message.");
  LOG_FATAL("This is a fatal message.");

  std::vector<std::thread> threads;
  for (int i = 0; i < 5; ++i) {
    threads.emplace_back([i]() {
      for (int j = 0; j < 10; ++j) {
        LOG_DEBUG("Thread " + std::to_string(i) + " logging message " + std::to_string(j));
      }
    });
  }
  for (auto& t : threads) {
    t.join();
  }

  return 0;
}
