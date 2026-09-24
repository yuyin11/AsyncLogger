
#pragma once

#include "mylogger/log_record.h"
#include <string>

namespace mylogger {

class Formatter {
public:
  virtual ~Formatter() = default;
  virtual std::string format(LogRecord const &rec) = 0;
};

} // namespace mylogger
