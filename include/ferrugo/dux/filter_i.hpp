#pragma once

#include <functional>

namespace ferrugo
{
namespace dux
{
namespace detail
{
struct filter_i_fn
{
    template <class Next, class Pred>
    struct reducer_t
    {
        Next m_next;
        Pred m_pred;
        mutable std::ptrdiff_t m_index = 0;

        template <class State, class... Args>
        constexpr auto operator()(State state, Args&&... args) const -> State
        {
            if (std::invoke(m_pred, m_index++, std::forward<Args>(args)...))
            {
                return m_next(std::move(state), std::forward<Args>(args)...);
            }
            return state;
        }
    };

    template <class Pred>
    struct transducer_t
    {
        Pred m_pred;

        template <class Next>
        constexpr auto operator()(Next next) const -> reducer_t<Next, Pred>
        {
            return { std::move(next), m_pred };
        }
    };

    template <class Pred>
    constexpr auto operator()(Pred&& pred) const -> transducer_t<std::decay_t<Pred>>
    {
        return { std::forward<Pred>(pred) };
    }
};
}  // namespace detail

static constexpr inline auto filter_i = detail::filter_i_fn{};
}  // namespace dux
}  // namespace ferrugo
