#pragma once

#include <ferrugo/dux/interfaces.hpp>
#include <tuple>

namespace ferrugo
{
namespace dux
{

namespace detail
{

template <class... Pipes>
struct compose_t
{
    std::tuple<Pipes...> m_pipes;

    constexpr compose_t(std::tuple<Pipes...> pipes) : m_pipes{ std::move(pipes) }
    {
    }

private:
    template <std::size_t I, class... Args>
    constexpr auto invoke(Args&&... args) const -> decltype(std::invoke(std::get<I>(m_pipes), std::forward<Args>(args)...))
    {
        return std::invoke(std::get<I>(m_pipes), std::forward<Args>(args)...);
    }

    template <std::size_t I, class... Args, std::enable_if_t<(I == 0), int> = 0>
    constexpr auto call(Args&&... args) const -> decltype(invoke<0>(std::forward<Args>(args)...))
    {
        return invoke<0>(std::forward<Args>(args)...);
    }

    template <std::size_t I, class... Args, std::enable_if_t<(I > 0), int> = 0>
    constexpr auto call(Args&&... args) const -> decltype(call<I - 1>(invoke<I>(std::forward<Args>(args)...)))
    {
        return call<I - 1>(invoke<I>(std::forward<Args>(args)...));
    }

public:
    template <class... Args>
    constexpr auto operator()(Args&&... args) const -> decltype(call<sizeof...(Pipes) - 1>(std::forward<Args>(args)...))
    {
        return call<sizeof...(Pipes) - 1>(std::forward<Args>(args)...);
    }
};

struct compose_fn
{
private:
    template <class Pipe>
    static constexpr auto to_tuple(Pipe pipe) -> std::tuple<Pipe>
    {
        return std::tuple<Pipe>{ std::move(pipe) };
    }

    template <class... Pipes>
    static constexpr auto to_tuple(compose_t<Pipes...> pipe) -> std::tuple<Pipes...>
    {
        return pipe.m_pipes;
    }

    template <class... Pipes>
    static constexpr auto from_tuple(std::tuple<Pipes...> tuple) -> compose_t<Pipes...>
    {
        return compose_t<Pipes...>{ std::move(tuple) };
    }

public:
    template <class... Pipes>
    constexpr auto operator()(Pipes&&... pipes) const
        -> decltype(from_tuple(std::tuple_cat(to_tuple(std::forward<Pipes>(pipes))...)))
    {
        return from_tuple(std::tuple_cat(to_tuple(std::forward<Pipes>(pipes))...));
    }
};

}  // namespace detail

static constexpr inline auto compose = detail::compose_fn{};

template <class L, class R>
constexpr auto operator|(transducer_interface_t<L> lhs, transducer_interface_t<R> rhs)
{
    return transducer_interface_t{ compose(std::move(lhs.m_impl), std::move(rhs.m_impl)) };
}

}  // namespace dux
}  // namespace ferrugo
