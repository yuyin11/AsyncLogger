
#include "mylogger/logger.h"
#include <exception>
#include <mutex>

thread_local mylogger::Logger::TLBuffer mylogger::Logger::tls_;

mylogger::Logger::Logger() {
  worker_ = std::thread(&Logger::backendLoop, this);
}

mylogger::Logger::~Logger() { stop(); }

void mylogger::Logger::backendLoop() {
  std::queue<LogRecord> local;
  while (true) {
    {
      std::unique_lock<std::mutex> lock(mtx_);
      cv_.wait(lock, [this] { return !buffer_.empty() || !running_; });
      if (buffer_.empty() && !running_)
        break;
      std::swap(local, buffer_);
    }
    {
      std::lock_guard<std::mutex> lock(sinks_mtx_);
      while (!local.empty()) {
        for (auto &s : sinks_) {
          try {
            s->write(local.front());
          } catch (std::exception const &e) {
            std::fprintf(stderr, "[logger] sink write failed: %s\n", e.what());
          } catch (...) {
            std::fprintf(stderr, "[logger] sink write failed: unknown\n");
          }
          local.pop();
        }
      }
    }
  }
}

void mylogger::Logger::addSink(std::unique_ptr<Sink> sink) {
  std::lock_guard<std::mutex> lock(sinks_mtx_);
  sinks_.push_back(std::move(sink));
}

void mylogger::Logger::stop() {
  if (tls_.owner == this && !tls_.records.empty()) {
    flushBatch(tls_.records);
  }
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
  stop();
  {
    std::lock_guard<std::mutex> lock(sinks_mtx_);
    sinks_.clear();
  }
  {
    std::lock_guard<std::mutex> lock(mtx_);
    running_ = true;
  }
  worker_ = std::thread(&Logger::backendLoop, this);
}

void mylogger::Logger::push(LogRecord rec) {
  if (tls_.owner != this) {
    if (tls_.owner && !tls_.records.empty()) {
      tls_.owner->flushBatch(tls_.records);
    }
    tls_.owner = this;
    tls_.records.clear();
  }
  tls_.records.push_back(std::move(rec));
  if (tls_.records.size() >= kLocalBufferSize) {
    flushBatch(tls_.records);
  }
}

void mylogger::Logger::flushBatch(std::vector<LogRecord> &batch) {
  if (batch.empty())
    return;
  {
    std::lock_guard<std::mutex> lock(mtx_);
    for (auto &r : batch)
      buffer_.push(std::move(r));
  }
  cv_.notify_one();
  batch.clear();
}
