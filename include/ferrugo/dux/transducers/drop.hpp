#pragma once

#include <ferrugo/dux/interfaces.hpp>

namespace ferrugo
{
namespace dux
{

namespace detail
{

struct drop_fn
{
    template <class Reducer>
    struct reducer_t
    {
        Reducer m_next_reducer;
        mutable std::ptrdiff_t m_count;

        template <class State, class... Args>
        constexpr auto operator()(State state, Args&&... args) const -> State
        {
            return m_count-- <= 0 ? m_next_reducer(std::move(state), std::forward<Args>(args)...) : state;
        }
    };

    struct transducer_t
    {
        std::ptrdiff_t m_count;

        template <class Reducer>
        constexpr auto operator()(Reducer&& next_reducer) const -> reducer_interface_t<reducer_t<std::decay_t<Reducer>>>
        {
            return { { std::forward<Reducer>(next_reducer), m_count } };
        }
    };

    constexpr auto operator()(std::ptrdiff_t count) const -> transducer_interface_t<transducer_t>
    {
        return { { count } };
    }
};

}  // namespace detail

static constexpr inline auto drop = detail::drop_fn{};

}  // namespace dux
}  // namespace ferrugo
