#pragma once

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
    auto operator()(Out out, Args&&... args) const -> Out
    {
        *out = to_tuple(std::forward<Args>(args)...);
        ++out;
        return out;
    }
};

}  // namespace detail

static constexpr inline auto output = detail::output_fn{};

}  // namespace dux
}  // namespace ferrugo
