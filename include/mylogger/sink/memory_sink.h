
#pragma once

#include "mylogger/sink/sink.h"
#include <vector>

namespace mylogger {
class MemorySink : public Sink {
public:
  explicit MemorySink(std::unique_ptr<Formatter> formatter)
      : Sink(std::move(formatter)) {}
  void write(LogRecord const &rec) override { lines_.push_back(format(rec)); }
  void flush() noexcept override {}

  std::vector<std::string> const &lines() const { return lines_; }

private:
  std::vector<std::string> lines_;
};

} // namespace mylogger
