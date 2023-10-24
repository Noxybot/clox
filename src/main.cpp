#include <iostream>

#include "chunk.hpp"
#include "debug.hpp"

int main(int argc, char **argv)
{
    clox::chunk chunk;
    const auto  const_idx = chunk.add_constant(1.256);
    chunk.write_chunk(clox::OpCode::OP_CONSTANT, 123);
    chunk.write_chunk(const_idx, 123);
    chunk.write_chunk(clox::OpCode::OP_RETURN, 123);
    clox::debug::disassemble_chunk(chunk, "test chunk");
    return 0;
}
