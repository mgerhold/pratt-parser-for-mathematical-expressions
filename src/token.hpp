#pragma once

#include <iomanip>
#include <iostream>
#include <string_view>
#include <utility>

enum class TokenType {
    Number,
    Plus,
    Minus,
    Asterisk,
    AsteriskAsterisk,
    Slash,
    OpeningParenthesis,
    ClosingParenthesis,
    EndOfInput,
};

inline std::ostream& operator<<(std::ostream& os, TokenType const token_type) {
    switch (token_type) {
        case TokenType::Number:
            return os << "Number";
        case TokenType::EndOfInput:
            return os << "EndOfInput";
        case TokenType::Plus:
            return os << "Plus";
        case TokenType::Minus:
            return os << "Minus";
        case TokenType::Asterisk:
            return os << "Asterisk";
        case TokenType::AsteriskAsterisk:
            return os << "AsteriskAsterisk";
        case TokenType::Slash:
            return os << "Slash";
        case TokenType::OpeningParenthesis:
            return os << "OpeningParenthesis";
        case TokenType::ClosingParenthesis:
            return os << "ClosingParenthesis";
    }
    std::unreachable();
}

struct Token final {
    TokenType type;
    std::string_view lexeme;

    Token()
        : type{ TokenType::EndOfInput } {}

    Token(TokenType const type, std::string_view const lexeme)
        : type{ type }, lexeme{ lexeme } {}

    friend std::ostream& operator<<(std::ostream& os, Token const& token) {
        return os << std::setw(20) << token.type << " ['" << token.lexeme << "']";
    }
};
