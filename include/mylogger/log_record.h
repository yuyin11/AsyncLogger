
#pragma once

#include "mylogger/level.h"
#include <fmt/chrono.h>
#include <fmt/format.h>

namespace mylogger {
struct LogRecord {
  Level level;
  std::chrono::system_clock::time_point time;
  char const *file;
  int line;
  std::string msg;
};

} // namespace mylogger
