
#include "mylogger/level.h"
#include "mylogger/logger.h"
#include "mylogger/macros.h"
#include "mylogger/sink/memory_sink.h"
#include "mylogger/sink_factory.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

TEST_CASE("level filter detection", "[logger][filter]") {
  logger.reset();
  auto memory_sink = std::make_unique<mylogger::MemorySink>(
      std::make_unique<mylogger::PatternFormatter>());
  auto *sink_ptr = memory_sink.get();
  logger.addSink(std::move(memory_sink));
  logger.setLevel(mylogger::Level::WARN);

  LOG_DEBUG("debug message");
  LOG_INFO("info message");
  LOG_WARN("warn message");
  LOG_ERROR("error message");
  logger.stop();

  auto const &lines = sink_ptr->lines();
  REQUIRE(lines.size() == 2);

  REQUIRE_THAT(lines[0], Catch::Matchers::ContainsSubstring("warn message"));
  REQUIRE_THAT(lines[1], Catch::Matchers::ContainsSubstring("error message"));
}
