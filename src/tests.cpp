#include "tests.hpp"
#include <array>
#include <iostream>
#include <string_view>
#include <tuple>
#include "common.hpp"
#include "lexer.hpp"
#include "parser.hpp"

[[nodiscard]] static bool run_test(
    std::string_view const test_name,
    std::string_view const source,
    double const expected_result
) {
    std::cout << "Executing test '" << test_name << "'...";
    auto lexer = Lexer{ source };
    try {
        auto const expression = parse(lexer);
        auto const result = expression->evaluate();
        auto const diff = std::abs(result - expected_result);
        if (diff > std::numeric_limits<double>::epsilon()) {
            std::cout << "FAILED: expected " << expected_result << ", got " << result << '\n';
            return false;
        }
        std::cout << "PASSED\n";
        return true;
    } catch (std::exception const& e) {
        std::cout << "FAILED TO EXECUTE: '" << e.what() << "'\n";
        return false;
    }
}

using namespace std::string_view_literals;

static constexpr auto test_cases = std::array{
    std::tuple{ "evaluate single number"sv,              "3.14"sv,    3.14 },
    std::tuple{           "unary plus 1"sv,             "+3.14"sv,    3.14 },
    std::tuple{           "unary plus 2"sv,                "+0"sv,     0.0 },
    std::tuple{           "unary plus 3"sv,              "+++0"sv,     0.0 },
    std::tuple{           "unary plus 4"sv,           "+++3.14"sv,    3.14 },
    std::tuple{          "unary minus 1"sv,             "-3.14"sv,   -3.14 },
    std::tuple{          "unary minus 2"sv,                "-0"sv,     0.0 },
    std::tuple{          "unary minus 3"sv,              "---0"sv,     0.0 },
    std::tuple{          "unary minus 4"sv,           "---3.14"sv,   -3.14 },
    std::tuple{          "binary plus 1"sv,             "1 + 2"sv,     3.0 },
    std::tuple{          "binary plus 2"sv,         "1.5 + 2.5"sv,     4.0 },
    std::tuple{          "binary plus 3"sv,         "1 + 2 + 3"sv,     6.0 },
    std::tuple{         "binary minus 1"sv,             "5 - 2"sv,     3.0 },
    std::tuple{         "binary minus 2"sv,             "5 - 7"sv,    -2.0 },
    std::tuple{         "binary minus 3"sv,         "6 - 2 - 5"sv,    -1.0 },
    std::tuple{      "binary asterisk 1"sv,             "2 * 3"sv,     6.0 },
    std::tuple{      "binary asterisk 2"sv,         "2 * 3 * 4"sv,    24.0 },
    std::tuple{         "binary slash 1"sv,            "28 / 2"sv,    14.0 },
    std::tuple{         "binary slash 2"sv,       "200 / 4 / 5"sv,    10.0 },
    std::tuple{                  "power"sv,              "2**8"sv,   256.0 },
    std::tuple{             "grouping 1"sv,       "(2 + 3) * 4"sv,    20.0 },
    std::tuple{             "grouping 2"sv,       "4 * (2 + 3)"sv,    20.0 },
    std::tuple{                "mixed 1"sv, "3 + 5 - 3 * 2 + 9"sv,    11.0 },
    std::tuple{                "mixed 2"sv,     "7 - 4**-2 + 8"sv, 14.9375 },
    std::tuple{                "mixed 3"sv,    "2**(2 + 3 + 3)"sv,   256.0 },
    std::tuple{                "mixed 4"sv,        "(1 + 3)**2"sv,    16.0 },
    std::tuple{                "mixed 4"sv,     "(-(1 + 3))**2"sv,    16.0 },
    std::tuple{                "mixed 4"sv,     "(-(1 + 3))**3"sv,   -64.0 },
};

void run_tests() {
    auto num_tests_passed = usize{ 0 };
    for (auto const& [test_name, source, expected_result] : test_cases) {
        if (run_test(test_name, source, expected_result)) {
            ++num_tests_passed;
        }
    }
    std::cout << "RESULT: " << num_tests_passed << " tests passed out of " << test_cases.size() << '\n';
    if (num_tests_passed == test_cases.size()) {
        std::cout << "=== ALL TESTS PASSED ===\n";
    } else {
        std::cout << "=== SOME TEST(S) FAILED ===\n";
    }
}
