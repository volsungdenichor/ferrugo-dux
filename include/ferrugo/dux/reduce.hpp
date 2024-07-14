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

struct output_fn
{
    template <class Iter, class... Args>
    auto operator()(Iter iter, Args&&... args) const -> Iter
    {
        *iter = to_tuple(std::forward<Args>(args)...);
        ++iter;
        return iter;
    }
};

static constexpr inline auto output = output_fn{};

struct reduce_fn
{
    template <class Reducer, class Seed, class Range>
    auto operator()(Reducer&& reducer, Seed seed, Range&& range) const -> Seed
    {
        const auto b = std::begin(range);
        const auto e = std::end(range);
        for (auto it = b; it != e; ++it)
        {
            seed = std::invoke(reducer, std::move(seed), *it);
        }
        return seed;
    }
};

static constexpr inline auto reduce = reduce_fn{};

struct transduce_fn
{
    template <class Transducer, class Reducer, class Seed, class... Ranges>
    auto operator()(Transducer&& transducer, Reducer&& reducer, Seed seed, Ranges&&... ranges) const -> Seed
    {
        return reduce(
            std::invoke(std::forward<Transducer>(transducer), std::forward<Reducer>(reducer)),
            std::move(seed),
            std::forward<Ranges>(ranges)...);
    }
};

static constexpr inline auto transduce = transduce_fn{};

struct into_fn
{
    template <class Result, class Transducer, class... Ranges>
    auto operator()(Result&& result, Transducer&& transducer, Ranges&&... ranges) const -> Result&&
    {
        transduce(std::forward<Transducer>(transducer), output, std::back_inserter(result), std::forward<Ranges>(ranges)...);
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
