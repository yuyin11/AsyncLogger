
#include "mylogger/formatter/pattern_formatter.h"

std::string mylogger::PatternFormatter::format(LogRecord const &rec) {
  std::string msg =
      fmt::format("[{}] [{}] [{}:{}] {}", formatTime(rec.time),
                  level_to_string(rec.level), rec.file, rec.line, rec.msg);
  msg.push_back('\n');
  return msg;
}

std::string mylogger::PatternFormatter::formatTime(
    std::chrono::system_clock::time_point tp) {
  auto local =
      std::chrono::floor<std::chrono::seconds>(tp) + std::chrono::hours(8);
  return fmt::format("{:%Y-%m-%d %H:%M:%S}", local);
}
