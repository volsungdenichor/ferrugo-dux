#pragma once

#include <functional>

namespace ferrugo
{

namespace dux
{

namespace detail
{

struct intersperse_fn
{
    template <class Next, class Delimiter>
    struct reducer_t
    {
        Next m_next;
        Delimiter m_delimiter;
        mutable bool m_init = false;

        template <class State, class... Args>
        auto operator()(State state, Args&&... args) const -> State
        {
            if (m_init)
            {
                state = m_next(std::move(state), m_delimiter);
            }
            m_init = true;
            return m_next(std::move(state), std::forward<Args>(args)...);
        }
    };

    template <class Delimiter>
    struct transducer_t
    {
        Delimiter m_delimiter;

        template <class Next>
        auto operator()(Next next) const -> reducer_t<Next, Delimiter>
        {
            return { std::move(next), m_delimiter };
        }
    };

    template <class Delimiter>
    auto operator()(Delimiter&& delimiter) const -> transducer_t<std::decay_t<Delimiter>>
    {
        return { std::forward<Delimiter>(delimiter) };
    }
};
}  // namespace detail

static constexpr inline auto intersperse = detail::intersperse_fn{};
}  // namespace dux
}  // namespace ferrugo
