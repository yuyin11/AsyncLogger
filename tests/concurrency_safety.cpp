
#include "catch2/catch_test_macros.hpp"
#include "mylogger/formatter/pattern_formatter.h"
#include "mylogger/logger.h"
#include "mylogger/sink/memory_sink.h"
#include <thread>
#include <vector>

TEST_CASE("logger multithread noloss", "[logger][concurrency]") {
  mylogger::Logger logger_;
  logger_.setLevel(mylogger::Level::TRACE);

  auto sink = std::make_unique<mylogger::MemorySink>(
      std::make_unique<mylogger::PatternFormatter>());
  auto *ptr = sink.get();
  logger_.addSink(std::move(sink));

  constexpr int kThreads = 8;
  constexpr int kPerThread = 1000;
  std::vector<std::thread> workers;
  for (int t = 0; t < kThreads; ++t) {
    workers.emplace_back([&logger_, t] {
      for (int i = 0; i < kPerThread; ++i)
        logger_.log(mylogger::Level::TRACE, __FILE__, __LINE__,
                    "thread{} msg{}", t, i);
    });
  }
  for (auto &w : workers)
    w.join();
  logger_.stop();
  REQUIRE(ptr->lines().size() == kThreads * kPerThread);
}
