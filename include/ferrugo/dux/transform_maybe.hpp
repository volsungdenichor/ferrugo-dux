#pragma once

#include <functional>

namespace ferrugo
{
namespace dux
{
namespace detail
{

template <bool Indexed>
struct transform_maybe_fn
{
    template <bool, class Reducer, class Func>
    struct reducer_t;

    template <class Reducer, class Func>
    struct reducer_t<false, Reducer, Func>
    {
        Reducer m_next_reducer;
        Func m_func;

        template <class State, class... Args>
        constexpr auto operator()(State state, Args&&... args) const -> State
        {
            if (auto res = std::invoke(m_func, std::forward<Args>(args)...))
            {
                return m_next_reducer(std::move(state), *std::move(res));
            }

            return state;
        }
    };

    template <class Reducer, class Func>
    struct reducer_t<true, Reducer, Func>
    {
        Reducer m_next_reducer;
        Func m_func;
        mutable std::ptrdiff_t m_index = 0;

        template <class State, class... Args>
        constexpr auto operator()(State state, Args&&... args) const -> State
        {
            if (auto res = std::invoke(m_func, m_index++, std::forward<Args>(args)...))
            {
                return m_next_reducer(std::move(state), *std::move(res));
            }

            return state;
        }
    };

    template <class Func>
    struct transducer_t
    {
        Func m_func;

        template <class Reducer>
        constexpr auto operator()(Reducer next_reducer) const -> reducer_t<Indexed, std::decay_t<Reducer>, Func>
        {
            return { std::forward<Reducer>(next_reducer), m_func };
        }
    };

    template <class Func>
    constexpr auto operator()(Func&& func) const -> transducer_t<std::decay_t<Func>>
    {
        return { std::forward<Func>(func) };
    }
};
}  // namespace detail

static constexpr inline auto transform_maybe = detail::transform_maybe_fn<false>{};
static constexpr inline auto transform_maybe_i = detail::transform_maybe_fn<true>{};
}  // namespace dux
}  // namespace ferrugo
