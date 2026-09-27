
#include "mylogger/formatter/pattern_formatter.h"
#include "mylogger/sink/console_sink.h"
#include "mylogger/sink/file_sink.h"
#include "mylogger/sink/rotating_file_sink.h"
#include <memory>

namespace mylogger {
inline std::unique_ptr<Sink> makeConsoleSink() {
  return std::make_unique<ConsoleSink>(std::make_unique<PatternFormatter>());
}

inline std::unique_ptr<Sink> makeFileSink(std::string path) {
  return std::make_unique<FileSink>(std::make_unique<PatternFormatter>(), path);
}

inline std::unique_ptr<Sink> makeRotatingFileSink(std::string path,
                                                  std::size_t max_size,
                                                  std::size_t max_files) {
  return std::make_unique<RotatingFileSink>(
      std::make_unique<PatternFormatter>(), path, max_size, max_files);
}
// inline std::unique_ptr<Sink> makeMemorySink() {
//   return std::make_unique<MemorySink>(std::make_unique<PatternFormatter>());
// }

} // namespace mylogger
