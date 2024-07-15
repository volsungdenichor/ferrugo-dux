#include <catch2/catch_test_macros.hpp>
#include <ferrugo/dux/dux.hpp>

#include "matchers.hpp"

namespace
{
std::string uppercase(std::string text)
{
    std::transform(text.begin(), text.end(), text.begin(), [](char ch) { return std::toupper(ch); });
    return text;
}
}  // namespace

using namespace ferrugo;

struct delimit_fn
{
    std::string m_delimiter;

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
        return total.empty() ? v : total + m_delimiter + v;
    }
};

static const inline auto delimit = delimit_fn{ ", " };

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
        dux::to_vector<int>(xform, in),
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
        dux::to_vector<int>(xform, in),
        matchers::elements_are(2, 3, 5));
}

TEST_CASE("drop", "[transducers]")
{
    const auto xform = dux::drop(3);
    const std::vector<int> in = { 2, 3, 5, 7, 9, 11, 12, 13, 14 };

    REQUIRE_THAT(  //
        dux::to_vector<int>(xform, in),
        matchers::elements_are(7, 9, 11, 12, 13, 14));
}

TEST_CASE("take_while", "[transducers]")
{
    const auto xform = dux::take_while([](int x) { return x < 10; });
    const std::vector<int> in = { 2, 3, 5, 7, 9, 11, 12, 13, 14 };

    REQUIRE_THAT(  //
        dux::to_vector<int>(xform, in),
        matchers::elements_are(2, 3, 5, 7, 9));
}

TEST_CASE("drop_while", "[transducers]")
{
    const auto xform = dux::drop_while([](int x) { return x < 10; });
    const std::vector<int> in = { 2, 3, 5, 7, 9, 11, 12, 13, 14 };

    REQUIRE_THAT(  //
        dux::to_vector<int>(xform, in),
        matchers::elements_are(11, 12, 13, 14));
}

TEST_CASE("join", "[transducers]")
{
    const auto xform = dux::join;
    const std::vector<std::string> in = { "Alpha", "Beta", "Gamma" };

    REQUIRE_THAT(  //
        dux::into(std::string{}, xform, in),
        matchers::equal_to("AlphaBetaGamma"));
}

TEST_CASE("join_with", "[transducers]")
{
    using namespace std::string_view_literals;
    const auto xform = dux::join_with(", "sv);
    const std::vector<std::string> in = { "Alpha", "Beta", "Gamma" };

    REQUIRE_THAT(  //
        dux::into(std::string{}, xform, in),
        matchers::equal_to("Alpha, Beta, Gamma"));
}
