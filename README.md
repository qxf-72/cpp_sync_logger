# cpp_sync_logger

![GitHub License](https://img.shields.io/github/license/qxf-72/cpp_sync_logger)
![GitHub top language](https://img.shields.io/github/languages/top/qxf-72/cpp_sync_logger)

一个基于 C++11 实现的轻量级同步日志库。

本项目用于学习和实践 C++ 工程化开发中的日志系统设计，支持日志级别过滤、时间戳、线程 ID、源码文件名与行号输出，并通过互斥锁保证多线程环境下日志写入的线程安全。

## 项目特点

* 支持 `DEBUG` / `INFO` / `WARN` / `ERROR` / `FATAL` 五种日志级别
* 支持日志级别过滤
* 支持输出时间戳、线程 ID、源码文件名、行号和日志内容
* 支持宏调用方式，例如 `LOG_INFO("message")`
* 使用 `std::mutex` 保证多线程写日志时的安全性
* 基于 CMake 构建，项目结构清晰，便于后续扩展
* 使用 `.clang-format` 统一代码风格

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

## 使用示例

```cpp
#include "Logger.h"

int main() {
    Logger::instance().setLevel(LogLevel::DEBUG);
    Logger::instance().setOutputFile("app.log");

    LOG_DEBUG("debug message");
    LOG_INFO("info message");
    LOG_WARN("warn message");
    LOG_ERROR("error message");

    return 0;
}
```

示例输出：

```text
[2026-06-06 15:30:21] [INFO] [thread:140391289259840] [main.cpp:8] info message
```

## 核心设计

本项目采用同步日志模型。

业务线程调用日志接口时，会直接完成日志格式化与写入操作。为了保证多个线程同时写日志时不会发生输出交错，日志写入过程使用 `std::mutex` 进行保护。

基本流程如下：

```text
业务线程调用日志宏
        |
        v
生成日志级别、时间、线程 ID、文件名、行号等信息
        |
        v
       加锁
        |
        v
写入控制台或日志文件
        |
        v
       解锁
```

同步日志库的优点是实现简单、逻辑清晰、可靠性高，适合学习日志系统的基本结构。

缺点是当日志量较大时，业务线程会直接承担日志写入开销，可能影响程序性能。因此，本项目也可以作为后续实现异步日志系统的基础版本。

## 日志级别

日志级别从低到高依次为：

```text
DEBUG < INFO < WARN < ERROR < FATAL
```

当设置当前日志级别后，低于该级别的日志不会被输出。

例如：

```cpp
Logger::instance().setLevel(LogLevel::WARN);
```

此时只会输出 `WARN` \ `ERROR` \ `FATAL` 日志，`DEBUG` 和 `INFO` 日志会被过滤。

## 线程安全

本项目使用 `std::mutex` 对日志写入过程进行加锁，保证多个线程同时写日志时不会出现内容交错的问题。

示例：

```cpp
#include "Logger.h"

#include <thread>
#include <vector>

int main() {
    Logger::instance().setLevel(LogLevel::DEBUG);

    std::vector<std::thread> threads;

    for (int i = 0; i < 5; ++i) {
        threads.emplace_back([i]() {
            for (int j = 0; j < 10; ++j) {
                LOG_INFO("message from thread");
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    return 0;
}
```

## 后续计划

* 支持日志文件按大小滚动
* 支持日志文件按日期滚动
* 支持自定义日志格式
* 支持配置文件初始化日志系统
* 支持更完善的单元测试

## License

MIT License
