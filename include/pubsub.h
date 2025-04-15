// pubsub.hpp
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

#include "unique_couter.h"

/**
 * @file pubsub.hpp
 * @brief Lightweight publish-subscribe library with type-safe events and RAII subscriptions.
 */

namespace pubsub {

    /**
     * @brief Event type wrapper that uniquely identifies an event by type and a generated ID.
     *
     * @tparam HandlerT The function signature for the event handler.
     * @tparam _id A compile-time unique identifier for this event.
     */
    template<typename HandlerT, size_t _id = counter::unique_id<size_t{}, decltype([] {})>()>
    struct Event {
        using func_t = HandlerT;
        static constexpr size_t id = _id;
        constexpr Event() = default;
    };

    /**
     * @brief Concept to ensure a type conforms to the Event type.
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
        std::function<void()> unsubscribe_fn;

    public:
        SubscriptionToken() = default;

        explicit SubscriptionToken(std::function<void()> fn);
        ~SubscriptionToken();

        SubscriptionToken(const SubscriptionToken&) = delete;
        SubscriptionToken& operator=(const SubscriptionToken&) = delete;

        SubscriptionToken(SubscriptionToken&& other) noexcept;
        SubscriptionToken& operator=(SubscriptionToken&& other) noexcept;
    };

    /**
     * @brief Abstract base class for event handlers.
     */
    struct IEventHandler {
        virtual ~IEventHandler() = default;
    };

    /**
     * @brief Manages subscribers for a specific event type.
     *
     * @tparam Event The event being handled.
     */
    template<auto Event>
    class EventHandler : public IEventHandler {
        using function_type = std::function<typename decltype(Event)::func_t>;
        std::list<function_type> callbacks;
        std::unordered_map<void*, typename std::list<function_type>::iterator> ptrs;

    public:
        void subscribe(const function_type& f);

        template<typename C, typename... Args>
        void subscribe(C* obj, void (C::* mem_fn_ptr)(Args...));

        template<typename C>
        void unsubscribe(C* obj);

        template<typename... Args>
        void emit(Args... args);

        template<typename... Args>
        void emit_async(Args... args);
    };

    /**
     * @brief Central publisher that manages all event handlers.
     */
    class Publisher {
        std::unordered_map<size_t, std::unique_ptr<IEventHandler>> events;

        template<auto Event>
        EventHandler<Event>* get_handler();

    public:
        template<auto Event> requires IsEvent<decltype(Event)>
        SubscriptionToken subscribe(std::function<typename decltype(Event)::func_t> f);

        template<auto Event, typename C, typename... Args> requires IsEvent<decltype(Event)>
        SubscriptionToken subscribe(C* obj, void (C::* mem_fn)(Args...));

        template<auto Event, typename... Args> requires IsEvent<decltype(Event)>
        void emit(Args... args);

        template<auto Event, typename... Args> requires IsEvent<decltype(Event)>
        void emit_async(Args... args);

        template<auto Event, typename C> requires IsEvent<decltype(Event)>
        void unsubscribe(C* obj);
    };

    /**
     * @brief Base class for any event subscriber.
     */
    class Subscriber {
        std::list<Publisher*> publishers;
    protected:
        std::vector<SubscriptionToken> tokens;

        /**
         * @brief Stores a subscription token for automatic unsubscription.
         */
        template<typename Token>
        void store_token(Token&& t);

    public:
        virtual ~Subscriber();

        /**
         * @brief Tracks the publisher for mass unsubscription later.
         */
        virtual void subscribe_to(Publisher& p);

        /**
         * @brief Custom unsubscription logic per subscriber type.
         */
        virtual void unsubscribe_from(Publisher& p) = 0;

        /**
         * @brief Unsubscribe from all known publishers and clear tokens.
         */
        void unsubscribe_from_all();
    };

} // namespace pubsub
