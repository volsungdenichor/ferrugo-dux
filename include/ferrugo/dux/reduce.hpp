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

static constexpr inline struct eq_fn
{
    template <class Iter>
    bool operator()(const Iter& lhs, const Iter& rhs) const
    {
        return call(lhs, rhs, std::make_index_sequence<std::tuple_size_v<Iter>>{});
    }

private:
    template <class Iter, std::size_t... I>
    static bool call(const Iter& lhs, const Iter& rhs, std::index_sequence<I...>)
    {
        return (... || (std::get<I>(lhs) == std::get<I>(rhs)));
    }
} eq;

template <class Iter>
void inc(Iter& iter)
{
    std::apply([](auto&... it) { (++it, ...); }, iter);
}

static constexpr inline struct invoke_reducer_fn
{
    template <class Reducer, class State, class Iter>
    auto operator()(Reducer&& reducer, State state, Iter it) const -> State
    {
        return call(
            std::forward<Reducer>(reducer), std::move(state), it, std::make_index_sequence<std::tuple_size_v<Iter>>{});
    }

private:
    template <class Reducer, class State, class Iter, std::size_t... I>
    static auto call(Reducer&& reducer, State state, Iter it, std::index_sequence<I...>) -> State
    {
        return std::invoke(std::forward<Reducer>(reducer), std::move(state), *std::get<I>(it)...);
    }
} invoke_reducer;

struct reduce_fn
{
    template <class State, class Reducer>
    struct proxy_t
    {
        State m_state;
        Reducer m_reducer;

        template <class... Ranges>
        auto operator()(Ranges&&... ranges) const -> State
        {
            State state = m_state;
            const auto begin = std::tuple{ std::begin(ranges)... };
            const auto end = std::tuple{ std::end(ranges)... };
            for (auto it = begin; !eq(it, end); inc(it))
            {
                state = invoke_reducer(m_reducer, std::move(state), it);
            }
            return state;
        }

        template <class Range>
        friend auto operator|(Range&& range, const proxy_t& proxy) -> State
        {
            return proxy(std::forward<Range>(range));
        }

        template <class... Ranges>
        friend auto operator|(const std::tuple<Ranges...>& ranges, const proxy_t& proxy) -> State
        {
            return std::apply(proxy, ranges);
        }

        template <class... Ranges>
        friend auto operator|(std::tuple<Ranges...>&& ranges, const proxy_t& proxy) -> State
        {
            return std::apply(proxy, std::move(ranges));
        }
    };

    template <class State, class Reducer>
    constexpr auto operator()(State state, Reducer&& reducer) const -> proxy_t<State, std::decay_t<Reducer>>
    {
        return { std::move(state), std::forward<Reducer>(reducer) };
    }
};

static constexpr inline auto reduce = reduce_fn{};

struct copy_fn
{
    template <class Out>
    constexpr auto operator()(Out out) const
    {
        return reduce(std::move(out), output);
    }

    template <class Out, class Transducer>
    constexpr auto operator()(Out out, Transducer&& transducer) const
    {
        return reduce(std::move(out), std::invoke(std::forward<Transducer>(transducer), output));
    }
};

static constexpr inline auto copy = copy_fn{};

struct into_fn
{
    template <class Result, class Transducer, class... Ranges>
    auto operator()(Result&& result, Transducer&& transducer, Ranges&&... ranges) const -> Result&&
    {
        copy(std::back_inserter(result), std::forward<Transducer>(transducer))(std::forward<Ranges>(ranges)...);
        return std::forward<Result>(result);
    }
};

static constexpr inline auto into = into_fn{};

}  // namespace detail

using detail::copy;
using detail::into;
using detail::reduce;

}  // namespace dux
}  // namespace ferrugo
