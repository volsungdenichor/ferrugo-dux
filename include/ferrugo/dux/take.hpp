#pragma once

#include <cstddef>

namespace ferrugo
{
namespace dux
{

namespace detail
{

struct take_fn
{
    template <class Next>
    struct reducer_t
    {
        Next m_next;
        mutable std::ptrdiff_t m_count;

        template <class State, class... Args>
        auto operator()(State state, Args&&... args) const -> State
        {
            if (m_count-- > 0)
            {
                return m_next(state, std::forward<Args>(args)...);
            }
            return state;
        }
    };

    struct transducer_t
    {
        std::ptrdiff_t m_count;

        template <class Next>
        auto operator()(Next next) const -> reducer_t<Next>
        {
            return { std::move(next), m_count };
        }
    };

    auto operator()(std::ptrdiff_t count) const -> transducer_t
    {
        return { count };
    }
};

}  // namespace detail

static constexpr inline auto take = detail::take_fn{};
}  // namespace dux
}  // namespace ferrugo