#include <catch2/catch_test_macros.hpp>
#include <ferrugo/dux/dux.hpp>
#include <optional>

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
        dux::push_back(std::vector<std::string>{}, xform, in),
        matchers::elements_are("2", "3", "5", "7", "9", "11"));

    REQUIRE_THAT(  //
        dux::transduce(xform, delimit, std::string{}, in),
        matchers::equal_to("2, 3, 5, 7, 9, 11"));
}

TEST_CASE("transform_i", "[transducers]")
{
    const auto xform = dux::transform_i([](int i, int x) { return std::to_string(i) + ". " + std::to_string(x); });
    const std::vector<int> in = { 2, 3, 5, 7, 9, 11 };

    REQUIRE_THAT(  //
        dux::push_back(std::vector<std::string>{}, xform, in),
        matchers::elements_are("0. 2", "1. 3", "2. 5", "3. 7", "4. 9", "5. 11"));

    REQUIRE_THAT(  //
        dux::transduce(xform, delimit, std::string{}, in),
        matchers::equal_to("0. 2, 1. 3, 2. 5, 3. 7, 4. 9, 5. 11"));
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

TEST_CASE("filter_i", "[transducers]")
{
    const auto xform = dux::filter_i([](int i, int x) { return i % 3 == 0 && x < 10; });
    const std::vector<int> in = { 2, 3, 5, 7, 9, 11, 12, 13, 14 };

    REQUIRE_THAT(  //
        dux::to_vector<int>(xform, in),
        matchers::elements_are(2, 7));
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

TEST_CASE("stride", "[transducers]")
{
    const auto xform = dux::stride(3);
    const std::vector<int> in = { 2, 3, 5, 7, 9, 11, 12, 13, 14 };

    REQUIRE_THAT(  //
        dux::to_vector<int>(xform, in),
        matchers::elements_are(2, 7, 12));
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
        dux::push_back(std::string{}, xform, in),
        matchers::equal_to("AlphaBetaGamma"));
}

TEST_CASE("join_with", "[transducers]")
{
    using namespace std::string_view_literals;
    const auto xform = dux::join_with(", "sv);
    const std::vector<std::string> in = { "Alpha", "Beta", "Gamma" };

    REQUIRE_THAT(  //
        dux::push_back(std::string{}, xform, in),
        matchers::equal_to("Alpha, Beta, Gamma"));
}

TEST_CASE("intersperse", "[transducers]")
{
    const auto xform = dux::intersperse(-1);
    const std::vector<int> in = { 2, 4, 6, 8 };

    REQUIRE_THAT(  //
        dux::to_vector<int>(xform, in),
        matchers::elements_are(2, -1, 4, -1, 6, -1, 8));
}

TEST_CASE("transform_maybe", "[transducers]")
{
    const auto xform = dux::transform_maybe(
        [](int x) -> std::optional<std::string>
        {
            return x % 2 == 0  //
                       ? std::optional<std::string>{ std::to_string(x) }
                       : std::optional<std::string>{};
        });
    const std::vector<int> in = { 1, 2, 3, 4, 5, 6 };

    REQUIRE_THAT(  //
        dux::to_vector<std::string>(xform, in),
        matchers::elements_are("2", "4", "6"));
}

TEST_CASE("transform_maybe_i", "[transducers]")
{
    const auto xform = dux::transform_maybe_i(
        [](int i, int x) -> std::optional<std::string>
        {
            return i % 2 == 0  //
                       ? std::optional<std::string>{ std::to_string(x) }
                       : std::optional<std::string>{};
        });
    const std::vector<int> in = { 1, 2, 3, 4, 5, 6 };

    REQUIRE_THAT(  //
        dux::to_vector<std::string>(xform, in),
        matchers::elements_are("1", "3", "5"));
}

TEST_CASE("inspect", "[transducers]")
{
    std::stringstream ss;
    const auto xform = dux::inspect([&](int x) { ss << x << " "; });

    const std::vector<int> in = { 1, 2, 3, 4, 5, 6 };

    dux::to_vector<int>(xform, in);

    REQUIRE_THAT(  //
        ss.str(),
        matchers::equal_to("1 2 3 4 5 6 "));
}

TEST_CASE("inspect_i", "[transducers]")
{
    std::stringstream ss;
    const auto xform = dux::inspect_i(
        [&](int i, int x)
        {
            if (i % 2 == 0)
            {
                ss << x << " ";
            }
            else
            {
                ss << "... ";
            }
        });

    const std::vector<int> in = { 1, 2, 3, 4, 5, 6 };

    dux::to_vector<int>(xform, in);

    REQUIRE_THAT(  //
        ss.str(),
        matchers::equal_to("1 ... 3 ... 5 ... "));
}

TEST_CASE("compose", "[transducers]")
{
    const auto xform = dux::compose(
        dux::filter([](int x) { return x % 2 == 0; }),
        dux::transform([](int x) { return std::to_string(x); }),
        dux::drop_while([](const std::string& x) { return x.size() < 2; }),
        dux::take(3));

    const std::vector<int> in = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19 };

    REQUIRE_THAT(  //
        dux::to_vector<std::string>(xform, in),
        matchers::elements_are("10", "12", "14"));
}

TEST_CASE("two inputs", "[transducers]")
{
    const auto xform = dux::transform([](int x, char y) { return std::to_string(x) + y; });

    const std::vector<int> in1 = { 2, 3, 4 };
    const std::string in2 = "ABCDEF";

    REQUIRE_THAT(  //
        dux::to_vector<std::string>(xform, in1, in2),
        matchers::elements_are("2A", "3B", "4C"));
}

TEST_CASE("three inputs", "[transducers]")
{
    const auto xform = dux::transform([](int x, char y, char z) { return std::to_string(x) + y + z; });

    const std::vector<int> in1 = { 2, 3, 4 };
    const std::string in2 = "ABCDEF";
    const std::string in3 = "+-+-+";

    REQUIRE_THAT(  //
        dux::to_vector<std::string>(xform, in1, in2, in3),
        matchers::elements_are("2A+", "3B-", "4C+"));
}
