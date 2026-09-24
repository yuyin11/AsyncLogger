
#pragma once

#include <string>

namespace mylogger {
enum class Level { TRACE, DEBUG, INFO, WARN, ERROR, FATAL };
std::string level_to_string(Level level);

} // namespace mylogger
