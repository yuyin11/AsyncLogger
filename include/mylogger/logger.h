
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

  Logger(Logger const &) = delete;
  Logger &operator=(Logger const &) = delete;
  Logger(Logger &&) = delete;
  Logger &operator=(Logger &&) = delete;

  void addSink(std::unique_ptr<Sink> sink);
  inline void setLevel(Level level) { level_ = level; }
  template <typename... Args>
  void log(Level level, char const *file, int line,
           fmt::format_string<Args...> format_str, Args &&...args);
  void stop();
  void reset();

private:
  void backendLoop();
  void flush();

  std::thread worker_;
  std::mutex mtx_;
  std::mutex sinks_mtx_;

  std::queue<LogRecord> buffer_;
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
