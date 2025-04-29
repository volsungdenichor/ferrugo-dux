#pragma once

#include <ferrugo/dux/interfaces.hpp>

namespace ferrugo
{
namespace dux
{

namespace detail
{

struct dev_null_fn
{
    template <class State, class... Args>
    auto operator()(State state, Args&&... args) const -> State
    {
        return state;
    }
};

}  // namespace detail

static constexpr inline auto dev_null = reducer_interface_t{ detail::dev_null_fn{} };

}  // namespace dux
}  // namespace ferrugo