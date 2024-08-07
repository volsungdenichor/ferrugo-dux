#pragma once

#include <cstddef>

namespace ferrugo
{
namespace dux
{

namespace detail
{

struct index_fn
{
    template <class Next>
    struct reducer_t
    {
        Next m_next;
        mutable std::ptrdiff_t m_start;

        template <class State, class... Args>
        constexpr auto operator()(State state, Args&&... args) const -> State
        {
            return m_next(std::move(state), m_start++, std::forward<Args>(args)...);
        }
    };

    struct transducer_t
    {
        std::ptrdiff_t m_start;

        template <class Next>
        constexpr auto operator()(Next next) const -> reducer_t<Next>
        {
            return { std::move(next), m_start };
        }
    };

    constexpr auto operator()(std::ptrdiff_t start = 0) const -> transducer_t
    {
        return { start };
    }
};

}  // namespace detail

static constexpr inline auto index = detail::index_fn{};

}  // namespace dux
}  // namespace ferrugo
