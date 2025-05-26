const { EventEmitter } = require('events');
const { performance } = require('perf_hooks');

function heavyWorkload(x) {
    let sum = x;
    for (let i = 1; i <= 1000; ++i) sum += i * i;
    return sum;
}

function createEmitterWithSubs(n) {
    const emitter = new EventEmitter();
    emitter.setMaxListeners(Infinity);
    for (let i = 0; i < n; ++i) emitter.on('myEvent', heavyWorkload);
    return emitter;
}

function stats(ns) {
    const mean = ns.reduce((a, b) => a + b, 0) / ns.length;
    const stddev = Math.sqrt(ns.map(x => (x - mean) ** 2).reduce((a, b) => a + b) / ns.length);
    const sorted = [...ns].sort((a, b) => a - b);
    const median = sorted[Math.floor(ns.length / 2)];
    const cv = (stddev / mean) * 100;
    return { mean, median, stddev, cv };
}

function runBenchmark(subCounts, repetitions = 10) {
    for (const subs of subCounts) {
        const emitter = createEmitterWithSubs(subs);
        const durations = [];

        for (let r = 0; r < repetitions; ++r) {
            const start = performance.now();
            emitter.emit('myEvent', 42);
            const end = performance.now();
            const duration_ns = (end - start) * 1e6;
            durations.push(duration_ns);
            const time_per_sub = duration_ns / subs;
            const subs_per_sec = 1e9 / time_per_sub;

            // name,iterations,real_time,cpu_time,time_unit,"subs_per_sec","time_per_sub_ns"
            console.log(`"Node_Emit/${subs}/repeats:${repetitions}",${repetitions},${duration_ns.toFixed(0)},${duration_ns.toFixed(0)},${duration_ns.toFixed(0)},${subs_per_sec.toFixed(2)},${time_per_sub.toFixed(1)}`);
        }

        const { mean, median, stddev, cv } = stats(durations);
        const time_per_sub_mean = mean / subs;
        const subs_per_sec_mean = 1e9 / time_per_sub_mean;

        console.log(`"Node_Emit/${subs}/mean",${repetitions},${mean.toFixed(0)},${mean.toFixed(0)},${mean.toFixed(0)},${subs_per_sec_mean.toFixed(2)},${time_per_sub_mean.toFixed(1)}`);
        // console.log(`Node_Emit/${subs}/median   ${median.toFixed(0)} ns`);
        // console.log(`Node_Emit/${subs}/stddev   ${stddev.toFixed(0)} ns`);
        // console.log(`Node_Emit/${subs}/cv       ${cv.toFixed(2)} %`);
        // console.log('');
    }
}

runBenchmark([1, 10, 100, 500, 1000]);
