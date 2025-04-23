#pragma once

#include <functional>

namespace ferrugo
{
namespace dux
{
namespace detail
{

struct join_with_fn
{
    template <class Reducer, class Delimiter>
    struct reducer_t
    {
        Reducer m_next_reducer;
        Delimiter m_delimiter;
        mutable bool m_init = false;

        template <class State, class Arg>
        constexpr auto operator()(State state, Arg&& arg) const -> State
        {
            if (m_init)
            {
                for (const auto& item : m_delimiter)
                {
                    state = m_next_reducer(std::move(state), item);
                }
            }
            m_init = true;
            for (auto&& item : arg)
            {
                state = m_next_reducer(std::move(state), std::forward<decltype(item)>(item));
            }
            return state;
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

struct join_fn
{
    template <class Reducer>
    struct reducer_t
    {
        Reducer m_next_reducer;

        template <class State, class Arg>
        constexpr auto operator()(State state, Arg&& arg) const -> State
        {
            for (auto&& item : arg)
            {
                state = m_next_reducer(std::move(state), std::forward<decltype(item)>(item));
            }
            return state;
        }
    };

    template <class Reducer>
    constexpr auto operator()(Reducer next_reducer) const -> reducer_t<Reducer>
    {
        return { std::move(next_reducer) };
    }
};
}  // namespace detail

static constexpr inline auto join = detail::join_fn{};
static constexpr inline auto join_with = detail::join_with_fn{};
}  // namespace dux
}  // namespace ferrugo
