
#include "catch2/catch_test_macros.hpp"
#include "mylogger/formatter/pattern_formatter.h"
#include "mylogger/macros.h"
#include "mylogger/sink/failing_sink.h"
#include "mylogger/sink/memory_sink.h"
#include <memory>
#include <mylogger/logger.h>

TEST_CASE("dispatch to all sinks", "[logger][sink]") {
  logger.reset();
  logger.setLevel(mylogger::Level::TRACE);

  auto sink_a = std::make_unique<mylogger::MemorySink>(
      std::make_unique<mylogger::PatternFormatter>());
  auto sink_b = std::make_unique<mylogger::MemorySink>(
      std::make_unique<mylogger::PatternFormatter>());

  auto *ptr_a = sink_a.get();
  auto *ptr_b = sink_b.get();

  logger.addSink(std::move(sink_a));
  logger.addSink(std::move(sink_b));

  LOG_INFO("hello from test");
  logger.stop();

  REQUIRE(ptr_a->lines().size() == 1);
  REQUIRE(ptr_b->lines().size() == 1);
  REQUIRE(ptr_a->lines()[0].find("hello from test") != std::string::npos);
  REQUIRE(ptr_b->lines()[0].find("hello from test") != std::string::npos);
}

TEST_CASE("failing sink does not break other sinks", "[logger][sink]") {
  logger.reset();
  logger.setLevel(mylogger::Level::TRACE);

  auto failing = std::make_unique<mylogger::FailingSink>(
      std::make_unique<mylogger::PatternFormatter>());
  auto memory = std::make_unique<mylogger::MemorySink>(
      std::make_unique<mylogger::PatternFormatter>());

  auto *failing_ptr = failing.get();
  auto *memory_ptr = memory.get();

  logger.addSink(std::move(failing));
  logger.addSink(std::move(memory));

  LOG_INFO("msg 1");
  LOG_INFO("msg 2");
  logger.stop();

  REQUIRE(failing_ptr->writeCount() == 2);
  REQUIRE(memory_ptr->lines().size() == 2);
}
