#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>
#include "pubsub.h"

using namespace pubsub;

struct TestEvents {
    static constexpr auto Ping = Event<void()>();
    static constexpr auto Data = Event<void(int)>();
};

class TestSubscriber : public Subscriber {
public:
    ~TestSubscriber() override {
        unsubscribe_from_all();
    }

    int ping_count = 0;
    int data_sum = 0;

    void handlePing() { ping_count++; }
    void handleData(int val) { data_sum += val; }

    void subscribe_to(Publisher& p) override {
        store_token(p.subscribe<TestEvents::Ping>(this, &TestSubscriber::handlePing));
        store_token(p.subscribe<TestEvents::Data>(this, &TestSubscriber::handleData));
        Subscriber::subscribe_to(p);
    }

    void unsubscribe_from(Publisher& p) override {
        p.unsubscribe<TestEvents::Ping>(this);
        p.unsubscribe<TestEvents::Data>(this);
    }
};

TEST_CASE("Basic pub-sub works", "[pubsub]") {
    Publisher pub;
    TestSubscriber sub;

    sub.subscribe_to(pub);

    pub.emit<TestEvents::Ping>();
    pub.emit<TestEvents::Ping>();
    pub.emit<TestEvents::Data>(42);

    REQUIRE(sub.ping_count == 2);
    REQUIRE(sub.data_sum == 42);
}

TEST_CASE("Auto unsubscribe on destruction", "[pubsub]") {
    Publisher pub;
    {
        TestSubscriber sub;
        sub.subscribe_to(pub);
        pub.emit<TestEvents::Ping>();
        REQUIRE(sub.ping_count == 1);
    }

    REQUIRE_NOTHROW(pub.emit<TestEvents::Ping>());
}

TEST_CASE("Subscribing the same object twice does not double subscribe") {
    Publisher pub;
    TestSubscriber sub;

    sub.subscribe_to(pub);
    sub.subscribe_to(pub);

    pub.emit<TestEvents::Ping>();
    pub.emit<TestEvents::Data>(10);

    REQUIRE(sub.ping_count == 1);
    REQUIRE(sub.data_sum == 10);
}

TEST_CASE("Unsubscribing an object that was never subscribed is safe") {
    Publisher pub;
    TestSubscriber sub;

    pub.unsubscribe<TestEvents::Ping>(&sub);
    pub.emit<TestEvents::Ping>();
    REQUIRE(sub.ping_count == 0);
}

TEST_CASE("Emitting with no subscribers does nothing and is safe") {
    Publisher pub;
    REQUIRE_NOTHROW(pub.emit<TestEvents::Ping>());
    REQUIRE_NOTHROW(pub.emit<TestEvents::Data>(42));
}

TEST_CASE("Async event delivery still invokes subscribers") {
    Publisher pub;
    TestSubscriber sub;

    sub.subscribe_to(pub);
    pub.emit_thread_async<TestEvents::Data>(99);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    REQUIRE(sub.data_sum == 99);
}

TEST_CASE("Subscriber destruction triggers unsubscribe and prevents memory leaks") {
    Publisher pub;
    int call_count = 0;

    {
        class TempSubscriber : public Subscriber {
        public:
            int& call_count_ref;
            TempSubscriber(int& count) : call_count_ref(count) {}

            void subscribe_to(Publisher& pub) override {
                store_token(pub.subscribe<TestEvents::Ping>(this, &TempSubscriber::handle));
                Subscriber::subscribe_to(pub);
            }

            void unsubscribe_from(Publisher& pub) override {
                pub.unsubscribe<TestEvents::Ping>(this);
            }

            void handle() { call_count_ref++; }
        } s(call_count);

        s.subscribe_to(pub);
        pub.emit<TestEvents::Ping>();
        REQUIRE(call_count == 1);
    }

    pub.emit<TestEvents::Ping>();
    REQUIRE(call_count == 1);
}

TEST_CASE("Multiple subscribers are cleaned up without memory leaks") {
    Publisher pub;
    int total_calls = 0;

    {
        class MultiSub : public Subscriber {
        public:
            int& ref;
            MultiSub(int& total) : ref(total) {}

            void subscribe_to(Publisher& pub) override {
                store_token(pub.subscribe<TestEvents::Ping>(this, &MultiSub::on_event));
                Subscriber::subscribe_to(pub);
            }

            void unsubscribe_from(Publisher& pub) override {
                pub.unsubscribe<TestEvents::Ping>(this);
            }

            void on_event() { ref++; }
        } sub1(total_calls), sub2(total_calls);

        sub1.subscribe_to(pub);
        sub2.subscribe_to(pub);

        pub.emit<TestEvents::Ping>();
        REQUIRE(total_calls == 2);
    }

    pub.emit<TestEvents::Ping>();
    REQUIRE(total_calls == 2);
}
