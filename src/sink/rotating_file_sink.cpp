
#include "mylogger/sink/rotating_file_sink.h"
#include <cstdio>
#include <filesystem>
#include <system_error>
#include <vector>

namespace fs = std::filesystem;

mylogger::RotatingFileSink::RotatingFileSink(
    std::unique_ptr<Formatter> formatter, std::string path,
    std::size_t max_size, std::size_t max_files)
    : FileSink(std::move(formatter), std::move(path)), max_size_(max_size),
      max_files_(max_files) {}

void mylogger::RotatingFileSink::writeToFile(std::string const &line) {
  if (line.size() + current_size_ > max_size_) {
    rotate();
  }
  FileSink::writeToFile(line);
}

void mylogger::RotatingFileSink::rotate() {
  file_.close();
  auto now = std::chrono::system_clock::now();
  auto local = now + std::chrono::hours(8);
  std::string time_str = fmt::format("{:%Y-%m-%d_%H:%M:%S}", local);
  std::string filename = path_ + "_" + time_str;
  std::error_code ec;
  fs::rename(path_, filename, ec);
  if (ec) {
    std::fprintf(stderr, "rotate rename failed: %s\n", ec.message().c_str());
    file_.open(path_);
    return;
  }
  cleanupOldFiles();
  file_.open(path_, std::ios::app);
  if (!file_)
    return;
  current_size_ = 0;
}

void mylogger::RotatingFileSink::cleanupOldFiles() {
  if (max_files_ == 0)
    return;
  std::string const base = fs::path(path_).filename().string();
  std::string const prefix = base + "_";

  fs::path dir = fs::path(path_).parent_path();
  if (dir.empty())
    dir = ".";
  std::vector<fs::path> files;
  std::error_code iter_ec;

  for (auto &entry : fs::directory_iterator(dir, iter_ec)) {
    if (iter_ec)
      break;
    if (!entry.is_regular_file())
      continue;

    std::string const name = entry.path().filename().string();
    if (name.rfind(prefix, 0) == 0) {
      files.push_back(entry.path());
    }
  }

  if (files.size() <= max_files_)
    return;
  std::sort(files.begin(), files.end());
  while (files.size() > max_files_) {
    std::error_code rm_ec;
    fs::remove(files.front(), rm_ec);
    if (rm_ec) {
      std::fprintf(stderr, "remove old log failed: %s\n",
                   rm_ec.message().c_str());
    }
    files.erase(files.begin());
  }
}
