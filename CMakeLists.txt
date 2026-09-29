cmake_minimum_required(VERSION 3.20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(SinkLogger LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

find_package(fmt REQUIRED)
find_package(Catch2 3 REQUIRED)

add_library(mylogger
  src/logger.cpp
  src/level.cpp
  src/sink/console_sink.cpp
  src/sink/file_sink.cpp
  src/sink/rotating_file_sink.cpp
  src/formatter/pattern_formatter.cpp
)

target_include_directories(mylogger
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_executable(test_logger
  ${CMAKE_CURRENT_SOURCE_DIR}/tests/level_filt.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/tests/discard_test.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/tests/rotating_logic.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/tests/sink_independence.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/tests/concurrency_safety.cpp
)

target_link_libraries(test_logger
  PRIVATE
    Catch2::Catch2WithMain
    fmt::fmt
    mylogger
)

add_executable(bench_logger
  ${CMAKE_CURRENT_SOURCE_DIR}/benchmarks/bench_logger.cpp
)
target_link_libraries(bench_logger
  PRIVATE
    mylogger
    fmt::fmt
)
target_compile_options(bench_logger PRIVATE -O2)

include(Catch)

catch_discover_tests(test_logger)
