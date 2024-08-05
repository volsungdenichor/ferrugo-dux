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
        auto it = std::begin(range);
        const auto e = std::end(range);
        for (; it != e; ++it)
        {
            seed = std::invoke(reducer, std::move(seed), *it);
        }
        return seed;
    }

    template <class Reducer, class Seed, class Range1, class Range2>
    auto operator()(Reducer&& reducer, Seed seed, Range1&& range1, Range2&& range2) const -> Seed
    {
        auto it1 = std::begin(range1);
        auto it2 = std::begin(range2);
        const auto e1 = std::end(range1);
        const auto e2 = std::end(range2);
        for (; it1 != e1 && it2 != e2; ++it1, ++it2)
        {
            seed = std::invoke(reducer, std::move(seed), *it1, *it2);
        }
        return seed;
    }

    template <class Reducer, class Seed, class Range1, class Range2, class Range3>
    auto operator()(Reducer&& reducer, Seed seed, Range1&& range1, Range2&& range2, Range3&& range3) const -> Seed
    {
        auto it1 = std::begin(range1);
        auto it2 = std::begin(range2);
        auto it3 = std::begin(range3);
        const auto e1 = std::end(range1);
        const auto e2 = std::end(range2);
        const auto e3 = std::end(range3);
        for (; it1 != e1 && it2 != e2 && it3 != e3; ++it1, ++it2, ++it3)
        {
            seed = std::invoke(reducer, std::move(seed), *it1, *it2, *it3);
        }
        return seed;
    }

    template <class Reducer, class Seed, class Range1, class Range2, class Range3, class Range4>
    auto operator()(Reducer&& reducer, Seed seed, Range1&& range1, Range2&& range2, Range3&& range3, Range4&& range4) const
        -> Seed
    {
        auto it1 = std::begin(range1);
        auto it2 = std::begin(range2);
        auto it3 = std::begin(range3);
        auto it4 = std::begin(range4);
        const auto e1 = std::end(range1);
        const auto e2 = std::end(range2);
        const auto e3 = std::end(range3);
        const auto e4 = std::end(range4);
        for (; it1 != e1 && it2 != e2 && it3 != e3 && it4 != e4; ++it1, ++it2, ++it3, ++it4)
        {
            seed = std::invoke(reducer, std::move(seed), *it1, *it2, *it3, *it4);
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

struct copy_fn
{
    template <class Out, class Transducer, class... Ranges>
    auto operator()(Out out, Transducer&& transducer, Ranges&&... ranges) const -> Out
    {
        return transduce(std::forward<Transducer>(transducer), output, std::move(out), std::forward<Ranges>(ranges)...);
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
