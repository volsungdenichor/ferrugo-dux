#pragma once

#include <functional>
#include <type_traits>

namespace ferrugo
{
namespace dux
{

template <class Impl>
struct reducer_interface_t
{
    Impl m_impl;

    template <class State, class... Args>
    constexpr auto operator()(State state, Args&&... args) const -> State
    {
        return std::invoke(m_impl, std::move(state), std::forward<Args>(args)...);
    }
};

template <class Impl>
struct transducer_interface_t
{
    Impl m_impl;

    template <class Reducer>
    constexpr auto operator()(Reducer&& reducer) const -> std::invoke_result_t<Impl, Reducer>
    {
        return std::invoke(m_impl, std::forward<Reducer>(reducer));
    }

    template <class Reducer>
    constexpr auto operator|(Reducer&& reducer) const -> std::invoke_result_t<Impl, Reducer>
    {
        return std::invoke(m_impl, std::forward<Reducer>(reducer));
    }
};

template <class Impl>
reducer_interface_t(Impl&&) -> reducer_interface_t<std::decay_t<Impl>>;

template <class Impl>
transducer_interface_t(Impl&&) -> transducer_interface_t<std::decay_t<Impl>>;

}  // namespace dux
}  // namespace ferrugo
