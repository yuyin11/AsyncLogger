
#pragma once

#include "mylogger/sink/sink.h"
#include <memory>

namespace mylogger {
class ConsoleSink : public Sink {
public:
  explicit ConsoleSink(std::unique_ptr<Formatter> formatter)
      : Sink(std::move(formatter)) {}
  void write(LogRecord const &rec) override;
  void flush() override;
};

} // namespace mylogger
