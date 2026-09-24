
#pragma once

#include "mylogger/formatter/formatter.h"
#include "mylogger/log_record.h"
#include <memory>

namespace mylogger {
class Sink {
public:
  explicit Sink(std::unique_ptr<Formatter> fmt) : formatter_(std::move(fmt)) {}
  virtual ~Sink() = default;
  virtual void write(LogRecord const &rec) = 0;
  virtual void flush() = 0;

protected:
  std::string format(LogRecord const &rec) { return formatter_->format(rec); }

private:
  std::unique_ptr<Formatter> formatter_;
};

} // namespace mylogger
