#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include "lexer.hpp"
#include "parser.hpp"
#include "tests.hpp"

[[nodiscard]] std::optional<std::string> read_file(std::filesystem::path const& filepath) {
    auto file = std::ifstream{ filepath };
    if (not file) {
        return std::nullopt;
    }
    auto stream = std::ostringstream{};
    stream << file.rdbuf();
    if (not stream or not file) {
        return std::nullopt;
    }
    return std::move(stream).str();
}

int main() {
    run_tests();
    auto const source = read_file("demo.pratt");
    if (not source.has_value()) {
        std::cerr << "Failed to read source file.";
        return EXIT_FAILURE;
    }
    auto lexer = Lexer{ source.value() };
    try {
        auto const expression = parse(lexer);
        std::cout << *expression << " => " << expression->evaluate() << '\n';
    } catch (std::exception const& e) {
        std::cerr << "Error: " << e.what() << '\n';
    }
}
