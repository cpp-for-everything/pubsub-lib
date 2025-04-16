from time import time

class PubSub:
    def __init__(self):
        self.subscribers = {}

    def subscribe(self, event, callback):
        if event not in self.subscribers:
            self.subscribers[event] = []
        self.subscribers[event].append(callback)

    def publish(self, event, data):
        for cb in self.subscribers.get(event, []):
            cb(data)

counter = 0
def handler(data):
    global counter
    counter += data

pubsub = PubSub()
pubsub.subscribe("event", handler)

start = time()
for _ in range(10_000_000):
    pubsub.publish("event", 1)
end = time()

print("Python pubsub: {:.2f} sec".format(end - start))
print("Counter:", counter)
print("Time per publish: {:.4f} nanosec".format((end - start) / counter * 10e7))
