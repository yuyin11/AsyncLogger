
#pragma once

#include "mylogger/sink/sink.h"
#include <cstddef>
#include <fstream>
#include <memory>

namespace mylogger {
class FileSink : public Sink {
public:
  explicit FileSink(std::unique_ptr<Formatter> formatter, std::string path);
  void write(LogRecord const &rec) override;
  void flush() override;
  void reportErrorOnce();

protected:
  std::size_t current_size_ = 0;
  std::ofstream file_;
  std::string path_;

  virtual void writeToFile(std::string const &line) {
    file_ << line;
    current_size_ += line.size();
    if (!file_) {
      std::fprintf(stderr, "write failed! status = %d\n", (int)file_.rdstate());
      file_.clear();
    }
  }

private:
  bool error_reported_ = true;
};

} // namespace mylogger
