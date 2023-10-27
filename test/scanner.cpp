#include "scanner.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

using namespace clox;

TEST_CASE("scanner::single", "[scanner]")
{
    const auto test =
        GENERATE(std::make_pair("(", TokenType::LEFT_PAREN),
                 std::make_pair(")", TokenType::RIGHT_PAREN),
                 std::make_pair("{", TokenType::LEFT_BRACE),
                 std::make_pair("}", TokenType::RIGHT_BRACE),
                 std::make_pair(",", TokenType::COMMA),
                 std::make_pair(".", TokenType::DOT),
                 std::make_pair("-", TokenType::MINUS),
                 std::make_pair("+", TokenType::PLUS),
                 std::make_pair(";", TokenType::SEMICOLON),
                 std::make_pair("/", TokenType::SLASH),
                 std::make_pair("*", TokenType::STAR),
                 std::make_pair("!", TokenType::BANG),
                 std::make_pair("!=", TokenType::BANG_EQUAL),
                 std::make_pair("==", TokenType::EQUAL_EQUAL),
                 std::make_pair("=", TokenType::EQUAL),
                 std::make_pair(">", TokenType::GREATER),
                 std::make_pair(">=", TokenType::GREATER_EQUAL),
                 std::make_pair("<", TokenType::LESS),
                 std::make_pair("<=", TokenType::LESS_EQUAL),
                 std::make_pair("identifier", TokenType::IDENTIFIER),
                 std::make_pair("\"test_string\"", TokenType::STRING),
                 std::make_pair("42.42", TokenType::NUMBER),
                 std::make_pair("and", TokenType::AND),
                 std::make_pair("class", TokenType::CLASS),
                 std::make_pair("else", TokenType::ELSE),
                 std::make_pair("false", TokenType::FALSE),
                 std::make_pair("for", TokenType::FOR),
                 std::make_pair("fun", TokenType::FUN),
                 std::make_pair("if", TokenType::IF),
                 std::make_pair("nil", TokenType::NIL),
                 std::make_pair("or", TokenType::OR),
                 std::make_pair("print", TokenType::PRINT),
                 std::make_pair("return", TokenType::RETURN),
                 std::make_pair("super", TokenType::SUPER),
                 std::make_pair("this", TokenType::THIS),
                 std::make_pair("true", TokenType::TRUE),
                 std::make_pair("var", TokenType::VAR),
                 std::make_pair("while", TokenType::WHILE),
                 std::make_pair("\"some unterminated string", TokenType::ERROR),
                 std::make_pair("", TokenType::EOF_));

    scanner scan{test.first};

    const auto token = scan.scan_token();
    DYNAMIC_SECTION("TEST: " << token.string())
    {
        CHECK(token.type == test.second);
        if (test.second == TokenType::ERROR)
        {
            CHECK(token.lexeme == "Unterminated string.");
        }
        else
        {
            CHECK(token.lexeme == test.first);
        }
        CHECK(token.line == 1);
    }
}
