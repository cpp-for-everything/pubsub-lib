const { EventEmitter } = require('events');

const emitter = new EventEmitter();
let counter = 0;

emitter.on('event', (data) => {
    counter += data;
});

console.time("Node.js pubsub");
for (let i = 0; i < 1e7; ++i) {
    emitter.emit('event', 1);
}
console.timeEnd("Node.js pubsub");
console.log("Counter:", counter);
