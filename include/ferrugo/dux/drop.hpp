#pragma once

#include <cstddef>

namespace ferrugo
{
namespace dux
{

namespace detail
{

struct drop_fn
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
                return state;
            }
            return m_next(state, std::forward<Args>(args)...);
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

static constexpr inline auto drop = detail::drop_fn{};
}  // namespace dux
}  // namespace ferrugo
