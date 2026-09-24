
#include "mylogger/level.h"

std::string mylogger::level_to_string(Level level) {
  switch (level) {
  case Level::TRACE:
    return "TRACE";
  case Level::DEBUG:
    return "DEBUG";
  case Level::INFO:
    return "INFO";
  case Level::WARN:
    return "WARN";
  case Level::ERROR:
    return "ERROR";
  case Level::FATAL:
    return "FATAL";
  }
  return "UNKN";
}
