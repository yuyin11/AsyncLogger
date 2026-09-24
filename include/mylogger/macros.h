
#pragma once

#define LOG_TRACE(...)                                                         \
  mylogger::Logger::getInstance().log(mylogger::Level::TRACE, __FILE__,        \
                                      __LINE__, __VA_ARGS__)
#define LOG_DEBUG(...)                                                         \
  mylogger::Logger::getInstance().log(mylogger::Level::DEBUG, __FILE__,        \
                                      __LINE__, __VA_ARGS__)
#define LOG_INFO(...)                                                          \
  mylogger::Logger::getInstance().log(mylogger::Level::INFO, __FILE__,         \
                                      __LINE__, __VA_ARGS__)
#define LOG_WARN(...)                                                          \
  mylogger::Logger::getInstance().log(mylogger::Level::WARN, __FILE__,         \
                                      __LINE__, __VA_ARGS__)
#define LOG_ERROR(...)                                                         \
  mylogger::Logger::getInstance().log(mylogger::Level::ERROR, __FILE__,        \
                                      __LINE__, __VA_ARGS__)
#define LOG_FATAL(...)                                                         \
  mylogger::Logger::getInstance().log(mylogger::Level::FATAL, __FILE__,        \
                                      __LINE__, __VA_ARGS__)

#define logger mylogger::Logger::getInstance()
