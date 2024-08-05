#pragma once

#include <cstddef>

namespace ferrugo
{
namespace dux
{

namespace detail
{

struct stride_fn
{
    template <class Next>
    struct reducer_t
    {
        Next m_next;
        std::ptrdiff_t m_count;
        mutable std::ptrdiff_t m_index = 0;

        template <class State, class... Args>
        constexpr auto operator()(State state, Args&&... args) const -> State
        {
            return (m_index++ % m_count) == 0 ? m_next(std::move(state), std::forward<Args>(args)...) : state;
        }
    };

    struct transducer_t
    {
        std::ptrdiff_t m_count;

        template <class Next>
        constexpr auto operator()(Next next) const -> reducer_t<Next>
        {
            return { std::move(next), m_count, 0 };
        }
    };

    constexpr auto operator()(std::ptrdiff_t count) const -> transducer_t
    {
        return { count };
    }
};

}  // namespace detail

static constexpr inline auto stride = detail::stride_fn{};
}  // namespace dux
}  // namespace ferrugo
