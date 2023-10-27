#pragma once
#include <iostream>
#include <sstream>
#include <string>

namespace clox
{
enum class TokenType
{
    // Single-character tokens.
    LEFT_PAREN,
    RIGHT_PAREN,
    LEFT_BRACE,
    RIGHT_BRACE,
    COMMA,
    DOT,
    MINUS,
    PLUS,
    SEMICOLON,
    SLASH,
    STAR,
    // One or two character tokens.
    BANG,
    BANG_EQUAL,
    EQUAL,
    EQUAL_EQUAL,
    GREATER,
    GREATER_EQUAL,
    LESS,
    LESS_EQUAL,
    // Literals.
    IDENTIFIER,
    STRING,
    NUMBER,
    // Keywords.
    AND,
    CLASS,
    ELSE,
    FALSE,
    FOR,
    FUN,
    IF,
    NIL,
    OR,
    PRINT,
    RETURN,
    SUPER,
    THIS,
    TRUE,
    VAR,
    WHILE,

    ERROR,
    EOF_
};

struct token
{
    TokenType        type;
    std::string_view lexeme;
    int              line;
    std::string      string() const
    {
        std::stringstream ss;
        ss << "type: " << static_cast<int>(type) << ", lexeme:'" << lexeme
           << "', line: " << line << std::endl;
        return ss.str();
    }
};

class scanner
{
    const std::string           source_;
    std::string::const_iterator start_;
    std::string::const_iterator current_;
    int                         line_ = 1;

  public:
    explicit scanner(std::string source);
    token scan_token();

  private:
    token     make_token(TokenType type) const;
    token     error_token(std::string_view message) const;
    token     string();
    token     number();
    token     identifier();
    TokenType check_keyword(std::string_view tail, TokenType token,
                            int start = 1);
    bool      is_at_end() const;
    char      advance();
    bool      match(char expected);
    void      skip_whitespaces();
    char      peek() const;
    char      peek_next() const;
};

}  // namespace clox
