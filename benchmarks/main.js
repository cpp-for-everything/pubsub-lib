const { EventEmitter } = require('events');
const { performance } = require('perf_hooks');

function heavyCallback(x) {
  let sum = x;
  for (let i = 1; i <= 1000; i++) {
    sum += i * i;
  }
  return sum;
}

function benchmarkNodePubSub(subCount, iterations = 10) {
  const emitter = new EventEmitter();

  for (let i = 0; i < subCount; i++) {
    emitter.on('event', heavyCallback);
  }

  const times = [];

  for (let j = 0; j < iterations; j++) {
    const t0 = performance.now();
    emitter.emit('event', 42);
    const t1 = performance.now();
    times.push(t1 - t0);
  }

  const avgMs = times.reduce((a, b) => a + b, 0) / times.length;
  const avgNsPerSub = (avgMs * 1e6) / subCount;
  const throughput = 1e9 / avgNsPerSub;

  console.log(`Subscribers: ${subCount}`);
  console.log(`Avg latency: ${avgNsPerSub.toFixed(1)} ns per sub`);
  console.log(`Throughput: ${throughput.toFixed(2)} callbacks/sec`);
}

[1, 10, 100, 500, 1000].forEach(count => benchmarkNodePubSub(count));
