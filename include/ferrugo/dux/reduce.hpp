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
