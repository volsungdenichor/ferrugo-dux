#pragma once

#include <ferrugo/dux/interfaces.hpp>
#include <type_traits>

namespace ferrugo
{
namespace dux
{
namespace detail
{

struct fork_fn
{
    template <class... Reducers>
    struct reducer_t
    {
        std::tuple<Reducers...> m_reducers;

        template <std::size_t N, class State, class... Args>
        auto call(State state, Args&&... args) const -> State
        {
            state = std::invoke(std::get<N>(m_reducers), std::move(state), args...);
            if constexpr (N + 1 < sizeof...(Reducers))
            {
                state = call<N + 1>(std::move(state), args...);
            }

            return state;
        }

        template <class State, class... Args>
        auto operator()(State state, Args&&... args) const -> State
        {
            return call<0>(std::move(state), args...);
        }
    };

    template <class... Reducers>
    constexpr auto operator()(Reducers&&... reducers) const -> reducer_interface_t<reducer_t<std::decay_t<Reducers>...>>
    {
        return { { std::tuple<std::decay_t<Reducers>...>{ std::forward<Reducers>(reducers)... } } };
    }
};

}  // namespace detail

static constexpr inline auto fork = detail::fork_fn{};

}  // namespace dux
}  // namespace ferrugo