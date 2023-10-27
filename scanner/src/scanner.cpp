#include "scanner.hpp"

namespace clox
{
scanner::scanner(std::string source)
    : source_(std::move(source)),
      start_(source_.begin()),
      current_(source_.begin())
{
}

token scanner::scan_token()
{
    skip_whitespaces();
    start_ = current_;
    if (is_at_end())
    {
        return make_token(TokenType::EOF_);
    }
    const char c = advance();
    if (std::isalpha(c))
    {
        return identifier();
    }
    if (std::isdigit(c))
    {
        return number();
    }

    switch (c)
    {
        case '(':
            return make_token(TokenType::LEFT_PAREN);
        case ')':
            return make_token(TokenType::RIGHT_PAREN);
        case '{':
            return make_token(TokenType::LEFT_BRACE);
        case '}':
            return make_token(TokenType::RIGHT_BRACE);
        case ';':
            return make_token(TokenType::SEMICOLON);
        case ',':
            return make_token(TokenType::COMMA);
        case '.':
            return make_token(TokenType::DOT);
        case '-':
            return make_token(TokenType::MINUS);
        case '+':
            return make_token(TokenType::PLUS);
        case '/':
            return make_token(TokenType::SLASH);
        case '*':
            return make_token(TokenType::STAR);

        case '!':
            return make_token(match('=') ? TokenType::BANG_EQUAL
                                         : TokenType::BANG);
        case '=':
            return make_token(match('=') ? TokenType::EQUAL_EQUAL
                                         : TokenType::EQUAL);
        case '<':
            return make_token(match('=') ? TokenType::LESS_EQUAL
                                         : TokenType::LESS);
        case '>':
            return make_token(match('=') ? TokenType::GREATER_EQUAL
                                         : TokenType::GREATER);
        case '"':
            return string();
    }

    return error_token("Unexpected character.");
}

token scanner::make_token(TokenType type) const
{
    token token;
    token.type   = type;
    token.lexeme = {start_, start_ + (current_ - start_)};
    token.line   = line_;
    return token;
}

token scanner::error_token(std::string_view message) const
{
    token token;
    token.type   = TokenType::ERROR;
    token.lexeme = message;
    token.line   = line_;
    return token;
}

token scanner::string()
{
    while (!is_at_end() && peek() != '"')
    {
        if (peek() == '\n')
        {
            line_++;
        }
        advance();
    }
    if (is_at_end())
    {
        return error_token("Unterminated string.");
    }
    // The closing quote.
    advance();
    return make_token(TokenType::STRING);
}
token scanner::number()
{
    while (std::isdigit(peek()))
    {
        advance();
    }
    // Look for a fractional part.
    if (peek() == '.' && std::isdigit(peek_next()))
    {
        // Consume the ".".
        advance();
        while (std::isdigit(peek()))
        {
            advance();
        }
    }
    return make_token(TokenType::NUMBER);
}

token scanner::identifier()
{
    const auto type = [this]
    {
        switch (*start_)
        {
            case 'a':
                return check_keyword("nd", TokenType::AND);
            case 'c':
                return check_keyword("lass", TokenType::CLASS);
            case 'e':
                return check_keyword("lse", TokenType::ELSE);
            case 'f':
                if (current_ - start_ > 1)
                {
                    switch (start_[1])
                    {
                        case 'a':
                            return check_keyword("lse", TokenType::FALSE, 2);
                        case 'o':
                            return check_keyword("r", TokenType::FOR, 2);
                        case 'u':
                            return check_keyword("n", TokenType::FUN, 2);
                    }
                }
                break;
            case 'i':
                return check_keyword("f", TokenType::IF);
            case 'n':
                return check_keyword("il", TokenType::NIL);
            case 'o':
                return check_keyword("r", TokenType::OR);
            case 'p':
                return check_keyword("rint", TokenType::PRINT);
            case 'r':
                return check_keyword("eturn", TokenType::RETURN);
            case 's':
                return check_keyword("uper", TokenType::SUPER);
            case 't':
                if (current_ - start_ > 1)
                {
                    switch (start_[1])
                    {
                        case 'h':
                            return check_keyword("is", TokenType::THIS, 2);
                        case 'r':
                            return check_keyword("ue", TokenType::TRUE, 2);
                    }
                }
                break;
            case 'v':
                return check_keyword("ar", TokenType::VAR);
            case 'w':
                return check_keyword("hile", TokenType::WHILE);
        }
        return TokenType::IDENTIFIER;
    };
    // After the first letter, we allow digits too.
    while (std::isalpha(peek()) || std::isdigit(peek()))
    {
        advance();
    }
    return make_token(type());
}

TokenType scanner::check_keyword(std::string_view tail, TokenType token,
                                 int start)
{
    for (int i = 0; i < tail.size(); ++i)
    {
        if (start_[start + i] != tail[i])
        {
            return TokenType::IDENTIFIER;
        }
    }
    return token;
}

bool scanner::is_at_end() const { return *current_ == '\0'; }

char scanner::advance()
{
    ++current_;
    return *std::prev(current_);
}

bool scanner::match(char expected)
{
    if (is_at_end())
    {
        return false;
    }
    if (*current_ != expected)
    {
        return false;
    }
    ++current_;
    return true;
}

void scanner::skip_whitespaces()
{
    for (;;)
    {
        const char c = peek();
        switch (c)
        {
            case ' ':
            case '\r':
            case '\t':
                advance();
                break;
            case '\n':
                ++line_;
                advance();
                break;
            case '/':
                if (peek_next() == '/')
                {
                    // A comment goes until the end of the line.
                    while (peek() != '\n' && !is_at_end())
                    {
                        advance();
                    }
                }
                else
                {
                    return;
                }
                break;
            default:
                return;
        }
    }
}

char scanner::peek() const { return *current_; }

char scanner::peek_next() const
{
    if (is_at_end())
        return '\0';
    return current_[1];
}

}  // namespace clox
