#include "parser.hpp"

#include <cassert>
#include <sstream>
#include "precedence.hpp"
#include "utils.hpp"

class Parser final {
private:
    Lexer* m_lexer;
    Token m_current;
    Token m_next;

public:
    explicit Parser(Lexer& lexer)
        : m_lexer{ &lexer } {
        if (auto const current = lexer.next(); current.has_value()) {
            m_current = current.value();
        } else {
            throw std::runtime_error{ "Empty input." };
        }
        if (auto const current = lexer.next(); current.has_value()) {
            m_next = current.value();
        } else {
            throw std::runtime_error{ "Empty input." };
        }
    }

    [[nodiscard]] std::unique_ptr<Expression> expression() {
        return expression(Precedence::Unknown);
    }

    [[nodiscard]] bool is_at_end() const {
        return m_current.type == EndOfInput;
    }

private:
    [[nodiscard]] Token const& current() const;
    void advance();
    void consume(TokenType type, std::string const& error_message);
    [[nodiscard]] std::unique_ptr<Expression> expression(Precedence precedence);
    [[nodiscard]] std::unique_ptr<Expression> number();
    [[nodiscard]] std::unique_ptr<Expression> unary();
    [[nodiscard]] std::unique_ptr<Expression> binary(std::unique_ptr<Expression> left_operand);
    [[nodiscard]] std::unique_ptr<Expression> grouping();
    [[nodiscard]] ParserTableRecord const& current_table_record() const;

    // clang-format off
    using enum TokenType;
    static inline auto parser_table = create_parser_table(
        ParserTableEntry<Number>            { &number,   nullptr, Precedence::Unknown },
        ParserTableEntry<Plus>              { &unary,    &binary, Precedence::Sum     },
        ParserTableEntry<Minus>             { &unary,    &binary, Precedence::Sum     },
        ParserTableEntry<Asterisk>          { nullptr,   &binary, Precedence::Product },
        ParserTableEntry<AsteriskAsterisk>  { nullptr,   &binary, Precedence::Power   },
        ParserTableEntry<Slash>             { nullptr,   &binary, Precedence::Product },
        ParserTableEntry<OpeningParenthesis>{ &grouping, nullptr, Precedence::Unknown },
        ParserTableEntry<ClosingParenthesis>{ nullptr,   nullptr, Precedence::Unknown },
        ParserTableEntry<EndOfInput>        { nullptr,   nullptr, Precedence::Unknown }
    );
    // clang-format on
};

[[nodiscard]] Token const& Parser::current() const {
    return m_current;
}

void Parser::advance() {
    m_current = m_next;
    if (auto const next = m_lexer->next(); next.has_value()) {
        m_next = next.value();
    }
}

void Parser::consume(TokenType const type, std::string const& error_message) {
    if (current().type == type) {
        advance();
    } else {
        throw std::runtime_error{ error_message };
    }
}

[[nodiscard]] std::unique_ptr<Expression> Parser::expression(Precedence const precedence) {
    auto const prefix_parser = current_table_record().prefix_parser;
    if (prefix_parser == nullptr) {
        throw std::runtime_error{ "Unexpected token." };
    }
    auto first_operand = std::invoke(prefix_parser, *this);

    // This could still only be the first operand of a binary operator.
    while (true) {
        auto const [_, infix_parser, infix_precedence] = current_table_record();
        if (infix_precedence <= precedence or infix_parser == nullptr) {
            return first_operand;
        }
        first_operand = std::invoke(infix_parser, *this, std::move(first_operand));
    }
}

[[nodiscard]] std::unique_ptr<Expression> Parser::number() {
    auto stream = std::istringstream{ std::string{ current().lexeme } };
    auto value = 0.0;
    stream >> value;
    assert(stream and stream.eof());
    advance();
    return std::make_unique<::Number>(value);
}

[[nodiscard]] std::unique_ptr<Expression> Parser::unary() {
    auto const type = current().type;
    advance();
    return std::make_unique<UnaryOperator>(type, expression(Precedence::Unary));
}

[[nodiscard]] std::unique_ptr<Expression> Parser::binary(std::unique_ptr<Expression> left_operand) {
    auto const type = current().type;
    advance();
    auto const precedence = parser_table.at(std::to_underlying(type)).infix_precedence;
    auto right_operand = expression(precedence);
    return std::make_unique<BinaryOperator>(std::move(left_operand), type, std::move(right_operand));
}

[[nodiscard]] std::unique_ptr<Expression> Parser::grouping() {
    advance(); // Consume '('.
    auto inner_expression = expression();
    consume(ClosingParenthesis, "Expected ')'.");
    return inner_expression;
}

[[nodiscard]] ParserTableRecord const& Parser::current_table_record() const {
    return parser_table.at(std::to_underlying(current().type));
}

[[nodiscard]] std::unique_ptr<Expression> parse(Lexer& lexer) {
    auto parser = Parser{ lexer };
    auto result = parser.expression();
    if (not parser.is_at_end()) {
        throw std::runtime_error{ "Excess tokens." };
    }
    return result;
}
