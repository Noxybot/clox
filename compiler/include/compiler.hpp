#pragma once
#include <optional>
#include <string>
#include <string_view>

#include "chunk.hpp"
#include "parser.hpp"
#include "rules.hpp"
#include "scanner.hpp"

namespace clox
{
class compiler
{
    scanner scanner_;
    parser  parser_;

    std::vector<chunk> chunks_;

    static parse_rule rules_[];

  public:
    explicit compiler(std::string source);
    std::optional<std::vector<chunk>> compile();

  private:
    void              advance();
    void              consume(TokenType type, std::string_view message);
    void              expression();
    void              groupping();
    void              number();
    void              unary();
    void              parse_precedence(Precedence precedence);
    const parse_rule* get_rule(TokenType type) const;
    void              end_compiler();
    void              grouping();
    void              binary();

    template <class... Args>
    void      emit_bytes(Args... bytes);
    void      emit_return();
    void      emit_constant(ValueType val);
    std::byte make_constant(ValueType val);

    chunk& current_chunk();
};

}  // namespace clox
