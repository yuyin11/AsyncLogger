
#include "mylogger/sink/file_sink.h"
#include <cstdio>
#include <stdexcept>

mylogger::FileSink::FileSink(std::unique_ptr<Formatter> formatter,
                             std::string path)
    : Sink(std::move(formatter)), path_(path) {
  file_.open(path_, std::ios::app);
  if (!file_) {
    throw std::runtime_error("FileSink: cannot open " + path_);
  }
}

void mylogger::FileSink::write(LogRecord const &rec) {
  if (!file_.is_open()) {
    file_.open(path_, std::ios::app);
    if (!file_) {
      reportErrorOnce();
      return;
    }
    error_reported_ = false;
  }
  std::string line = format(rec);
  writeToFile(line);
  if (!file_) {
    file_.close();
  }
}

void mylogger::FileSink::flush() {
  if (file_.is_open())
    file_.flush();
  file_.close();
}

void mylogger::FileSink::reportErrorOnce() {
  if (error_reported_)
    return;
  std::fprintf(stderr, "FileSink: reopen failed: %s\n", path_.c_str());
  error_reported_ = true;
}
