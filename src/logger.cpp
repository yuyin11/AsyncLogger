
#include "mylogger/logger.h"
#include <exception>

mylogger::Logger::Logger() {
  worker_ = std::thread(&Logger::backendLoop, this);
}

mylogger::Logger::~Logger() { stop(); }

void mylogger::Logger::backendLoop() {
  while (true) {
    LogRecord rec;
    {
      std::unique_lock<std::mutex> lock(mtx_);
      cv_.wait(lock, [this] { return !buffer_.empty() || !running_; });
      if (buffer_.empty() && !running_)
        break;
      rec = std::move(buffer_.front());
      buffer_.pop();
    }
    {
      std::lock_guard<std::mutex> lock(sinks_mtx_);
      for (auto &s : sinks_) {
        try {
          s->write(rec);
        } catch (std::exception const &e) {
          std::fprintf(stderr, "[logger] sink write failed: %s\n", e.what());
        } catch (...) {
          std::fprintf(stderr, "[logger] sink write failed: unknown\n");
        }
      }
    }
  }
}

void mylogger::Logger::addSink(std::unique_ptr<Sink> sink) {
  std::lock_guard<std::mutex> lock(sinks_mtx_);
  sinks_.push_back(std::move(sink));
}

void mylogger::Logger::setLevel(Level level) { level_ = level; }

void mylogger::Logger::stop() {
  {
    std::unique_lock<std::mutex> lock(mtx_);
    running_ = false;
    cv_.notify_one();
  }
  if (worker_.joinable()) {
    worker_.join();
  }
  flush();
}

void mylogger::Logger::flush() {
  std::lock_guard<std::mutex> lock(sinks_mtx_);
  for (auto &s : sinks_) {
    s->flush();
  }
}

void mylogger::Logger::reset() {
  if (worker_.joinable())
    return;
  running_ = true;
  sinks_.clear();
  worker_ = std::thread(&Logger::backendLoop, this);
}
