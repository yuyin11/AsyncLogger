# SinkLogger
A learning-purpose async C++ logger with sink abstraction

## Features
- Async logging with background worker
- Log levels
- Pluggable sinks (console, file, rotating file, memory)
- Thread-safe

## Build
```bash
cmake -B build
cmake --build build
```

## Test
```bash
cd build
ctest
```
