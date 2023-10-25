#include "../src/chunk.hpp"  // Include the header file that defines the chunk class and its dependencies

#include <catch2/catch_test_macros.hpp>

using namespace clox;

TEST_CASE("Test chunk::write_chunk<std::uint8_t>")
{
    chunk c;
    c.write_chunk<>(static_cast<std::uint8_t>(42), 1);

    REQUIRE(c.size() == 1);
    REQUIRE(*c.get_instruction(0) == 42);
}

TEST_CASE("Test chunk::write_chunk<OpCode>")
{
    chunk c;
    c.write_chunk<OpCode>(OpCode::OP_CONSTANT, 2);

    REQUIRE(c.size() == 1);
    REQUIRE(static_cast<OpCode>(*c.get_instruction(0)) == OpCode::OP_CONSTANT);
}

TEST_CASE("Test chunk::add_constant and chunk::get_op_code")
{
    chunk        c;
    ValueType    constant_value = 3.14;
    std::uint8_t constant_index = c.add_constant(constant_value);

    REQUIRE(c.size() == 0);
    REQUIRE(constant_index == 0);
}
