// SPDX-License-Identifier: Apache-2.0
#pragma once
#include <functional>
#include <memory>
#include <list>
#include <unordered_map>
#include <concepts>
#include <type_traits>
#include <typeindex>
#include <iostream>
#include <future>
#include <execution>
#include <algorithm>

#ifdef WITH_TBB
    #include <tbb/parallel_for_each.h>
#endif

#include "unique_couter.h"

/**
 * @file pubsub.hpp
 * @brief Lightweight publish-subscribe library with type-safe events and RAII subscriptions.
 */

namespace pubsub {

    /**
     * @brief Compile-time event representation, associated with a handler signature and unique ID.
     * @tparam HandlerT Function signature for event callbacks.
     * @tparam _id Unique ID assigned to differentiate between identical function signatures.
     */
    template<typename HandlerT, size_t _id = counter::unique_id<size_t{}, decltype([] {})>()>
    struct Event {
        using func_t = HandlerT; ///< Function type associated with this event.
        static constexpr size_t id = _id; ///< Unique ID for this event.
        constexpr Event() = default;
    };

    /**
     * @brief Create a new event with a unique ID.
     * @tparam HandlerT Event handler function signature.
     * @tparam _id Automatically generated unique identifier.
     */
    template<typename HandlerT, size_t _id = counter::unique_id<size_t{}, decltype([] {})>()>
    Event<HandlerT, _id> make_event() { return {}; }

    /**
     * @brief Concept used to constrain valid Event types.
     */
    template <typename EventT>
    concept IsEvent = requires {
        typename EventT::func_t;
        { EventT::id } -> std::convertible_to<size_t>;
    };

    /**
     * @brief RAII token used to automatically unsubscribe when destroyed.
     */
    class SubscriptionToken {
        std::function<void()> unsubscribe_fn; ///< Internal function to unsubscribe.

    public:
        SubscriptionToken() = default;

        /**
         * @brief Construct with unsubscription logic.
         * @param fn Function to call when token is destroyed.
         */
        explicit SubscriptionToken(std::function<void()> fn) : unsubscribe_fn(std::move(fn)) {}

        /**
         * @brief Destructor automatically calls the unsubscription function.
         */
        ~SubscriptionToken() {
            if (unsubscribe_fn) unsubscribe_fn();
        }

        SubscriptionToken(const SubscriptionToken&) = delete;
        SubscriptionToken& operator=(const SubscriptionToken&) = delete;

        /**
         * @brief Move constructor.
         */
        SubscriptionToken(SubscriptionToken&& other) noexcept : unsubscribe_fn(std::move(other.unsubscribe_fn)) {
            other.unsubscribe_fn = nullptr;
        }

        /**
         * @brief Move assignment operator.
         */
        SubscriptionToken& operator=(SubscriptionToken&& other) noexcept {
            if (this != &other) {
                unsubscribe_fn = std::move(other.unsubscribe_fn);
                other.unsubscribe_fn = nullptr;
            }
            return *this;
        }
    };

    /**
     * @brief Interface for type-erased event handlers.
     */
    struct IEventHandler {
        virtual ~IEventHandler() = default;
    };

    /**
     * @brief Type-safe handler list for a given event.
     * @tparam Event Event descriptor.
     */
    template<auto Event>
    class EventHandler : public IEventHandler {
        using function_type = std::function<typename decltype(Event)::func_t>;
        std::list<function_type> callbacks;
        std::unordered_map<void*, typename std::list<function_type>::iterator> ptrs;

    public:
        /**
         * @brief Add a free-function/lambda callback.
         */
        void subscribe(const function_type& f) {
            callbacks.push_back(f);
        }

        /**
         * @brief Add a member function callback from an object.
         */
        template<typename C, typename... Args>
        void subscribe(C* obj, void (C::* mem_fn_ptr)(Args...)) {
            if (ptrs.contains(obj)) return;
            function_type f = [obj, mem_fn_ptr](Args... args) noexcept {
                ((*obj).*mem_fn_ptr)(args...);
            };
            ptrs[obj] = callbacks.insert(callbacks.end(), f);
        }

        /**
         * @brief Unsubscribe a previously subscribed object.
         */
        template<typename C>
        void unsubscribe(C* obj) {
            if (ptrs.contains(obj)) {
                callbacks.erase(ptrs[obj]);
                ptrs.erase(obj);
            }
        }

        /**
         * @brief Emit an event synchronously.
         */
        template<typename... Args>
        [[nodiscard]] bool emit(Args... args) {
            bool success = true;
            for (const auto& cb : callbacks) {
                try {
                    cb(args...);
                }
                catch(...) {
                    success = false;
                }  
            }
            return success;
        }

        /**
         * @brief Emit an event asynchronously.
         * @todo Implement and use thread pool with coroutine 
         * that takes the callbacks and their arguments from 
         * a queue and calls them when a thread in the pool is free
         */
        template<typename... Args>
        [[nodiscard]] bool emit_thread_async(Args... args) {
            bool success = true;
            std::vector<std::future<void>> futures;
            futures.reserve(callbacks.size());
            for (const auto& cb : callbacks) {
                futures.emplace_back(std::async(std::launch::async, cb, args...));
            }
            return success;
        }

#ifdef WITH_TBB
        /**
         * @brief Emit an event asynchronously using oneTBB.
         */
        template<typename... Args>
        [[nodiscard]] bool emit_tbb_async(Args... args) {
            bool success = true;
            tbb::parallel_for_each(callbacks.begin(), callbacks.end(), [&](const auto& cb) {
                try {
                    cb(args...);
                }
                catch(...) {
                    success = false;
                }
            });
            return success;
        }
#endif

#if defined(__cpp_lib_execution)
        /**
         * @brief Emit an event asynchronously using <execution>.
         */
        template<typename ExecutionPolicy, typename... Args, typename = std::is_execution_policy<ExecutionPolicy>>
        [[nodiscard]] bool emit_async(ExecutionPolicy policy, Args... args) {
            bool success = true;
            std::for_each(policy, callbacks.begin(), callbacks.end(),
                [&](const auto& cb) {
                    try {
                        cb(args...);
                    }
                    catch(...) {
                        success = false;
                    }
                }
            );
            return success;
        }
#endif
    };

    /**
     * @brief Central publisher that manages event handlers and distributes events.
     */
    class Publisher {
        std::unordered_map<size_t, std::unique_ptr<IEventHandler>> events; ///< All registered handlers.

        template<auto Event>
        EventHandler<Event>* get_handler() {
            auto it = events.find(decltype(Event)::id);
            if (it == events.end()) {
                auto handler = std::make_unique<EventHandler<Event>>();
                auto* raw = handler.get();
                events[decltype(Event)::id] = std::move(handler);
                return raw;
            }
            return static_cast<EventHandler<Event>*>(events[decltype(Event)::id].get());
        }

    public:
        /**
         * @brief Subscribe a free function or lambda.
         */
        template<auto Event> requires IsEvent<decltype(Event)>
        SubscriptionToken subscribe(std::function<typename decltype(Event)::func_t> f) {
            auto* handler = get_handler<Event>();
            handler->subscribe(f);
            return SubscriptionToken([handler]() {}); // Lambdas not tracked
        }

        /**
         * @brief Subscribe a member function from an object.
         */
        template<auto Event, typename C, typename... Args> requires IsEvent<decltype(Event)>
        SubscriptionToken subscribe(C* obj, void (C::* mem_fn)(Args...)) {
            auto* handler = get_handler<Event>();
            handler->subscribe(obj, mem_fn);
            return SubscriptionToken([handler, obj]() {
                handler->unsubscribe(obj);
            });
        }

        /**
         * @brief Emit an event synchronously to all listeners.
         */
        template<auto Event, typename... Args> requires IsEvent<decltype(Event)>
        bool emit(Args... args) {
            return get_handler<Event>()->emit(args...);
        }

        /**
         * @brief Emit an event asynchronously to all listeners.
         */
        template<auto Event, typename... Args> requires IsEvent<decltype(Event)>
        bool emit_thread_async(Args... args) {
            return get_handler<Event>()->emit_thread_async(args...);
        }

#ifdef WITH_TBB
        /**
         * @brief Emit an event asynchronously using oneTBB to all listeners.
         */
        template<auto Event, typename... Args> requires IsEvent<decltype(Event)>
        bool emit_tbb_async(Args... args) {
            return get_handler<Event>()->emit_tbb_async(args...);
        }
#else
        #warning TBB not available
#endif

#if defined(__cpp_lib_execution)
        /**
         * @brief Emit an event asynchronously to all listeners.
         */
        template<auto Event, typename... Args> requires IsEvent<decltype(Event)>
        bool emit_async(Args... args) {
            return get_handler<Event>()->emit_async(args...);
        }
#else
        #warning std::execution not available
#endif

        /**
         * @brief Unsubscribe an object from the event.
         */
        template<auto Event, typename C> requires IsEvent<decltype(Event)>
        void unsubscribe(C* obj) {
            auto it = events.find(decltype(Event)::id);
            if (it != events.end()) {
                static_cast<EventHandler<Event>*>(it->second.get())->unsubscribe(obj);
            }
        }
    };

    /**
     * @brief Base class for lifetime-aware event subscribers.
     */
    class Subscriber {
        std::list<Publisher*> publishers; ///< All known publishers.
    protected:
        std::vector<SubscriptionToken> tokens; ///< Stores subscription tokens for RAII unsubscription.

        /**
         * @brief Internally track a token.
         */
        template<typename Token>
        void store_token(Token&& t) {
            tokens.emplace_back(std::forward<Token>(t));
        }

    public:
        /**
         * @brief Automatically unsubscribes on destruction.
         */
        virtual ~Subscriber() = default;

        /**
         * @brief Tracks a publisher.
         */
        virtual void subscribe_to(Publisher& p) {
            publishers.push_back(&p);
        }

        /**
         * @brief Subclass must define how to unsubscribe from a publisher.
         */
        virtual void unsubscribe_from(Publisher& p) = 0;

        /**
         * @brief Unsubscribes from all known publishers.
         */
        void unsubscribe_from_all() {
            for (auto& pub : publishers) {
                unsubscribe_from(*pub);
            }
            publishers.clear();
            tokens.clear();
        }
    };

} // namespace pubsub
