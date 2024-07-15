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
    template <class Next, class Delimiter>
    struct reducer_t
    {
        Next m_next;
        Delimiter m_delimiter;
        mutable bool m_init = false;

        template <class State, class Arg>
        auto operator()(State state, Arg&& arg) const -> State
        {
            if (m_init)
            {
                for (const auto& item : m_delimiter)
                {
                    state = m_next(std::move(state), item);
                }
            }
            m_init = true;
            for (auto&& item : arg)
            {
                state = m_next(std::move(state), std::forward<decltype(item)>(item));
            }
            return state;
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

struct join_fn
{
    template <class Next>
    struct reducer_t
    {
        Next m_next;

        template <class State, class Arg>
        auto operator()(State state, Arg&& arg) const -> State
        {
            for (auto&& item : arg)
            {
                state = m_next(std::move(state), std::forward<decltype(item)>(item));
            }
            return state;
        }
    };

    template <class Next>
    auto operator()(Next next) const -> reducer_t<Next>
    {
        return { std::move(next) };
    }
};
}  // namespace detail

static constexpr inline auto join = detail::join_fn{};
static constexpr inline auto join_with = detail::join_with_fn{};
}  // namespace dux
}  // namespace ferrugo
