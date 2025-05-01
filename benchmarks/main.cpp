#include <benchmark/benchmark.h>
#include <functional>
#include <memory>
#include <vector>
#include <unordered_map>
#include <cstdint>
#include <chrono>

#include "pubsub.h"

#ifdef __linux__
    #include <sys/types.h>
    #include <sys/sysinfo.h>
    #include <unistd.h>
#elif defined(_WIN32)
    #include <windows.h>
    #include <psapi.h>
#endif

// ========== Event Definition ==========
constexpr auto MyEvent = pubsub::Event<void(int)>();

// ========== Simulated Heavy Work ==========
void heavy_callback_workload(int x) {
    volatile uint64_t sum = x;
    for (int i = 1; i <= 1000; ++i)
        sum += i * i;
    benchmark::DoNotOptimize(sum);
}

// ========== Create Publisher with N Heavy Subscribers ==========
std::unique_ptr<pubsub::Publisher> create_publisher_with_heavy_subs(int num_subs) {
    auto pub = std::make_unique<pubsub::Publisher>();
    for (int i = 0; i < num_subs; ++i) {
        pub->subscribe<MyEvent>([](int x) {
            heavy_callback_workload(x);
        });
    }
    return pub;
}

// ========== Global Unordered Map of Prebuilt Publishers ==========
std::vector<int> subscriber_counts = {1, 10, 100, 500, 1000};
std::unordered_map<int, std::unique_ptr<pubsub::Publisher>> heavy_publishers;

// ========== Updated Benchmark Macro with Memory + Time Metrics ==========
#define DEFINE_HEAVY_EMIT_BENCH(name, emit_method)                                                                  \
    static void name(benchmark::State& state) {                                                                     \
        std::unordered_map<int, std::unique_ptr<pubsub::Publisher>>::iterator it;                                   \
        int subs = state.range(0);                                                                                  \
        it = heavy_publishers.find(subs);                                                                           \
        if (it == heavy_publishers.end()) state.SkipWithError("Missing pub");                                       \
        auto& pub = it->second;                                                                                     \
        for (auto _ : state) {                                                                                      \
            auto start_time = std::chrono::high_resolution_clock::now();                                            \
            benchmark::DoNotOptimize(pub->emit_method);                                                             \
            auto end_time = std::chrono::high_resolution_clock::now();                                              \
            auto duration = end_time - start_time;                                                                  \
            state.counters["time_per_sub_ns"] =                                                                     \
                std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count() / static_cast<double>(subs); \
            state.counters["subs_per_sec"] =                                                                        \
                benchmark::Counter(subs, benchmark::Counter::kIsRate);                                              \
        }                                                                                                           \
        state.SetComplexityN(state.range(0));                                                                       \
    }                                                                                                               \
    BENCHMARK(name)->MeasureProcessCPUTime()                                                                        \
                   ->UseRealTime()                                                                                  \
                   ->Repetitions(10)                                                                                 \
                   ->Args({1})->Args({10})->Args({100})->Args({500})->Args({1000})                                  \
                   ->Complexity(benchmark::oN)                                                                      \
    ;

// ========== Emit Variants ==========
DEFINE_HEAVY_EMIT_BENCH(BM_PubSub_Emit_Sync, emit<MyEvent>(42))

DEFINE_HEAVY_EMIT_BENCH(BM_PubSub_Emit_StdAsync_NoWait, emit_thread_async<MyEvent>(42))

#if defined(__cpp_lib_execution)
DEFINE_HEAVY_EMIT_BENCH(BM_PubSub_Emit_StdExec_seq, emit_async<MyEvent>(std::execution::seq, 42))
DEFINE_HEAVY_EMIT_BENCH(BM_PubSub_Emit_StdExec_par, emit_async<MyEvent>(std::execution::par, 42))
DEFINE_HEAVY_EMIT_BENCH(BM_PubSub_Emit_StdExec_par_unseq, emit_async<MyEvent>(std::execution::par_unseq, 42))
DEFINE_HEAVY_EMIT_BENCH(BM_PubSub_Emit_StdExec_unseq, emit_async<MyEvent>(std::execution::unseq, 42))
#endif

#ifdef WITH_TBB
DEFINE_HEAVY_EMIT_BENCH(BM_PubSub_Emit_TBB, emit_tbb_async<MyEvent>(42))
#endif

// ========== Manual Main ==========
int main(int argc, char** argv) {
    for (int count : subscriber_counts) {
        heavy_publishers[count] = create_publisher_with_heavy_subs(count);
    }

    benchmark::Initialize(&argc, argv);
    if (benchmark::ReportUnrecognizedArguments(argc, argv)) return 1;
    
    benchmark::RunSpecifiedBenchmarks();
    return 0;
}
