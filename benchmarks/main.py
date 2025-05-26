import time

def heavy_callback(x):
    total = x
    for i in range(1, 1001):
        total += i * i
    return total

class PubSub:
    def __init__(self):
        self.subscribers = {}

    def subscribe(self, event, callback):
        self.subscribers.setdefault(event, []).append(callback)

    def publish(self, event, data):
        for cb in self.subscribers.get(event, []):
            cb(data)

def benchmark_python_pubsub(sub_count, iterations=10):
    pubsub = PubSub()
    for _ in range(sub_count):
        pubsub.subscribe('event', heavy_callback)

    times = []
    for _ in range(iterations):
        start = time.perf_counter_ns()
        pubsub.publish('event', 42)
        end = time.perf_counter_ns()
        times.append(end - start)

    avg_ns = sum(times) / len(times)
    ns_per_sub = avg_ns / sub_count
    throughput = 1e9 / ns_per_sub

    print(f"Subscribers: {sub_count}")
    print(f"Avg latency: {ns_per_sub:.1f} ns per sub")
    print(f"Throughput: {throughput:.2f} callbacks/sec")

for count in [1, 10, 100, 500, 1000]:
    benchmark_python_pubsub(count)
