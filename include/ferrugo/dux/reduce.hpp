#pragma once

#include <ferrugo/dux/reducers/output.hpp>
#include <ferrugo/dux/to_tuple.hpp>
#include <functional>
#include <type_traits>

namespace ferrugo
{
namespace dux
{

namespace detail
{

struct eq_fn
{
    template <class Iter, std::size_t... I>
    static bool call(const Iter& lhs, const Iter& rhs, std::index_sequence<I...>)
    {
        return (... || (std::get<I>(lhs) == std::get<I>(rhs)));
    }

    template <class Iter>
    bool operator()(const Iter& lhs, const Iter& rhs) const
    {
        return call(lhs, rhs, std::make_index_sequence<std::tuple_size_v<Iter>>{});
    }
};

struct inc_fn
{
    template <class Iter, std::size_t... I>
    static void call(Iter& it, std::index_sequence<I...>)
    {
        (++std::get<I>(it), ...);
    }

    template <class Iter>
    void operator()(Iter& it) const
    {
        call(it, std::make_index_sequence<std::tuple_size_v<Iter>>{});
    }
};

static constexpr inline auto eq = eq_fn{};
static constexpr inline auto inc = inc_fn{};

struct invoke_reducer_fn
{
    template <class Reducer, class State, class Iter, std::size_t... I>
    static auto call(Reducer& reducer, State state, Iter it, std::index_sequence<I...>) -> State
    {
        return std::invoke(reducer, std::move(state), *std::get<I>(it)...);
    }

    template <class Reducer, class State, class Iter>
    auto operator()(Reducer& reducer, State state, Iter it) const -> State
    {
        return call(reducer, std::move(state), it, std::make_index_sequence<std::tuple_size_v<Iter>>{});
    }
};

static constexpr inline auto invoke_reducer = invoke_reducer_fn{};

struct reduce_fn
{
    template <class State, class Reducer, class Iter>
    static auto call(State state, Reducer&& reducer, Iter begin, Iter end) -> State
    {
        for (Iter it = begin; !eq(it, end); inc(it))
        {
            state = invoke_reducer(reducer, std::move(state), it);
        }
        return state;
    }

    template <class State, class Reducer, class... Ranges>
    auto operator()(State state, Reducer&& reducer, Ranges&&... ranges) const -> State
    {
        return call(
            std::move(state),
            std::forward<Reducer>(reducer),
            std::tuple{ std::begin(ranges)... },
            std::tuple{ std::end(ranges)... });
    }
};

static constexpr inline auto reduce = reduce_fn{};

struct transduce_fn
{
    template <class State, class Reducer, class Transducer, class... Ranges>
    auto operator()(State state, Reducer&& reducer, Transducer&& transducer, Ranges&&... ranges) const -> State
    {
        return reduce(
            std::move(state),
            std::invoke(std::forward<Transducer>(transducer), std::forward<Reducer>(reducer)),
            std::forward<Ranges>(ranges)...);
    }
};

static constexpr inline auto transduce = transduce_fn{};

struct into_fn
{
    template <class Result, class Transducer, class... Ranges>
    auto operator()(Result&& result, Transducer&& transducer, Ranges&&... ranges) const -> Result&&
    {
        transduce(std::back_inserter(result), output, std::forward<Transducer>(transducer), std::forward<Ranges>(ranges)...);
        return std::forward<Result>(result);
    }
};

static constexpr inline auto into = into_fn{};

}  // namespace detail

using detail::into;
using detail::reduce;
using detail::transduce;

}  // namespace dux
}  // namespace ferrugo
