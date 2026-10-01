
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
  static size_t constexpr kLocalBufferSize = 8192;
  struct TLBuffer {
    std::vector<LogRecord> records;
    Logger *owner = nullptr;

    ~TLBuffer() {
      if (owner && !records.empty()) {
        owner->flushBatch(records);
      }
    }
  };

  std::thread worker_;
  std::mutex mtx_;
  std::mutex sinks_mtx_;
  std::condition_variable cv_;

  static thread_local TLBuffer tls_;
  std::queue<LogRecord> buffer_;
  bool running_ = true;
  std::vector<std::unique_ptr<Sink>> sinks_;
  std::atomic<Level> level_ = Level::TRACE;

  void backendLoop();
  void flush();
  void push(LogRecord rec);
  void flushBatch(std::vector<LogRecord> &batch);
};

template <typename... Args>
void Logger::log(Level level, char const *file, int line,
                 fmt::format_string<Args...> format_str, Args &&...args) {
  if (level < level_)
    return;
  LogRecord rec{level, std::chrono::system_clock::now(), file, line,
                fmt::format(format_str, std::forward<Args>(args)...)};
  push(rec);
}

} // namespace mylogger
