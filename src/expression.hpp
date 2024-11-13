#pragma once

#include <iostream>
#include <memory>
#include "token.hpp"

class Expression {
public:
    Expression() = default;
    Expression(Expression const& other) = default;
    Expression(Expression&& other) noexcept = default;
    Expression& operator=(Expression const& other) = default;
    Expression& operator=(Expression&& other) noexcept = default;
    virtual ~Expression() = default;

    virtual void print_to_stream(std::ostream& os) const = 0;
    [[nodiscard]] virtual double evaluate() const = 0;

    friend std::ostream& operator<<(std::ostream& os, Expression const& expression) {
        expression.print_to_stream(os);
        return os;
    }
};

class Number final : public Expression {
private:
    double m_value;

public:
    explicit Number(double value);
    void print_to_stream(std::ostream& os) const override;
    [[nodiscard]] double evaluate() const override;
};

class UnaryOperator final : public Expression {
private:
    TokenType m_type;
    std::unique_ptr<Expression> m_operand;

public:
    explicit UnaryOperator(TokenType type, std::unique_ptr<Expression> operand);
    void print_to_stream(std::ostream& os) const override;
    [[nodiscard]] double evaluate() const override;
};

class BinaryOperator final : public Expression {
private:
    std::unique_ptr<Expression> m_left_operand;
    TokenType m_type;
    std::unique_ptr<Expression> m_right_operand;

public:
    explicit BinaryOperator(
        std::unique_ptr<Expression> left_operand,
        TokenType type,
        std::unique_ptr<Expression> right_operand
    );

    void print_to_stream(std::ostream& os) const override;
    [[nodiscard]] double evaluate() const override;
};
