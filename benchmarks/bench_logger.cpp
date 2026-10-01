
#include "mylogger/formatter/formatter.h"
#include "mylogger/logger.h"
#include "mylogger/sink/sink.h"
#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstdio>
#include <utility>
#include <vector>

#define LOG_INFO(...)                                                          \
  logger.log(mylogger::Level::INFO, __FILE__, __LINE__, __VA_ARGS__)

// 空Sink 和 Formatter
class NullSink : public mylogger::Sink {
public:
  explicit NullSink(std::unique_ptr<mylogger::Formatter> formatter)
      : mylogger::Sink(std::move(formatter)) {}
  void write(mylogger::LogRecord const &) override {}
  void flush() override {}
};

class NullFormatter : public mylogger::Formatter {
  std::string format(mylogger::LogRecord const &) override { return ""; }
};

struct BenchResult {
  double throughput;
  double p50_us;
  double p99_us;
  double p999_us;
};

BenchResult runBench(int numThreads, int msgsPerThread) {
  mylogger::Logger logger;
  logger.setLevel(mylogger::Level::TRACE);
  logger.addSink(std::make_unique<NullSink>(std::make_unique<NullFormatter>()));

  std::vector<std::vector<double>> latencies(numThreads);
  for (auto &v : latencies)
    v.reserve(msgsPerThread);

  // 每个线程；记录latencies
  std::atomic<bool> start{false};
  auto worker = [&](int tid) {
    while (!start.load(std::memory_order_acquire)) {
    }
    auto &lat = latencies[tid];
    for (int i = 0; i < msgsPerThread; ++i) {
      auto t0 = std::chrono::steady_clock::now();
      LOG_INFO("thread{}, msg{}", tid, i);
      auto t1 = std::chrono::steady_clock::now();
      lat.push_back(std::chrono::duration<double, std::micro>(t1 - t0).count());
    }
  };

  // 执行函数；处于忙等待
  std::vector<std::thread> threads;
  for (int t = 0; t < numThreads; ++t)
    threads.emplace_back(worker, t);

  // 统一开始执行（start）；结束join；记录执行的总时间（end - begin）
  auto begin = std::chrono::steady_clock::now();
  start.store(true, std::memory_order_release);
  for (auto &t : threads)
    t.join();
  logger.stop();
  auto end = std::chrono::steady_clock::now();

  // 将结果放在一个vector中；排序
  std::vector<double> all;
  all.reserve(numThreads * msgsPerThread);
  for (auto &v : latencies)
    all.insert(all.end(), v.begin(), v.end());
  std::sort(all.begin(), all.end());

  // 百分比结果函数
  auto percentile = [&](double p) {
    return all[static_cast<size_t>(all.size() * p)];
  };

  double seconds = std::chrono::duration<double>(end - begin).count();
  double throughput = (numThreads * msgsPerThread) / seconds;

  return {throughput, percentile(0.50), percentile(0.99), percentile(0.999)};
}

int main() {
  constexpr int kMsgsPerThread = 100000;
  std::vector<int> threadCounts = {1, 2, 4, 8};
  printf("%-8s %-15s %-10s %-10s %-10s\n", "Threads", "Throughput/s", "P50(us)",
         "P99(us)", "P999(us)");
  printf("---------------------------------------------------------\n");

  for (int tc : threadCounts) {
    // 预热
    runBench(tc, 1000);

    // 得到3次结果
    std::vector<BenchResult> results;
    for (int r = 0; r < 3; ++r)
      results.push_back(runBench(tc, kMsgsPerThread));

    // 取中位数
    std::sort(results.begin(), results.end(),
              [](auto &a, auto &b) { return a.throughput < b.throughput; });
    auto &m = results[1];

    printf("%-8d %-15.0f %-10.2f %-10.2f %-10.2f\n", tc, m.throughput, m.p50_us,
           m.p99_us, m.p999_us);
  }
  return 0;
}
