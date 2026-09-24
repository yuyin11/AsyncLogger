
#include "mylogger/sink/console_sink.h"
#include <iostream>

void mylogger::ConsoleSink::write(LogRecord const &rec) {
  std::cout << format(rec);
}

void mylogger::ConsoleSink::flush() {
  std::cout.flush();
  std::cerr.flush();
}
