#pragma once

#include <functional>

namespace ferrugo
{
namespace dux
{

namespace detail
{

struct to_tuple_fn
{
    template <class Arg>
    auto operator()(Arg&& arg) const -> Arg
    {
        return std::forward<Arg>(arg);
    }

    template <class... Args>
    auto operator()(Args&&... args) const -> std::tuple<Args...>
    {
        return std::forward_as_tuple(std::forward<Args>(args)...);
    }
};

static constexpr inline auto to_tuple = to_tuple_fn{};

struct output_reducer_fn
{
    template <class Out, class... Args>
    auto operator()(Out out, Args&&... args) const -> Out
    {
        *out = to_tuple(std::forward<Args>(args)...);
        ++out;
        return out;
    }
};

static constexpr inline auto output_reducer = output_reducer_fn{};

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
    template <class State, class Transducer, class Reducer, class... Ranges>
    auto operator()(State state, Transducer&& transducer, Reducer&& reducer, Ranges&&... ranges) const -> State
    {
        return reduce(
            std::move(state),
            std::invoke(std::forward<Transducer>(transducer), std::forward<Reducer>(reducer)),
            std::forward<Ranges>(ranges)...);
    }
};

static constexpr inline auto transduce = transduce_fn{};

struct copy_fn
{
    template <class Out, class Transducer, class... Ranges>
    auto operator()(Out out, Transducer&& transducer, Ranges&&... ranges) const -> Out
    {
        return transduce(
            std::move(out), std::forward<Transducer>(transducer), output_reducer, std::forward<Ranges>(ranges)...);
    }
};

static constexpr inline auto copy = copy_fn{};

struct push_back_fn
{
    template <class Result, class Transducer, class... Ranges>
    auto operator()(Result&& result, Transducer&& transducer, Ranges&&... ranges) const -> Result&&
    {
        copy(std::back_inserter(result), std::forward<Transducer>(transducer), std::forward<Ranges>(ranges)...);
        return std::forward<Result>(result);
    }
};

static constexpr inline auto push_back = push_back_fn{};

}  // namespace detail

using detail::copy;
using detail::push_back;
using detail::reduce;
using detail::transduce;

template <class T, class Transducer, class... Ranges>
auto to_vector(Transducer&& transducer, Ranges&&... ranges) -> std::vector<T>
{
    return push_back(std::vector<T>{}, std::forward<Transducer>(transducer), std::forward<Ranges>(ranges)...);
}

}  // namespace dux
}  // namespace ferrugo
