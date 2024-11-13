#include "lexer.hpp"
#include <array>
#include <cctype>
#include <utility>

[[nodiscard]] std::optional<Token> Lexer::next() {
    using namespace std::string_view_literals;

    static constexpr auto single_char_tokens = std::array{
        std::pair{ '+',               TokenType::Plus },
        std::pair{ '-',              TokenType::Minus },
        std::pair{ '/',              TokenType::Slash },
        std::pair{ '(', TokenType::OpeningParenthesis },
        std::pair{ ')', TokenType::ClosingParenthesis },
    };

    discard_whitespace();
    if (is_at_end()) {
        return Token{ TokenType::EndOfInput, ""sv };
    }

    for (auto const [char_to_match, token_type] : single_char_tokens) {
        if (match(char_to_match)) {
            return token_from_previous(token_type);
        }
    }

    if (match('*')) {
        if (match('*')) {
            return Token{ TokenType::AsteriskAsterisk, m_source.substr(m_index - 2, 2) };
        }
        return token_from_previous(TokenType::Asterisk);
    }

    if (std::isdigit(static_cast<unsigned char>(current()))) {
        return number();
    }
    return std::nullopt;
}

void Lexer::discard_whitespace() {
    while (current_is_whitespace()) {
        advance();
    }
}

[[nodiscard]] bool Lexer::current_is_whitespace() const {
    switch (current()) {
        case ' ':
        case '\t':
        case '\n':
        case '\r':
            return true;
        default:
            return false;
    }
}

[[nodiscard]] char Lexer::current() const {
    return is_at_end() ? '\0' : m_source.at(m_index);
}

[[nodiscard]] bool Lexer::is_at_end() const {
    return m_index >= m_source.length();
}

void Lexer::advance() {
    if (not is_at_end()) {
        ++m_index;
    }
}

bool Lexer::match(char const c) {
    if (current() != c) {
        return false;
    }
    advance();
    return true;
}

[[nodiscard]] std::optional<Token> Lexer::number() {
    auto const start_index = m_index;
    advance();
    while (not is_at_end() and std::isdigit(static_cast<unsigned char>(current()))) {
        advance();
    }
    if (match('.')) {
        if (not std::isdigit(static_cast<unsigned char>(current()))) {
            return std::nullopt;
        }
        advance();
        while (not is_at_end() and std::isdigit(static_cast<unsigned char>(current()))) {
            advance();
        }
    }
    return token_from_range(start_index, TokenType::Number);
}

[[nodiscard]] Token Lexer::token_from_previous(TokenType const type) const {
    return Token{ type, m_source.substr(m_index - 1, 1) };
}

[[nodiscard]] Token Lexer::token_from_range(usize const start_index, TokenType const type) const {
    auto const lexeme = m_source.substr(start_index, m_index - start_index);
    return Token{ type, lexeme };
}
