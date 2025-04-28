#pragma once

#include <tuple>

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

}  // namespace detail

static constexpr inline auto to_tuple = detail::to_tuple_fn{};

}  // namespace dux
}  // namespace ferrugo
