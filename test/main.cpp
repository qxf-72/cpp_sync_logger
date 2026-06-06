#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include "Logger.h"

int main() {
  if (!Logger::instance().init("test.log")) {
    return -1;
  }
  Logger::instance().setLevel(LogLevel::DEBUG);

  LOG_DEBUG("this is a debug message.");
  LOG_INFO("this is an info message.");
  LOG_WARN("this is a warning message.");
  LOG_ERROR("this is an error message.");
  LOG_FATAL("this is a fatal message.");

  std::vector<std::thread> threads;
  for (int i = 0; i < 5; ++i) {
    threads.emplace_back([i]() {
      for (int j = 0; j < 10; ++j) {
        LOG_INFO("Thread " + std::to_string(i) + " logging message " + std::to_string(j));
      }
    });
  }

  for (auto& thread : threads) {
    thread.join();
  }

  return 0;
}
