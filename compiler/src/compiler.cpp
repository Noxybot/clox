#include "compiler.hpp"

#include <format>
#include <limits>

#include "chunk.hpp"
#include "debug.hpp"
#include "rules.hpp"
#include "scanner.hpp"

#define DEBUG_PRINT_CODE

namespace clox
{

parse_rule compiler::rules_[] = {
    [static_cast<int>(TokenType::LEFT_PAREN)]  = {&compiler::grouping, nullptr,
                                                  Precedence::NONE},
    [static_cast<int>(TokenType::RIGHT_PAREN)] = {nullptr, nullptr,
                                                  Precedence::NONE},
    [static_cast<int>(TokenType::LEFT_BRACE)]  = {nullptr, nullptr,
                                                  Precedence::NONE},
    [static_cast<int>(TokenType::RIGHT_BRACE)] = {nullptr, nullptr,
                                                  Precedence::NONE},
    [static_cast<int>(TokenType::COMMA)] = {nullptr, nullptr, Precedence::NONE},
    [static_cast<int>(TokenType::DOT)]   = {nullptr, nullptr, Precedence::NONE},
    [static_cast<int>(TokenType::MINUS)] = {&compiler::unary, &compiler::binary,
                                            Precedence::TERM},
    [static_cast<int>(TokenType::PLUS)]  = {nullptr, &compiler::binary,
                                            Precedence::TERM},
    [static_cast<int>(TokenType::SEMICOLON)] = {nullptr, nullptr,
                                                Precedence::NONE},
    [static_cast<int>(TokenType::SLASH)]     = {nullptr, &compiler::binary,
                                                Precedence::FACTOR},
    [static_cast<int>(TokenType::STAR)]      = {nullptr, &compiler::binary,
                                                Precedence::FACTOR},
    [static_cast<int>(TokenType::BANG)] = {nullptr, nullptr, Precedence::NONE},
    [static_cast<int>(TokenType::BANG_EQUAL)] = {nullptr, nullptr,
                                                 Precedence::NONE},
    [static_cast<int>(TokenType::EQUAL)] = {nullptr, nullptr, Precedence::NONE},
    [static_cast<int>(TokenType::EQUAL_EQUAL)]   = {nullptr, nullptr,
                                                    Precedence::NONE},
    [static_cast<int>(TokenType::GREATER)]       = {nullptr, nullptr,
                                                    Precedence::NONE},
    [static_cast<int>(TokenType::GREATER_EQUAL)] = {nullptr, nullptr,
                                                    Precedence::NONE},
    [static_cast<int>(TokenType::LESS)] = {nullptr, nullptr, Precedence::NONE},
    [static_cast<int>(TokenType::LESS_EQUAL)] = {nullptr, nullptr,
                                                 Precedence::NONE},
    [static_cast<int>(TokenType::IDENTIFIER)] = {nullptr, nullptr,
                                                 Precedence::NONE},
    [static_cast<int>(TokenType::STRING)]     = {nullptr, nullptr,
                                                 Precedence::NONE},
    [static_cast<int>(TokenType::NUMBER)]     = {&compiler::number, nullptr,
                                                 Precedence::NONE},
    [static_cast<int>(TokenType::AND)]   = {nullptr, nullptr, Precedence::NONE},
    [static_cast<int>(TokenType::CLASS)] = {nullptr, nullptr, Precedence::NONE},
    [static_cast<int>(TokenType::ELSE)]  = {nullptr, nullptr, Precedence::NONE},
    [static_cast<int>(TokenType::FALSE)] = {nullptr, nullptr, Precedence::NONE},
    [static_cast<int>(TokenType::FOR)]   = {nullptr, nullptr, Precedence::NONE},
    [static_cast<int>(TokenType::FUN)]   = {nullptr, nullptr, Precedence::NONE},
    [static_cast<int>(TokenType::IF)]    = {nullptr, nullptr, Precedence::NONE},
    [static_cast<int>(TokenType::NIL)]   = {nullptr, nullptr, Precedence::NONE},
    [static_cast<int>(TokenType::OR)]    = {nullptr, nullptr, Precedence::NONE},
    [static_cast<int>(TokenType::PRINT)] = {nullptr, nullptr, Precedence::NONE},
    [static_cast<int>(TokenType::RETURN)] = {nullptr, nullptr,
                                             Precedence::NONE},
    [static_cast<int>(TokenType::SUPER)] = {nullptr, nullptr, Precedence::NONE},
    [static_cast<int>(TokenType::THIS)]  = {nullptr, nullptr, Precedence::NONE},
    [static_cast<int>(TokenType::TRUE)]  = {nullptr, nullptr, Precedence::NONE},
    [static_cast<int>(TokenType::VAR)]   = {nullptr, nullptr, Precedence::NONE},
    [static_cast<int>(TokenType::WHILE)] = {nullptr, nullptr, Precedence::NONE},
    [static_cast<int>(TokenType::ERROR)] = {nullptr, nullptr, Precedence::NONE},
    [static_cast<int>(TokenType::EOF_)]  = {nullptr, nullptr, Precedence::NONE},
};

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
    else
    {
#ifdef DEBUG_PRINT_CODE
        debug::disassemble_chunk(current_chunk(), "code");
#endif
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

void compiler::parse_precedence(Precedence precedence)
{
    advance();
    const auto prefix_rule = get_rule(parser.previous.type)->prefix;
    if (prefix_rule == nullptr)
    {
        error("Expect expression.");
        return;
    }
    std::invoke(prefix_rule, this);
    while (precedence <= get_rule(parser.current.type)->precedence)
    {
        advance();
        const auto infix_rule = get_rule(parser.previous.type)->infix;
        std::invoke(infix_rule, this);
    }
}

const parse_rule* compiler::get_rule(TokenType type) const
{
    return &rules_[static_cast<int>(type)];
}

void compiler::end_compiler() { emit_return(); }

void compiler::grouping()
{
    expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
}

void compiler::binary()
{
    const auto operator_type = parser.previous.type;
    auto*      rule          = get_rule(operator_type);
    parse_precedence(
        static_cast<Precedence>(static_cast<int>(rule->precedence) + 1));

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
    if (constant > std::numeric_limits<std::uint8_t>::max())
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
