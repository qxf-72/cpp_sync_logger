# cpp_sync_logger

![GitHub License](https://img.shields.io/github/license/qxf-72/cpp_sync_logger)
![GitHub top language](https://img.shields.io/github/languages/top/qxf-72/cpp_sync_logger)

一个基于 C++11 实现的轻量级多线程同步日志库。

本项目用于学习和实践 Linux C++ 工程化开发中的日志系统设计，支持日志级别过滤、控制台输出、文件输出、时间戳、线程 ID、源码文件名与行号输出，并通过互斥锁保证多线程环境下日志写入的线程安全。

## 项目特点

* 支持 `DEBUG` / `INFO` / `WARN` / `ERROR` / `FATAL` 五种日志级别
* 支持日志级别过滤
* 支持输出到控制台
* 支持输出到日志文件
* 支持控制台输出和文件输出同时开启
* 支持时间戳，精确到毫秒
* 支持输出线程 ID、源码文件名和行号
* 支持宏调用方式，例如 `LOG_INFO("message")`
* 使用 `std::mutex` 保证多线程写日志时的线程安全
* 基于 CMake 构建，项目结构清晰，便于后续扩展

## 项目结构

```text
cpp_sync_logger/
├── CMakeLists.txt
├── include/
│   └── Logger.h
├── src/
│   └── Logger.cpp
└── test/
    └── main.cpp
```

## 构建方式

本项目使用 CMake 构建。

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

运行示例程序：

```bash
./build/sync_logger_demo
```

如果需要重新查看干净的日志输出，可以先删除旧日志文件：

```bash
rm -f test.log
./build/sync_logger_demo
```

## 使用示例

### 1. 输出到控制台

```cpp
#include "Logger.h"

int main() {
  Logger::instance().setLevel(LogLevel::DEBUG);
  Logger::instance().setConsoleOutput(true);

  LOG_DEBUG("this is a debug message.");
  LOG_INFO("this is an info message.");
  LOG_WARN("this is a warning message.");
  LOG_ERROR("this is an error message.");
  LOG_FATAL("this is a fatal message.");

  return 0;
}
```

示例输出：

```text
[2026-06-06 15:30:21.123][INFO][140391289259840][main.cpp:8] this is an info message.
```

### 2. 输出到文件

```cpp
#include "Logger.h"

int main() {
  if (!Logger::instance().init("test.log", LogLevel::DEBUG)) {
    return -1;
  }

  Logger::instance().setConsoleOutput(false);

  LOG_INFO("this message will be written to file.");

  Logger::instance().closeFile();

  return 0;
}
```

运行后，日志会被写入 `test.log`。

### 3. 同时输出到控制台和文件

```cpp
#include "Logger.h"

int main() {
  if (!Logger::instance().init("test.log", LogLevel::DEBUG)) {
    return -1;
  }

  Logger::instance().setConsoleOutput(true);

  LOG_INFO("this message will be written to console and file.");

  Logger::instance().closeFile();

  return 0;
}
```

## 多线程示例

```cpp
#include "Logger.h"

#include <string>
#include <thread>
#include <vector>

int main() {
  if (!Logger::instance().init("test.log", LogLevel::DEBUG)) {
    return -1;
  }

  Logger::instance().setConsoleOutput(false);

  std::vector<std::thread> threads;

  for (int i = 0; i < 5; ++i) {
    threads.emplace_back([i]() {
      for (int j = 0; j < 5; ++j) {
        LOG_INFO("Thread " + std::to_string(i) + " logging message " + std::to_string(j));
      }
    });
  }

  for (auto& thread : threads) {
    thread.join();
  }

  Logger::instance().closeFile();

  return 0;
}
```

该示例会创建多个线程并发写日志。日志写入过程由互斥锁保护，避免多个线程同时写入时出现日志内容交错的问题。

## 日志级别

日志级别从低到高依次为：

```text
DEBUG < INFO < WARN < ERROR < FATAL
```

当设置最低输出日志级别后，低于该级别的日志不会被输出。

例如：

```cpp
Logger::instance().setLevel(LogLevel::WARN);
```

此时只会输出：

```text
WARN
ERROR
FATAL
```

而 `DEBUG` 和 `INFO` 日志会被过滤。

## 核心设计

本项目采用同步日志模型。

业务线程调用日志接口时，会直接完成日志级别判断、日志格式化和日志写入操作。为了保证多个线程同时写日志时不会发生输出交错，日志写入过程使用 `std::mutex` 进行保护。

基本流程如下：

```text
业务线程调用日志宏
        |
        v
传入日志级别、文件名、行号和日志内容
        |
        v
判断日志级别是否需要输出
        |
        v
生成时间戳、线程 ID、文件名、行号等信息
        |
        v
加锁写入控制台或日志文件
        |
        v
       解锁
```

同步日志库的优点是实现简单、逻辑清晰、可靠性较高，适合作为学习日志系统基本结构的入门项目。

同步日志库的缺点是业务线程需要直接承担日志写入开销。当日志量较大或磁盘 IO 较慢时，可能影响业务线程的执行效率。因此，本项目也可以作为后续实现异步日志系统的基础版本。

## 主要接口

```cpp
static Logger& instance();

bool init(const std::string& filename, LogLevel level = LogLevel::DEBUG);

void setLevel(LogLevel level);

void setConsoleOutput(bool enabled);

void log(LogLevel level, const char* file, int line, const std::string& message);

void closeFile();
```

常用宏：

```cpp
LOG_DEBUG("debug message");
LOG_INFO("info message");
LOG_WARN("warn message");
LOG_ERROR("error message");
LOG_FATAL("fatal message");
```

## 说明

`FATAL` 当前只是一个日志级别，不会自动终止程序。

日志文件默认以追加模式打开，多次运行程序时，新的日志会追加到旧日志后面。

如果只需要输出到控制台，可以不调用 `init()`；如果需要输出到文件，则需要先调用 `init()` 打开日志文件。

## 后续计划

* 支持日志文件按大小滚动
* 支持日志文件按日期滚动
* 支持自定义日志格式
* 支持异步写日志
* 支持配置文件初始化日志系统
* 增加更完整的单元测试

## 项目定位

本项目主要用于练习以下 C++ 工程能力：

* C++11 标准库使用
* 单例模式
* RAII 思想
* 文件 IO
* 多线程互斥锁
* 日志级别过滤
* 宏封装
* CMake 项目组织
* Linux C++ 工程化开发

## License

MIT License
