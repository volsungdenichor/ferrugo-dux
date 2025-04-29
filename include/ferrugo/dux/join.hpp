#pragma once

#include <ferrugo/dux/transducer_interface.hpp>

namespace ferrugo
{
namespace dux
{
namespace detail
{

template <class Iter, class State, class BinaryOp>
constexpr auto accumulate(Iter begin, Iter end, State state, BinaryOp&& op) -> State
{
    for (; begin != end; ++begin)
    {
        state = std::invoke(op, std::move(state), *begin);
    }
    return state;
}

struct join_with_fn
{
    template <class Reducer, class Delimiter>
    struct reducer_t
    {
        Reducer m_next_reducer;
        Delimiter m_delimiter;
        mutable bool m_first_item = true;

        template <class State, class Arg>
        constexpr auto operator()(State state, Arg&& arg) const -> State
        {
            if (!m_first_item)
            {
                state = accumulate(std::begin(m_delimiter), std::end(m_delimiter), std::move(state), m_next_reducer);
            }
            m_first_item = false;
            return accumulate(std::begin(arg), std::end(arg), std::move(state), m_next_reducer);
        }
    };

    template <class Delimiter>
    struct transducer_t
    {
        Delimiter m_delimiter;

        template <class Reducer>
        constexpr auto operator()(Reducer&& next_reducer) const -> reducer_t<std::decay_t<Reducer>, Delimiter>
        {
            return { std::forward<Reducer>(next_reducer), m_delimiter };
        }
    };

    template <class Delimiter>
    constexpr auto operator()(Delimiter&& delimiter) const -> transducer_interface_t<transducer_t<std::decay_t<Delimiter>>>
    {
        return { { std::forward<Delimiter>(delimiter) } };
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
            return accumulate(std::begin(arg), std::end(arg), std::move(state), m_next_reducer);
        }
    };

    template <class Reducer>
    constexpr auto operator()(Reducer&& next_reducer) const -> reducer_t<std::decay_t<Reducer>>
    {
        return { std::forward<Reducer>(next_reducer) };
    }
};
}  // namespace detail

static constexpr inline auto join = detail::join_fn{};
static constexpr inline auto join_with = detail::join_with_fn{};

}  // namespace dux
}  // namespace ferrugo
