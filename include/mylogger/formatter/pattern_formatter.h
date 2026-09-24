
#pragma once

#include "mylogger/formatter/formatter.h"
#include <chrono>

namespace mylogger {
class PatternFormatter : public Formatter {
public:
  std::string format(LogRecord const &rec) override;

private:
  static std::string formatTime(std::chrono::system_clock::time_point tp);
};

} // namespace mylogger
