#pragma once

#include <optional>
#include <string_view>
#include "common.hpp"
#include "token.hpp"

class Lexer {
private:
    std::string_view m_source;
    usize m_index = 0;

public:
    explicit Lexer(std::string_view const source)
        : m_source{ source } {}

    [[nodiscard]] std::optional<Token> next();

private:
    void discard_whitespace();
    [[nodiscard]] bool current_is_whitespace() const;
    [[nodiscard]] char current() const;
    [[nodiscard]] bool is_at_end() const;
    void advance();
    [[nodiscard]] bool match(char c);
    [[nodiscard]] std::optional<Token> number();
    [[nodiscard]] Token token_from_previous(TokenType type) const;
    [[nodiscard]] Token token_from_range(usize start_index, TokenType type) const;
};
