#pragma once

#include <functional>

namespace ferrugo
{
namespace dux
{
namespace detail
{
struct transform_i_fn
{
    template <class Next, class Func>
    struct reducer_t
    {
        Next m_next;
        Func m_func;
        mutable std::ptrdiff_t m_index = 0;

        template <class State, class... Args>
        auto operator()(State state, Args&&... args) const -> State
        {
            return m_next(std::move(state), std::invoke(m_func, m_index++, std::forward<Args>(args)...));
        }
    };

    template <class Func>
    struct transducer_t
    {
        Func m_func;

        template <class Next>
        auto operator()(Next next) const -> reducer_t<Next, Func>
        {
            return { std::move(next), m_func };
        }
    };

    template <class Func>
    auto operator()(Func&& func) const -> transducer_t<std::decay_t<Func>>
    {
        return { std::forward<Func>(func) };
    }
};

}  // namespace detail

static constexpr inline auto transform_i = detail::transform_i_fn{};
}  // namespace dux
}  // namespace ferrugo
