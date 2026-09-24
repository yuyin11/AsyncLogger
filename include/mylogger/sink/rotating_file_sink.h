
#pragma once

#include "mylogger/sink/file_sink.h"

namespace mylogger {
class RotatingFileSink : public FileSink {
public:
  explicit RotatingFileSink(std::unique_ptr<Formatter> formatter,
                            std::string path, std::size_t max_size,
                            std::size_t max_files);

protected:
  void writeToFile(const std::string &line) override;

private:
  std::size_t max_size_;
  std::size_t max_files_;

  void rotate();
  void cleanupOldFiles();
};

} // namespace mylogger
