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

struct str_fn
{
    template <class... Args>
    std::string operator()(const Args&... args) const
    {
        std::stringstream ss;
        (ss << ... << args);
        return ss.str();
    }
};

static constexpr inline auto str = str_fn{};

struct delimit
{
    std::string m_delimiter;

    template <class T>
    auto operator()(std::string total, const T& item) const -> std::string
    {
        const std::string v = str(item);
        return total.empty() ? v : total + m_delimiter + v;
    }
};

static constexpr inline struct min_value_fn
{
    template <class T>
    constexpr const T& operator()(const T& lhs, const T& rhs) const
    {
        return std::min(lhs, rhs);
    }
} min_value;

static constexpr inline struct max_value_fn
{
    template <class T>
    constexpr const T& operator()(const T& lhs, const T& rhs) const
    {
        return std::max(lhs, rhs);
    }
} max_value;

TEST_CASE("reduce", "[reducers]")
{
    const std::vector<int> in = { 2, 3, 5, 7, 9, 11, 12, 13, 14 };

    REQUIRE_THAT(  //
        dux::reduce(0, std::plus{})(in),
        matchers::equal_to(76));

    REQUIRE_THAT(  //
        in | dux::reduce(0, std::plus{}),
        matchers::equal_to(76));

    REQUIRE_THAT(  //
        dux::reduce(1, std::multiplies{})(in),
        matchers::equal_to(45'405'360));

    REQUIRE_THAT(  //
        in | dux::reduce(1, std::multiplies{}),
        matchers::equal_to(45'405'360));

    REQUIRE_THAT(  //
        dux::reduce(std::numeric_limits<int>::max(), min_value)(in),
        matchers::equal_to(2));

    REQUIRE_THAT(  //
        dux::reduce(std::numeric_limits<int>::min(), max_value)(in),
        matchers::equal_to(14));
}

TEST_CASE("transform", "[transducers]")
{
    const auto xform = dux::transform(str);
    const std::vector<int> in = { 2, 3, 5, 7, 9, 11 };

    REQUIRE_THAT(  //
        dux::into(std::vector<std::string>{}, xform, in),
        matchers::elements_are("2", "3", "5", "7", "9", "11"));

    REQUIRE_THAT(  //
        dux::into(std::vector<std::string>{}, xform, in),
        matchers::elements_are("2", "3", "5", "7", "9", "11"));

    REQUIRE_THAT(  //
        dux::reduce(std::string{}, xform | delimit{ ", " })(in),
        matchers::equal_to("2, 3, 5, 7, 9, 11"));
}

TEST_CASE("transform_i", "[transducers]")
{
    const auto xform = dux::transform_i([](int i, int x) { return str(i, ". ", x); });
    const std::vector<int> in = { 2, 3, 5, 7, 9, 11 };

    REQUIRE_THAT(  //
        dux::into(std::vector<std::string>{}, xform, in),
        matchers::elements_are("0. 2", "1. 3", "2. 5", "3. 7", "4. 9", "5. 11"));

    REQUIRE_THAT(  //
        dux::reduce(std::string{}, xform | delimit{ ", " })(in),
        matchers::equal_to("0. 2, 1. 3, 2. 5, 3. 7, 4. 9, 5. 11"));

    REQUIRE_THAT(  //
        in | dux::reduce(std::string{}, xform | delimit{ ", " }),
        matchers::equal_to("0. 2, 1. 3, 2. 5, 3. 7, 4. 9, 5. 11"));
}

TEST_CASE("filter", "[transducers]")
{
    const auto xform = dux::filter([](int x) { return x % 2 == 0; });
    const std::vector<int> in = { 2, 3, 5, 7, 9, 11, 12, 13, 14 };

    REQUIRE_THAT(  //
        dux::into(std::vector<int>{}, xform, in),
        matchers::elements_are(2, 12, 14));

    REQUIRE_THAT(  //
        dux::reduce(0, xform | std::plus{})(in),
        matchers::equal_to(28));

    REQUIRE_THAT(  //
        in | dux::reduce(0, xform | std::plus{}),
        matchers::equal_to(28));
}

TEST_CASE("filter_i", "[transducers]")
{
    const auto xform = dux::filter_i([](int i, int x) { return i % 3 == 0 && x < 10; });
    const std::vector<int> in = { 2, 3, 5, 7, 9, 11, 12, 13, 14 };

    REQUIRE_THAT(  //
        dux::into(std::vector<int>{}, xform, in),
        matchers::elements_are(2, 7));
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

TEST_CASE("stride", "[transducers]")
{
    const auto xform = dux::stride(3);
    const std::vector<int> in = { 2, 3, 5, 7, 9, 11, 12, 13, 14 };

    REQUIRE_THAT(  //
        dux::into(std::vector<int>{}, xform, in),
        matchers::elements_are(2, 7, 12));
}

TEST_CASE("take_while", "[transducers]")
{
    const auto xform = dux::take_while([](int x) { return x < 10; });
    const std::vector<int> in = { 2, 3, 5, 7, 9, 11, 12, 13, 14 };

    REQUIRE_THAT(  //
        dux::into(std::vector<int>{}, xform, in),
        matchers::elements_are(2, 3, 5, 7, 9));
}

TEST_CASE("drop_while", "[transducers]")
{
    const auto xform = dux::drop_while([](int x) { return x < 10; });
    const std::vector<int> in = { 2, 3, 5, 7, 9, 11, 12, 13, 14 };

    REQUIRE_THAT(  //
        dux::into(std::vector<int>{}, xform, in),
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

TEST_CASE("intersperse", "[transducers]")
{
    const auto xform = dux::intersperse(-1);
    const std::vector<int> in = { 2, 4, 6, 8 };

    REQUIRE_THAT(  //
        dux::into(std::vector<int>{}, xform, in),
        matchers::elements_are(2, -1, 4, -1, 6, -1, 8));
}

TEST_CASE("transform_maybe", "[transducers]")
{
    const auto xform = dux::transform_maybe(
        [](int x) -> std::optional<std::string>
        {
            return x % 2 == 0  //
                       ? std::optional<std::string>{ str(x) }
                       : std::optional<std::string>{};
        });
    const std::vector<int> in = { 1, 2, 3, 4, 5, 6 };

    REQUIRE_THAT(  //
        dux::into(std::vector<std::string>{}, xform, in),
        matchers::elements_are("2", "4", "6"));
}

TEST_CASE("transform_maybe_i", "[transducers]")
{
    const auto xform = dux::transform_maybe_i(
        [](int i, int x) -> std::optional<std::string>
        {
            return i % 2 == 0  //
                       ? std::optional<std::string>{ str(x) }
                       : std::optional<std::string>{};
        });
    const std::vector<int> in = { 1, 2, 3, 4, 5, 6 };

    REQUIRE_THAT(  //
        dux::into(std::vector<std::string>{}, xform, in),
        matchers::elements_are("1", "3", "5"));
}

TEST_CASE("inspect", "[transducers]")
{
    std::stringstream ss;
    const auto xform = dux::inspect([&](int x) { ss << x << " "; });

    const std::vector<int> in = { 1, 2, 3, 4, 5, 6 };

    dux::into(std::vector<int>{}, xform, in);

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

    dux::into(std::vector<int>{}, xform, in);

    REQUIRE_THAT(  //
        ss.str(),
        matchers::equal_to("1 ... 3 ... 5 ... "));
}

TEST_CASE("compose", "[transducers]")
{
    const auto xform = dux::compose(
        dux::filter([](int x) { return x % 2 == 0; }),
        dux::transform(str),
        dux::drop_while([](const std::string& x) { return x.size() < 2; }),
        dux::take(3));

    const std::vector<int> in = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19 };

    REQUIRE_THAT(  //
        dux::into(std::vector<std::string>{}, xform, in),
        matchers::elements_are("10", "12", "14"));
}

TEST_CASE("compose transducers with pipe operator", "[transducers]")
{
    const auto xform = dux::filter([](int x) { return x % 2 == 0; })                         //
                       | dux::transform(str)                                                 //
                       | dux::drop_while([](const std::string& x) { return x.size() < 2; })  //
                       | dux::take(3);

    const std::vector<int> in = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19 };

    REQUIRE_THAT(  //
        dux::into(std::vector<std::string>{}, xform, in),
        matchers::elements_are("10", "12", "14"));
}

TEST_CASE("reduce with pipe operator", "[transducers]")
{
    const auto reducer = dux::filter([](int x) { return x % 2 == 0; })                         //
                         | dux::transform(str)                                                 //
                         | dux::drop_while([](const std::string& x) { return x.size() < 2; })  //
                         | dux::take(3)                                                        //
                         | delimit{ "|" };

    const std::vector<int> in = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19 };

    REQUIRE_THAT(  //
        dux::reduce(std::string{}, reducer)(in),
        matchers::equal_to("10|12|14"));

    REQUIRE_THAT(  //
        in | dux::reduce(std::string{}, reducer),
        matchers::equal_to("10|12|14"));
}

TEST_CASE("two inputs", "[transducers]")
{
    const auto xform = dux::transform([](int x, char y) { return str(x, y); });

    const std::vector<int> in1 = { 2, 3, 4 };
    const std::string in2 = "ABCDEF";

    REQUIRE_THAT(  //
        dux::into(std::vector<std::string>{}, xform, in1, in2),
        matchers::elements_are("2A", "3B", "4C"));
}

TEST_CASE("three inputs", "[transducers]")
{
    const auto xform = dux::transform([](int x, char y, char z) { return str(x, y, z); });

    const std::vector<int> in1 = { 2, 3, 4 };
    const std::string in2 = "ABCDEF";
    const std::string in3 = "+-+-+";

    REQUIRE_THAT(dux::into(std::vector<std::string>{}, xform, in1, in2, in3), matchers::elements_are("2A+", "3B-", "4C+"));

    REQUIRE_THAT(  //
        dux::reduce(std::string{}, xform | delimit{ "/" })(in1, in2, in3),
        matchers::equal_to("2A+/3B-/4C+"));

    REQUIRE_THAT(  //
        std::tie(in1, in2, in3) | dux::reduce(std::string{}, xform | delimit{ "/" }),
        matchers::equal_to("2A+/3B-/4C+"));
}

TEST_CASE("composition", "[transducers]")
{
    const std::vector<int> in = { 2, 3, 4, 5, 6, 7 };

    REQUIRE_THAT(  //
        dux::reduce(
            std::string{},
            dux::compose(
                dux::filter([](int x) { return x % 2 == 0; }),
                dux::transform([](int x) { return 10 * x; }),
                dux::transform(str),
                dux::take(2))(delimit{ ", " }))(in),
        matchers::equal_to("20, 40"));

    REQUIRE_THAT(  //
        in
            | dux::reduce(
                std::string{},
                dux::compose(
                    dux::filter([](int x) { return x % 2 == 0; }),
                    dux::transform([](int x) { return 10 * x; }),
                    dux::transform(str),
                    dux::take(2))(delimit{ ", " })),
        matchers::equal_to("20, 40"));

    REQUIRE_THAT(  //
        in
            | dux::reduce(
                std::string{},
                dux::filter([](int x) { return x % 2 == 0; })       //
                    | dux::transform([](int x) { return 10 * x; })  //
                    | dux::transform(str)                           //
                    | dux::take(2)                                  //
                    | delimit{ ", " }),
        matchers::equal_to("20, 40"));
}

TEST_CASE("fork", "[transducers]")
{
    const std::vector<int> in = { 2, 3, 5, 6, 7, 9 };

    REQUIRE_THAT(     //
        dux::reduce(  //
            std::string{},
            dux::fork(                                         //
                dux::filter([](int x) { return x % 2 == 0; })  //
                    | delimit{ ", " },
                dux::filter([](int x) { return x % 2 == 1; })                 //
                    | dux::transform([](int x) { return 10 * x; })            //
                    | dux::transform([](int x) { return str('[', x, ']'); })  //
                    | delimit{ "" }))(in),
        matchers::equal_to("2[30][50], 6[70][90]"));

    REQUIRE_THAT(  //
        in
            | dux::reduce(  //
                std::string{},
                dux::fork(                                         //
                    dux::filter([](int x) { return x % 2 == 0; })  //
                        | delimit{ ", " },
                    dux::filter([](int x) { return x % 2 == 1; })                 //
                        | dux::transform([](int x) { return 10 * x; })            //
                        | dux::transform([](int x) { return str('[', x, ']'); })  //
                        | delimit{ "" })),
        matchers::equal_to("2[30][50], 6[70][90]"));
}
