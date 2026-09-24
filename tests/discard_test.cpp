
#include "catch2/catch_test_macros.hpp"
#include "mylogger/logger.h"
#include "mylogger/macros.h"
#include "mylogger/sink_factory.h"
#include <filesystem>
#include <iterator>

namespace fs = std::filesystem;

fs::path makeTestDir(std::string const &name) {
  auto dir = fs::temp_directory_path() / ("mylogger_test_" + name);
  fs::remove_all(dir);
  fs::create_directories(dir);
  return dir;
}

std::size_t countLines(std::filesystem::path const &p) {
  std::ifstream f(p);
  return static_cast<std::size_t>(std::count(std::istreambuf_iterator<char>(f),
                                             std::istreambuf_iterator<char>(),
                                             '\n'));
}

TEST_CASE("log discard detection", "[logger][output]") {
  logger.reset();
  logger.setLevel(mylogger::Level::TRACE);

  auto dir = makeTestDir("rotating_no_loss");
  auto path = (dir / "app.log").string();

  logger.addSink(mylogger::makeRotatingFileSink(path, 1024 * 1024, 3));

  for (int i = 0; i < 1000; ++i) {
    LOG_INFO("msg{}", i);
  }
  logger.stop();
  std::size_t total = 0;
  for (auto &entry : fs::directory_iterator(dir)) {
    total += countLines(entry);
  }
  REQUIRE(total == 1000);

  fs::remove_all(dir);
}
