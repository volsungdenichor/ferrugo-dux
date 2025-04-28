#pragma once

#include <ferrugo/dux/output.hpp>
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
        return transduce(std::move(out), std::forward<Transducer>(transducer), output, std::forward<Ranges>(ranges)...);
    }
};

static constexpr inline auto copy = copy_fn{};

struct into_fn
{
    template <class Result, class Transducer, class... Ranges>
    auto operator()(Result&& result, Transducer&& transducer, Ranges&&... ranges) const -> Result&&
    {
        copy(std::back_inserter(result), std::forward<Transducer>(transducer), std::forward<Ranges>(ranges)...);
        return std::forward<Result>(result);
    }
};

static constexpr inline auto into = into_fn{};

template <class State, class Reducer>
struct output_iterator
{
    using iterator_category = std::output_iterator_tag;
    using value_type = void;
    using reference = void;
    using pointer = void;
    using difference_type = void;

    State m_state;
    Reducer m_reducer;

    output_iterator& operator*()
    {
        return *this;
    }

    output_iterator& operator++()
    {
        return *this;
    }

    output_iterator operator++(int)
    {
        return *this;
    }

    template <class... Args>
    output_iterator& operator=(Args&&... args)
    {
        m_state = std::invoke(m_reducer, std::move(m_state), std::forward<Args>(args)...);
        return *this;
    }

    operator State() const
    {
        return m_state;
    }
};

struct reducer_to_output_iterator_fn
{
    template <class State, class Reducer>
    auto operator()(State state, Reducer&& reducer) const -> output_iterator<State, std::decay_t<Reducer>>
    {
        return { std::move(state), std::forward<Reducer>(reducer) };
    }
};

static constexpr inline auto reducer_to_output_iterator = detail::reducer_to_output_iterator_fn{};

}  // namespace detail

using detail::copy;
using detail::into;
using detail::reduce;
using detail::reducer_to_output_iterator;
using detail::transduce;

}  // namespace dux
}  // namespace ferrugo
