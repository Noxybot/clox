#include "parser.hpp"

#include <format>

namespace clox
{

void parser::error(std::string_view message) { error_at(previous, message); }

void parser::error_at_current(std::string_view message)
{
    error_at(current, message);
}

void parser::error_at(const token& token, std::string_view message)
{
    if (panic_mode)
    {
        return;
    }
    panic_mode = true;
    std::cerr << std::format("[line {}] Error", token.line);
    if (token.type == TokenType::EOF_)
    {
        std::cerr << " at end";
    }
    else if (token.type == TokenType::ERROR)
    {
        // Nothing.
    }
    else
    {
        std::cerr << std::format("at {}", token.lexeme);
    }
    std::cerr << std::format(": {}", message) << std::endl;
    had_error = true;
}
}  // namespace clox
