#include "compiler.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "chunk.hpp"

using namespace clox;

TEST_CASE("compiler::single", "[compiler]")
{
    const auto TEST = GENERATE(std::make_pair("+", OpCode::OP_ADD),
                               std::make_pair("-", OpCode::OP_SUBTRACT),
                               std::make_pair("*", OpCode::OP_MULTIPLY),
                               std::make_pair("/", OpCode::OP_DIVIDE));

    clox::compiler comp{"1" + std::string(TEST.first) + "2"};
    const auto     chunks_opt = comp.compile();
    REQUIRE(chunks_opt.has_value());
    const auto chunks = chunks_opt.value();
    REQUIRE(chunks.size() == 1);
    const auto chunk = std::move(chunks[0]);
    CHECK(chunk.get_constant(0) == 1.);
    CHECK(chunk.get_constant(1) == 2.);
    CHECK(*chunk.get_instruction(0) == static_cast<int>(OpCode::OP_CONSTANT));
    CHECK(*chunk.get_instruction(2) == static_cast<int>(OpCode::OP_CONSTANT));
    CHECK(*chunk.get_instruction(4) == static_cast<int>(TEST.second));
    CHECK(*chunk.get_instruction(5) == static_cast<int>(OpCode::OP_RETURN));
}

TEST_CASE("compiler::precedence", "[compiler]")
{
    const auto LOWER_PREC = GENERATE(std::make_pair("+", OpCode::OP_ADD),
                                     std::make_pair("-", OpCode::OP_SUBTRACT));

    const auto HIGHER_PREC = GENERATE(std::make_pair("*", OpCode::OP_MULTIPLY),
                                      std::make_pair("/", OpCode::OP_DIVIDE));

    clox::compiler comp{"1" + std::string(LOWER_PREC.first) + "2" +
                        std::string(HIGHER_PREC.first) + "3"};

    const auto chunks_opt = comp.compile();
    REQUIRE(chunks_opt.has_value());
    const auto chunks = chunks_opt.value();
    REQUIRE(chunks.size() == 1);
    const auto chunk = std::move(chunks[0]);
    CHECK(chunk.get_constant(0) == 1.);
    CHECK(chunk.get_constant(1) == 2.);
    CHECK(chunk.get_constant(2) == 3.);
    CHECK(*chunk.get_instruction(0) == static_cast<int>(OpCode::OP_CONSTANT));
    CHECK(*chunk.get_instruction(2) == static_cast<int>(OpCode::OP_CONSTANT));
    CHECK(*chunk.get_instruction(4) == static_cast<int>(OpCode::OP_CONSTANT));
    CHECK(*chunk.get_instruction(6) == static_cast<int>(HIGHER_PREC.second));
    CHECK(*chunk.get_instruction(7) == static_cast<int>(LOWER_PREC.second));
    CHECK(*chunk.get_instruction(8) == static_cast<int>(OpCode::OP_RETURN));
}

TEST_CASE("compiler::parenthesis", "[compiler]")
{
    const auto LOWER_PREC = GENERATE(std::make_pair("+", OpCode::OP_ADD),
                                     std::make_pair("-", OpCode::OP_SUBTRACT));

    const auto HIGHER_PREC = GENERATE(std::make_pair("*", OpCode::OP_MULTIPLY),
                                      std::make_pair("/", OpCode::OP_DIVIDE));

    clox::compiler comp{"(4214" + std::string(LOWER_PREC.first) + "9549)" +
                        std::string(HIGHER_PREC.first) + "2135"};
    const auto     chunks_opt = comp.compile();
    REQUIRE(chunks_opt.has_value());
    const auto chunks = chunks_opt.value();
    REQUIRE(chunks.size() == 1);
    const auto chunk = std::move(chunks[0]);
    CHECK(chunk.get_constant(0) == 4214.);
    CHECK(chunk.get_constant(1) == 9549.);
    CHECK(chunk.get_constant(2) == 2135.);
    CHECK(*chunk.get_instruction(0) == static_cast<int>(OpCode::OP_CONSTANT));
    CHECK(*chunk.get_instruction(2) == static_cast<int>(OpCode::OP_CONSTANT));
    CHECK(*chunk.get_instruction(4) == static_cast<int>(LOWER_PREC.second));
    CHECK(*chunk.get_instruction(5) == static_cast<int>(OpCode::OP_CONSTANT));
    CHECK(*chunk.get_instruction(7) == static_cast<int>(HIGHER_PREC.second));
    CHECK(*chunk.get_instruction(8) == static_cast<int>(OpCode::OP_RETURN));
}

TEST_CASE("compiler::errors", "[compiler]")
{
    clox::compiler comp{GENERATE("1+", "1+++", "-", "1+2)", "(1")};
    CHECK_FALSE(comp.compile().has_value());
}
