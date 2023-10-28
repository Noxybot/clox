#pragma once
#include <optional>
#include <string>
#include <string_view>

#include "chunk.hpp"
#include "scanner.hpp"

namespace clox
{

// From lowest to highest.
enum class Precedence
{
    NONE,
    ASSIGNMENT,  // =
    OR,          // or
    AND,         // and
    EQUALITY,    // == !=
    COMPARISON,  // < > <= >=
    TERM,        // + -
    FACTOR,      // * /
    UNARY,       // ! -
    CALL,        // . ()
    PRIMARY
};

class compiler
{
    scanner scanner_;

    std::vector<chunk> chunks_;

  public:
    explicit compiler(std::string source);
    std::optional<std::vector<chunk>> compile();

  private:
    void advance();
    void consume(TokenType type, std::string_view message);
    void expression();
    void groupping();
    void number();
    void unary();
    void parse_precedence(Precedence precedence);
    void end_compiler();
    void binary();

    template <class... Args>
    void      emit_bytes(Args... bytes);
    void      emit_return();
    void      emit_constant(ValueType val);
    std::byte make_constant(ValueType val);

    chunk& current_chunk();

    void error(std::string_view message) const;
    void error_at_current(std::string_view message) const;
    void error_at(const token& token, std::string_view message) const;
};

}  // namespace clox
