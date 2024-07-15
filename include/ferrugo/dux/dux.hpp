#pragma once

#include <ferrugo/dux/compose.hpp>
#include <ferrugo/dux/drop.hpp>
#include <ferrugo/dux/drop_while.hpp>
#include <ferrugo/dux/filter.hpp>
#include <ferrugo/dux/index.hpp>
#include <ferrugo/dux/inspect.hpp>
#include <ferrugo/dux/intersperse.hpp>
#include <ferrugo/dux/join.hpp>
#include <ferrugo/dux/reduce.hpp>
#include <ferrugo/dux/take.hpp>
#include <ferrugo/dux/take_while.hpp>
#include <ferrugo/dux/transform.hpp>
#include <ferrugo/dux/transform_maybe.hpp>

namespace ferrugo
{

namespace dux
{

static constexpr inline auto transform_i = detail::indexed_fn<detail::transform_fn>{};
static constexpr inline auto transform_maybe_i = detail::indexed_fn<detail::transform_maybe_fn>{};
static constexpr inline auto filter_i = detail::indexed_fn<detail::filter_fn>{};
static constexpr inline auto inspect_i = detail::indexed_fn<detail::inspect_fn>{};

}  // namespace dux

}  // namespace ferrugo