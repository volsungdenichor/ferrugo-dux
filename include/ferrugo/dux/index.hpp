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
    template <class Reducer>
    struct reducer_t
    {
        Reducer m_next_reducer;
        mutable std::ptrdiff_t m_start;

        template <class State, class... Args>
        constexpr auto operator()(State state, Args&&... args) const -> State
        {
            return m_next_reducer(std::move(state), m_start++, std::forward<Args>(args)...);
        }
    };

    struct transducer_t
    {
        std::ptrdiff_t m_start;

        template <class Reducer>
        constexpr auto operator()(Reducer next_reducer) const -> reducer_t<Reducer>
        {
            return { std::move(next_reducer), m_start };
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
