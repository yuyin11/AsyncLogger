
#pragma once

#include "mylogger/log_record.h"
#include "mylogger/sink/sink.h"
#include <stdexcept>

namespace mylogger {
class FailingSink : public Sink {
public:
  explicit FailingSink(std::unique_ptr<Formatter> f) : Sink(std::move(f)) {}
  void write(LogRecord const &rec) override {
    ++write_count_;
    throw std::runtime_error("intentional failure");
  }
  void flush() noexcept override {}
  int writeCount() const { return write_count_; }

private:
  int write_count_ = 0;
};

} // namespace mylogger
