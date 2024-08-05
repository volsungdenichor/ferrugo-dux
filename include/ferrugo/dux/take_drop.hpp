#pragma once

#include <cstddef>

namespace ferrugo
{
namespace dux
{

namespace detail
{

template <bool Type>
struct take_drop_fn
{
    template <class Next>
    struct reducer_t
    {
        Next m_next;
        mutable std::ptrdiff_t m_count;

        template <class State, class... Args>
        constexpr auto operator()(State state, Args&&... args) const -> State
        {
            const auto done = m_count-- <= 0;
            return done == Type ? m_next(std::move(state), std::forward<Args>(args)...) : state;
        }
    };

    struct transducer_t
    {
        std::ptrdiff_t m_count;

        template <class Next>
        constexpr auto operator()(Next next) const -> reducer_t<Next>
        {
            return { std::move(next), m_count };
        }
    };

    constexpr auto operator()(std::ptrdiff_t count) const -> transducer_t
    {
        return { count };
    }
};

}  // namespace detail

static constexpr inline auto take = detail::take_drop_fn<false>{};
static constexpr inline auto drop = detail::take_drop_fn<true>{};
}  // namespace dux
}  // namespace ferrugo
