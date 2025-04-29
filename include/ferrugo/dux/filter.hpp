#pragma once

#include <ferrugo/dux/interfaces.hpp>

namespace ferrugo
{
namespace dux
{
namespace detail
{
template <bool Indexed>
struct filter_fn
{
    template <bool, class Reducer, class Pred>
    struct reducer_t;

    template <class Reducer, class Pred>
    struct reducer_t<false, Reducer, Pred>
    {
        Reducer m_next_reducer;
        Pred m_pred;

        template <class State, class... Args>
        constexpr auto operator()(State state, Args&&... args) const -> State
        {
            if (std::invoke(m_pred, std::forward<Args>(args)...))
            {
                return m_next_reducer(std::move(state), std::forward<Args>(args)...);
            }
            return state;
        }
    };

    template <class Reducer, class Pred>
    struct reducer_t<true, Reducer, Pred>
    {
        Reducer m_next_reducer;
        Pred m_pred;
        mutable std::ptrdiff_t m_index = 0;

        template <class State, class... Args>
        constexpr auto operator()(State state, Args&&... args) const -> State
        {
            if (std::invoke(m_pred, m_index++, std::forward<Args>(args)...))
            {
                return m_next_reducer(std::move(state), std::forward<Args>(args)...);
            }
            return state;
        }
    };

    template <class Pred>
    struct transducer_t
    {
        Pred m_pred;

        template <class Reducer>
        constexpr auto operator()(Reducer&& next_reducer) const
            -> reducer_interface_t<reducer_t<Indexed, std::decay_t<Reducer>, Pred>>
        {
            return { { std::forward<Reducer>(next_reducer), m_pred } };
        }
    };

    template <class Pred>
    constexpr auto operator()(Pred&& pred) const -> transducer_interface_t<transducer_t<std::decay_t<Pred>>>
    {
        return { { std::forward<Pred>(pred) } };
    }
};
}  // namespace detail

static constexpr inline auto filter = detail::filter_fn<false>{};
static constexpr inline auto filter_i = detail::filter_fn<true>{};
}  // namespace dux
}  // namespace ferrugo
