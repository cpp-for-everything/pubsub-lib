import time
import statistics

class Emitter:
    def __init__(self):
        self.handlers = []
    def on(self, f):
        self.handlers.append(f)
    def emit(self, x):
        for h in self.handlers:
            h(x)

def heavy_workload(x):
    s = x
    for i in range(1, 1001):
        s += i * i
    return s

def create_emitter(subs):
    e = Emitter()
    for _ in range(subs):
        e.on(heavy_workload)
    return e

def run_benchmark(sub_counts, repetitions=10):
    for subs in sub_counts:
        e = create_emitter(subs)
        durations = []

        for _ in range(repetitions):
            start = time.perf_counter_ns()
            e.emit(42)
            end = time.perf_counter_ns()
            dur = end - start
            durations.append(dur)
            time_per_sub = dur / subs
            subs_per_sec = 1e9 / time_per_sub
            print(f"Py_Emit/{subs}/repeats:{repetitions} real_time {dur} ns  {subs_per_sec:.2f}/s  {time_per_sub:.1f} ns/sub")

        mean = statistics.mean(durations)
        median = statistics.median(durations)
        stddev = statistics.stdev(durations)
        cv = stddev / mean * 100

        mean_tps = 1e9 / (mean / subs)

        print(f"Py_Emit/{subs}/mean     {int(mean)} ns  {mean_tps:.2f}/s  {(mean/subs):.1f} ns/sub")
        print(f"Py_Emit/{subs}/median   {int(median)} ns")
        print(f"Py_Emit/{subs}/stddev   {int(stddev)} ns")
        print(f"Py_Emit/{subs}/cv       {cv:.2f} %")
        print("")

run_benchmark([1, 10, 100, 500, 1000])
