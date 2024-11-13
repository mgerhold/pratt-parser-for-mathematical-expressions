#pragma once

#include "expression.hpp"
#include "lexer.hpp"

[[nodiscard]] std::unique_ptr<Expression> parse(Lexer& lexer);
