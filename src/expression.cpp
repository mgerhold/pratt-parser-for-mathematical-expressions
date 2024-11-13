#include "expression.hpp"
#include <cmath>

Number::Number(double const value)
    : m_value{ value } {}

void Number::print_to_stream(std::ostream& os) const {
    os << m_value;
}

[[nodiscard]] double Number::evaluate() const {
    return m_value;
}

UnaryOperator::UnaryOperator(TokenType const type, std::unique_ptr<Expression> operand)
    : m_type{ type }, m_operand{ std::move(operand) } {}

void UnaryOperator::print_to_stream(std::ostream& os) const {
    auto const operator_char = [&] {
        // clang-format off
        switch (m_type) {
            using enum TokenType;
            case Plus:  return '+';
            case Minus: return '-';
            default:    return '?';
        }
        // clang-format on
    }();
    os << operator_char << "(" << *m_operand << ')';
}

[[nodiscard]] double UnaryOperator::evaluate() const {
    switch (m_type) {
        using enum TokenType;
        case Plus:
            return m_operand->evaluate();
        case Minus:
            return -m_operand->evaluate();
        default:
            throw std::runtime_error{ "unreachable" };
    }
}

BinaryOperator::BinaryOperator(
    std::unique_ptr<Expression> left_operand,
    TokenType const type,
    std::unique_ptr<Expression> right_operand
)
    : m_left_operand{ std::move(left_operand) }, m_type{ type }, m_right_operand{ std::move(right_operand) } {}

void BinaryOperator::print_to_stream(std::ostream& os) const {
    auto operator_chars = [&] {
        // clang-format off
        switch (m_type) {
            using enum TokenType;
            case Plus:             return "+";
            case Minus:            return "-";
            case Asterisk:         return "*";
            case Slash:            return "/";
            case AsteriskAsterisk: return "**";
            default:               return "?";
        }
        // clang-format on
    }();
    os << '(' << *m_left_operand << ' ' << operator_chars << ' ' << *m_right_operand << ')';
}

[[nodiscard]] double BinaryOperator::evaluate() const {
    switch (m_type) {
        using enum TokenType;
        case Plus:
            return m_left_operand->evaluate() + m_right_operand->evaluate();
        case Minus:
            return m_left_operand->evaluate() - m_right_operand->evaluate();
        case Asterisk:
            return m_left_operand->evaluate() * m_right_operand->evaluate();
        case Slash:
            return m_left_operand->evaluate() / m_right_operand->evaluate();
        case AsteriskAsterisk:
            return std::pow(m_left_operand->evaluate(), m_right_operand->evaluate());
        default:
            throw std::runtime_error{ "Binary operator not implemented." };
    }
}
