
#pragma once

#include "mylogger/sink/sink.h"
#include <atomic>
#include <condition_variable>
#include <fmt/core.h>
#include <fmt/format.h>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>
#include <vector>

namespace mylogger {
class Logger {
public:
  Logger();
  ~Logger();
  static Logger &getInstance() {
    static Logger instance;
    return instance;
  }

  Logger(const Logger &) = delete;
  Logger &operator=(const Logger &) = delete;
  Logger(Logger &&) = delete;
  Logger &operator=(Logger &&) = delete;

  void addSink(std::unique_ptr<Sink> sink);
  void setLevel(Level level);
  template <typename... Args>
  void log(Level level, char const *file, int line,
           fmt::format_string<Args...> format_str, Args &&...args);
  void flush();
  void stop();
  void reset();

private:
  void backendLoop();

  std::queue<LogRecord> buffer_;
  std::thread worker_;
  std::mutex mtx_;
  std::mutex sinks_mtx_;
  std::condition_variable cv_;
  bool running_ = true;

  std::vector<std::unique_ptr<Sink>> sinks_;
  std::atomic<Level> level_ = Level::TRACE;
};

template <typename... Args>
void Logger::log(Level level, char const *file, int line,
                 fmt::format_string<Args...> format_str, Args &&...args) {
  if (level < level_)
    return;
  LogRecord rec{level, std::chrono::system_clock::now(), file, line,
                fmt::format(format_str, std::forward<Args>(args)...)};

  std::unique_lock<std::mutex> lock(mtx_);
  buffer_.push(rec);
  cv_.notify_one();
}

} // namespace mylogger
