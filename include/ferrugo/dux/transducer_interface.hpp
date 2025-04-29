#pragma once

#include <functional>
#include <type_traits>

namespace ferrugo
{
namespace dux
{
template <class Impl>
struct transducer_interface_t
{
    Impl m_impl;

    template <class Reducer>
    constexpr auto operator()(Reducer&& reducer) const -> std::invoke_result_t<Impl, Reducer>
    {
        return std::invoke(m_impl, std::forward<Reducer>(reducer));
    }
};

template <class Impl>
transducer_interface_t(Impl&&) -> transducer_interface_t<std::decay_t<Impl>>;

}  // namespace dux
}  // namespace ferrugo
