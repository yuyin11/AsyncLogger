# AsyncLogger
A learning-purpose async C++ logger

## Features
- **Asynchronous logging** -- producer threads never block on I/O.
- **Multiple sinks** -- write to files, rotating files, memory ...
- **Log levels** -- TRACE, DEBUG, INFO, WARN, ERROR, FATAL.
- **Thread-safe** -- designed for multi-threaded producers.
- **Configurable batching** -- tune 'kLocalBufferSize' for your workload.
- **Rotating file sink** -- size-based rotation with a configurable number of files.
- **C++17** -- no external dependencies except [Catch2](https://github.com/catchorg/Catch2) for tests.

## Requirements
- CMake 3.20+
- C++17 compiler (GCC, Clang, MSVC)
- [Catch2](https://github.com/catchorg/Catch2) v3 (fetch automatically via 'FetchContent' if not installed)

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
## Benchmark Results

### Test Environment
- CPU: AMD Ryzen 7 5800H with Radeon Graphics
- RAM: 16GB
- OS: Arch Linux(x86_64)
- Compiler : GCC 16.2.1
- Build: '-O2 -DNDEBUG'
- Sink: NullSink (no I/O)
- Messages per thread: 100,000
- Warmup: 1 round, then 3 measured rounds, median reported

### Baseline (v1.0.0)
Single global queue + single worker thread + one mutex
|Threads | Throughput/s  |  P50(us)  |  P99(us)  |P999(us)|
|--------|---------------|-----------|-----------|--------|
|1       |1111123        |0.28       |7.23       |15.05   |
|2       |1658056        |0.26       |12.56      |24.27   |
|4       |2060456        |0.71       |15.06      |27.72   |
|8       |2129903        |1.25       |24.59      |41.38   |

Observation: Throughtput plateaus around 2.1M/s. P50 and P99 grow with thread count -- classic lock contention.

### v1.1.0: Thread-Local Buffer + Batch Swap in Consumer + kLocalBufferSize = 8192
Each producer thread buffers records locally and flushes a batch under lock.
Consumer swaps out the entire queue under lock, processes outside the lock. 
8192 is the sweet spot. Beyond it, cache misses from the enlarged local buffer
and longer lock hold time during flush outweigh the reduced lock frequency.
|Threads | Throughput/s  |  P50(us)  |  P99(us)  |P999(us)|
|--------|---------------|-----------|-----------|--------|
|8       |6607640        |0.19       |0.86       |1.51    |

Throughput at 8T: 2.1M -> 6.6M (+214%)
P99 at 8T: 24.59us -> 0.86us (-96%)
