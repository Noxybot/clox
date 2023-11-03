#include "compiler.hpp"

#include <format>
#include <limits>

#include "chunk.hpp"
#include "debug.hpp"
#include "rules.hpp"
#include "scanner.hpp"

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
    [static_cast<int>(TokenType::SEMICOLON)]  = {nullptr, nullptr,
                                                 Precedence::NONE},
    [static_cast<int>(TokenType::SLASH)]      = {nullptr, &compiler::binary,
                                                 Precedence::FACTOR},
    [static_cast<int>(TokenType::STAR)]       = {nullptr, &compiler::binary,
                                                 Precedence::FACTOR},
    [static_cast<int>(TokenType::BANG)]       = {&compiler::unary, nullptr,
                                                 Precedence::NONE},
    [static_cast<int>(TokenType::BANG_EQUAL)] = {nullptr, &compiler::binary,
                                                 Precedence::NONE},
    [static_cast<int>(TokenType::EQUAL)] = {nullptr, nullptr, Precedence::NONE},
    [static_cast<int>(TokenType::EQUAL_EQUAL)]   = {nullptr, &compiler::binary,
                                                    Precedence::EQUALITY},
    [static_cast<int>(TokenType::GREATER)]       = {nullptr, &compiler::binary,
                                                    Precedence::COMPARISON},
    [static_cast<int>(TokenType::GREATER_EQUAL)] = {nullptr, &compiler::binary,
                                                    Precedence::COMPARISON},
    [static_cast<int>(TokenType::LESS)]          = {nullptr, &compiler::binary,
                                                    Precedence::COMPARISON},
    [static_cast<int>(TokenType::LESS_EQUAL)]    = {nullptr, &compiler::binary,
                                                    Precedence::COMPARISON},
    [static_cast<int>(TokenType::IDENTIFIER)]    = {nullptr, nullptr,
                                                    Precedence::NONE},
    [static_cast<int>(TokenType::STRING)]        = {nullptr, nullptr,
                                                    Precedence::NONE},
    [static_cast<int>(TokenType::NUMBER)]        = {&compiler::number, nullptr,
                                                    Precedence::NONE},
    [static_cast<int>(TokenType::AND)]   = {nullptr, nullptr, Precedence::NONE},
    [static_cast<int>(TokenType::CLASS)] = {nullptr, nullptr, Precedence::NONE},
    [static_cast<int>(TokenType::ELSE)]  = {nullptr, nullptr, Precedence::NONE},
    [static_cast<int>(TokenType::FALSE)] = {&compiler::literal, nullptr,
                                            Precedence::NONE},
    [static_cast<int>(TokenType::FOR)]   = {nullptr, nullptr, Precedence::NONE},
    [static_cast<int>(TokenType::FUN)]   = {nullptr, nullptr, Precedence::NONE},
    [static_cast<int>(TokenType::IF)]    = {nullptr, nullptr, Precedence::NONE},
    [static_cast<int>(TokenType::NIL)]   = {&compiler::literal, nullptr,
                                            Precedence::NONE},
    [static_cast<int>(TokenType::OR)]    = {nullptr, nullptr, Precedence::NONE},
    [static_cast<int>(TokenType::PRINT)] = {nullptr, nullptr, Precedence::NONE},
    [static_cast<int>(TokenType::RETURN)] = {nullptr, nullptr,
                                             Precedence::NONE},
    [static_cast<int>(TokenType::SUPER)] = {nullptr, nullptr, Precedence::NONE},
    [static_cast<int>(TokenType::THIS)]  = {nullptr, nullptr, Precedence::NONE},
    [static_cast<int>(TokenType::TRUE)]  = {&compiler::literal, nullptr,
                                            Precedence::NONE},
    [static_cast<int>(TokenType::VAR)]   = {nullptr, nullptr, Precedence::NONE},
    [static_cast<int>(TokenType::WHILE)] = {nullptr, nullptr, Precedence::NONE},
    [static_cast<int>(TokenType::ERROR)] = {nullptr, nullptr, Precedence::NONE},
    [static_cast<int>(TokenType::EOF_)]  = {nullptr, nullptr, Precedence::NONE},
};

compiler::compiler(std::string source) : scanner_(std::move(source)) {}

std::optional<std::vector<chunk>> compiler::compile()
{
    chunks_.emplace_back();
    parser_.had_error  = false;
    parser_.panic_mode = false;

    advance();
    expression();
    consume(TokenType::EOF_, "Expect end of expression.");
    end_compiler();
    if (parser_.had_error)
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
    parser_.previous = parser_.current;

    for (;;)
    {
        parser_.current = scanner_.scan_token();
        if (parser_.current.type != TokenType::ERROR)
        {
            break;
        }
        parser_.error_at_current(parser_.current.lexeme);
    }
}

void compiler::consume(TokenType type, std::string_view message)
{
    if (parser_.current.type == type)
    {
        advance();
        return;
    }
    parser_.error_at_current(message);
}

void compiler::expression() { parse_precedence(Precedence::ASSIGNMENT); }

void compiler::number()
{
    const auto value = std::strtod(parser_.previous.lexeme.data(), nullptr);
    emit_constant(value);
}
void compiler::unary()
{
    const auto operator_type = parser_.previous.type;

    // Compile the operand.
    parse_precedence(Precedence::UNARY);

    // Emit the operator instruction.
    switch (operator_type)
    {
        case clox::TokenType::MINUS:
            emit_bytes(OpCode::OP_NEGATE);
            break;
        case clox::TokenType::BANG:
            emit_bytes(OpCode::OP_NOT);
            break;
        default:
            return;  // Unreachable.
    }
}

void compiler::parse_precedence(Precedence precedence)
{
    advance();
    const auto prefix_rule = get_rule(parser_.previous.type)->prefix;
    if (prefix_rule == nullptr)
    {
        parser_.error("Expect expression.");
        return;
    }
    std::invoke(prefix_rule, this);
    while (precedence <= get_rule(parser_.current.type)->precedence)
    {
        advance();
        const auto infix_rule = get_rule(parser_.previous.type)->infix;
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
    const auto operator_type = parser_.previous.type;
    auto*      rule          = get_rule(operator_type);
    parse_precedence(
        static_cast<Precedence>(static_cast<int>(rule->precedence) + 1));

    switch (operator_type)
    {
        case TokenType::BANG_EQUAL:
            emit_bytes(OpCode::OP_EQUAL, OpCode::OP_NOT);
            break;
        case TokenType::EQUAL_EQUAL:
            emit_bytes(OpCode::OP_EQUAL);
            break;
        case TokenType::GREATER:
            emit_bytes(OpCode::OP_GREATER);
            break;
        case TokenType::GREATER_EQUAL:
            emit_bytes(OpCode::OP_LESS, OpCode::OP_NOT);
            break;
        case TokenType::LESS:
            emit_bytes(OpCode::OP_LESS);
            break;
        case TokenType::LESS_EQUAL:
            emit_bytes(OpCode::OP_GREATER, OpCode::OP_NOT);
            break;
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

void compiler::literal()
{
    switch (parser_.previous.type)
    {
        case TokenType::FALSE:
            emit_bytes(OpCode::OP_FALSE);
            break;
        case TokenType::NIL:
            emit_bytes(OpCode::OP_NIL);
            break;
        case TokenType::TRUE:
            emit_bytes(OpCode::OP_TRUE);
            break;
        default:
            return;  // Unreachable.
    }
}

template <class... Args>
void compiler::emit_bytes(Args... bytes)
{
    (current_chunk().write_chunk(bytes, parser_.current.line), ...);
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
        parser_.error("Too many constants in one chunk.");
        return {};
    }
    return static_cast<std::byte>(constant);
}

chunk& compiler::current_chunk() { return chunks_.back(); }

}  // namespace clox
