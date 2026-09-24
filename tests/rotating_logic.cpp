
#include "catch2/catch_test_macros.hpp"
#include "mylogger/logger.h"
#include "mylogger/macros.h"
#include "mylogger/sink_factory.h"
#include <filesystem>
#include <system_error>

namespace fs = std::filesystem;

fs::path mkTestDir(std::string const &name) {
  auto dir = fs::temp_directory_path() / ("mylogger_test_" + name);
  fs::remove_all(dir);
  fs::create_directories(dir);
  return dir;
}

TEST_CASE("rotating logic", "[logger][output]") {
  // 文件大小 <= max_size_; 文件数量 <= max_files_
  logger.reset();
  logger.setLevel(mylogger::Level::TRACE);

  auto dir = mkTestDir("rotating_logic");
  auto log_path = (dir / "app.log").string();

  logger.addSink(mylogger::makeRotatingFileSink(log_path, 1024, 3));
  for (int i = 0; i < 100; ++i) {
    LOG_DEBUG("msg {}", i);
  }
  logger.stop();

  std::string const prefix = fs::path(log_path).filename().string() + "_";
  int count = 0;
  for (auto &entry : fs::directory_iterator(dir)) {
    if (!entry.is_regular_file())
      continue;
    std::error_code ec;
    std::string const name = entry.path().filename().string();
    if (name.rfind(prefix, 0) == 0) {
      ++count;
    }
    auto sz = fs::file_size(entry, ec);
    REQUIRE(!ec);
    REQUIRE(sz <= 1024);
  }
  REQUIRE(count >= 1);
  REQUIRE(count <= 3);
  REQUIRE(fs::exists(log_path));
  fs::remove_all(dir);
}
