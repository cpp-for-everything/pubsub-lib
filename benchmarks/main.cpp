#include <benchmark/benchmark.h>
#include <iostream>
#include <functional>
#include <vector>

// ========== pubsub-lib ==========
#include "pubsub.h" // path to your pubsub-lib header

// ======= Benchmark Setup =======

// Simple event struct
constexpr auto MyEvent = pubsub::Event<void(int)>();

// Global for pubsub-lib
pubsub::Publisher pub;
auto token = pub.subscribe<MyEvent>([](int data) {
    int dummy = data;
    benchmark::DoNotOptimize(dummy);
});

// ================= pubsub-lib Benchmark =================
static void BM_PubSubLib(benchmark::State& state) {
    for (auto _ : state) {
        pub.emit<MyEvent>(42);
    }
}
BENCHMARK(BM_PubSubLib);

// =============== pubsub-lib async Benchmark ==============
static void BM_PubSubLib_ASYNC(benchmark::State& state) {
    for (auto _ : state) {
        pub.emit_async<MyEvent>(42);
    }
}
BENCHMARK(BM_PubSubLib_ASYNC);

// ================= Boost.Signals2 Benchmark =================
#ifdef USE_BOOST
#include <boost/signals2.hpp>

static void BM_BoostSignals2(benchmark::State& state) {
    boost::signals2::signal<void(int)> signal;
    signal.connect([](int data) {
        int dummy = data;
        benchmark::DoNotOptimize(dummy);
    });

    for (auto _ : state) {
        signal(42);
    }
}
BENCHMARK(BM_BoostSignals2);
#endif

// ================= Qt Signals Benchmark =================
#ifdef USE_QT
#include <QObject>

class QtEmitter : public QObject {
    Q_OBJECT
signals:
    void mySignal(int);
};

static void BM_QtSignals(benchmark::State& state) {
    QtEmitter emitter;
    QObject::connect(&emitter, &QtEmitter::mySignal, [](int val) {
        int dummy = val;
        benchmark::DoNotOptimize(dummy);
    });

    for (auto _ : state) {
        emit emitter.mySignal(42);
    }
}
BENCHMARK(BM_QtSignals);
#endif

BENCHMARK_MAIN();

#ifdef USE_QT
#include "main.moc"
#endif