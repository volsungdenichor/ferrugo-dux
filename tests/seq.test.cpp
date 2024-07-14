#include <catch2/catch_test_macros.hpp>
#include <ferrugo/dux/dux.hpp>

#include "matchers.hpp"

using namespace ferrugo;

struct delimit_fn
{
    template <class T>
    auto operator()(std::string total, const T& item) const -> std::string
    {
        const auto v = std::invoke(
            [&]()
            {
                std::stringstream ss;
                ss << item;
                return ss.str();
            });
        return total.empty() ? v : total + ", " + v;
    }
};

static constexpr inline auto delimit = delimit_fn{};

TEST_CASE("transform", "[transducers]")
{
    const auto xform = dux::transform([](int x) { return std::to_string(x); });
    const std::vector<int> in = { 2, 3, 5, 7, 9, 11 };

    REQUIRE_THAT(  //
        dux::into(std::vector<std::string>{}, xform, in),
        matchers::elements_are("2", "3", "5", "7", "9", "11"));

    REQUIRE_THAT(  //
        dux::transduce(xform, delimit, std::string{}, in),
        matchers::equal_to("2, 3, 5, 7, 9, 11"));
}

TEST_CASE("filter", "[transducers]")
{
    const auto xform = dux::filter([](int x) { return x % 2 == 0; });
    const std::vector<int> in = { 2, 3, 5, 7, 9, 11, 12, 13, 14 };

    REQUIRE_THAT(  //
        dux::into(std::vector<int>{}, xform, in),
        matchers::elements_are(2, 12, 14));

    REQUIRE_THAT(  //
        dux::transduce(xform, std::plus<>{}, 0, in),
        matchers::equal_to(28));
}

TEST_CASE("take", "[transducers]")
{
    const auto xform = dux::take(3);
    const std::vector<int> in = { 2, 3, 5, 7, 9, 11, 12, 13, 14 };

    REQUIRE_THAT(  //
        dux::into(std::vector<int>{}, xform, in),
        matchers::elements_are(2, 3, 5));
}

TEST_CASE("drop", "[transducers]")
{
    const auto xform = dux::drop(3);
    const std::vector<int> in = { 2, 3, 5, 7, 9, 11, 12, 13, 14 };

    REQUIRE_THAT(  //
        dux::into(std::vector<int>{}, xform, in),
        matchers::elements_are(7, 9, 11, 12, 13, 14));
}