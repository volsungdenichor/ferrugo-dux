#pragma once

#include <ferrugo/dux/interfaces.hpp>
#include <ferrugo/dux/to_tuple.hpp>

namespace ferrugo
{
namespace dux
{
namespace detail
{

struct output_fn
{
    template <class Out, class... Args>
    constexpr auto operator()(Out out, Args&&... args) const -> Out
    {
        *out = to_tuple(std::forward<Args>(args)...);
        ++out;
        return out;
    }
};

}  // namespace detail

static constexpr inline auto output = reducer_interface_t{ detail::output_fn{} };

}  // namespace dux
}  // namespace ferrugo
