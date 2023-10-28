#include "compiler.hpp"

#include <format>
#include <limits>

#include "chunk.hpp"
#include "scanner.hpp"

namespace clox
{

struct parser
{
    token current;
    token previous;
    bool  had_error;
    bool  panic_mode;
} parser;

compiler::compiler(std::string source) : scanner_(std::move(source)) {}

std::optional<std::vector<chunk>> compiler::compile()
{
    chunks_.emplace_back();
    parser.had_error  = false;
    parser.panic_mode = false;

    advance();
    expression();
    consume(TokenType::EOF_, "Expect end of expression.");
    end_compiler();
    if (parser.had_error)
    {
        return std::nullopt;
    }
    return std::move(chunks_);
}

void compiler::advance()
{
    parser.previous = parser.current;

    for (;;)
    {
        parser.current = scanner_.scan_token();
        if (parser.current.type != TokenType::ERROR)
        {
            break;
        }
        error_at_current(parser.current.lexeme);
    }
}

void compiler::consume(TokenType type, std::string_view message)
{
    if (parser.current.type == type)
    {
        advance();
        return;
    }
    error_at_current(message);
}

void compiler::expression() { parse_precedence(Precedence::ASSIGNMENT); }

void compiler::groupping()
{
    expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
}

void compiler::number()
{
    const auto value = std::strtod(parser.previous.lexeme.data(), nullptr);
    emit_constant(value);
}
void compiler::unary()
{
    const auto operator_type = parser.previous.type;

    // Compile the operand.
    parse_precedence(Precedence::UNARY);

    // Emit the operator instruction.
    switch (operator_type)
    {
        case clox::TokenType::MINUS:
            emit_bytes(OpCode::OP_NEGATE);
            break;
        default:
            return;  // Unreachable.
    }
}

void compiler::parse_precedence(Precedence precedence) {}

void compiler::end_compiler() { emit_return(); }

void compiler::binary()
{
    const auto operator_type = parser.previous.type;
    ParseRule* rule          = get_rule(operatorType);
    parsePrecedence((Precedence)(rule->precedence + 1));

    switch (operator_type)
    {
        case TokenType::PLUS:
            emit_bytes(OpCode::OP_ADD);
            break;
        case TokenType::MINUS:
            emit_bytes(OpCode::OP_SUBTRACT);
            break;
        case TokenType::STAR:
            emit_bytes(OpCode::OP_MULTIPLY);
            break;
        case TokenType::SLASH:
            emit_bytes(OpCode::OP_DIVIDE);
            break;
        default:
            return;  // Unreachable.
    }
}

template <class... Args>
void compiler::emit_bytes(Args... bytes)
{
    (current_chunk().write_chunk(bytes, parser.current.line), ...);
}

void compiler::emit_return()
{
    emit_bytes(static_cast<std::byte>(OpCode::OP_RETURN));
}

void compiler::emit_constant(ValueType val)
{
    emit_bytes(static_cast<std::byte>(OpCode::OP_CONSTANT), make_constant(val));
}

std::byte compiler::make_constant(ValueType val)
{
    const auto constant = current_chunk().add_constant(val);
    if (constant >
        static_cast<decltype(constant)>(std::numeric_limits<std::byte>::max()))
    {
        error("Too many constants in one chunk.");
        return {};
    }
    return static_cast<std::byte>(constant);
}

chunk& compiler::current_chunk() { return chunks_.back(); }

void compiler::error(std::string_view message) const
{
    error_at(parser.previous, message);
}

void compiler::error_at_current(std::string_view message) const
{
    error_at(parser.current, message);
}

void compiler::error_at(const token& token, std::string_view message) const
{
    if (parser.panic_mode)
    {
        return;
    }
    parser.panic_mode = true;
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
    parser.had_error = true;
}

}  // namespace clox
