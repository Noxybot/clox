#pragma once
namespace clox
{
class compiler;
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

typedef void (compiler::*ParseFn)();

struct parse_rule
{
    ParseFn    prefix;
    ParseFn    infix;
    Precedence precedence;
};

}  // namespace clox
