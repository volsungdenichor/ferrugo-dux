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
    template <class Reducer, class Delimiter>
    struct reducer_t
    {
        Reducer m_next_reducer;
        Delimiter m_delimiter;
        mutable bool m_init = false;

        template <class State, class... Args>
        constexpr auto operator()(State state, Args&&... args) const -> State
        {
            if (m_init)
            {
                state = m_next_reducer(std::move(state), m_delimiter);
            }
            m_init = true;
            return m_next_reducer(std::move(state), std::forward<Args>(args)...);
        }
    };

    template <class Delimiter>
    struct transducer_t
    {
        Delimiter m_delimiter;

        template <class Reducer>
        constexpr auto operator()(Reducer next_reducer) const -> reducer_t<Reducer, Delimiter>
        {
            return { std::move(next_reducer), m_delimiter };
        }
    };

    template <class Delimiter>
    constexpr auto operator()(Delimiter&& delimiter) const -> transducer_t<std::decay_t<Delimiter>>
    {
        return { std::forward<Delimiter>(delimiter) };
    }
};
}  // namespace detail

static constexpr inline auto intersperse = detail::intersperse_fn{};
}  // namespace dux
}  // namespace ferrugo
