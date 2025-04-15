#pragma once
#include <type_traits>

namespace counter {
    template<size_t Id>
    struct counter {
        using tag = counter;

        struct generator {
            friend constexpr bool is_defined(tag) // The line that might cause a warning
            {
                return true;
            }
        };
        friend constexpr bool is_defined(tag);

        template<typename Tag = tag, bool = is_defined(Tag{}) >
        static constexpr bool exists(auto)
        {
            return true;
        }

        static constexpr bool exists(...)
        {
            return generator(), false;
        }
    };

    template<size_t Id = 0, typename = decltype([] {}) >
    constexpr size_t unique_id() {
        if constexpr (not counter<Id>::exists(Id)) return Id;
        else return unique_id < Id + 1, decltype([] {}) > ();
    }
}
