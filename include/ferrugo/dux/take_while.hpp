#pragma once

#include <functional>

namespace ferrugo
{
namespace dux
{
namespace detail
{

struct take_while_fn
{
    template <class Reducer, class Pred>
    struct reducer_t
    {
        Reducer m_next_reducer;
        Pred m_pred;
        mutable bool m_done = false;

        template <class State, class... Args>
        constexpr auto operator()(State state, Args&&... args) const -> State
        {
            m_done = m_done || !std::invoke(m_pred, args...);
            return !m_done ? m_next_reducer(std::move(state), std::forward<Args>(args)...) : state;
        }
    };

    template <class Pred>
    struct transducer_t
    {
        Pred m_pred;

        template <class Reducer>
        constexpr auto operator()(Reducer next_reducer) const -> reducer_t<Reducer, Pred>
        {
            return { std::move(next_reducer), m_pred };
        }
    };

    template <class Pred>
    constexpr auto operator()(Pred&& pred) const -> transducer_t<std::decay_t<Pred>>
    {
        return { std::forward<Pred>(pred) };
    }
};
}  // namespace detail

static constexpr inline auto take_while = detail::take_while_fn{};
}  // namespace dux
}  // namespace ferrugo
