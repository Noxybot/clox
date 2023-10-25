#include <iostream>

#include "chunk.hpp"
#include "debug.hpp"
#include "vm.hpp"

int main(int argc, char **argv)
{
    clox::chunk chunk;
    auto        const_idx = chunk.add_constant(1.256);
    chunk.write_chunk(clox::OpCode::OP_CONSTANT, 123);
    chunk.write_chunk(const_idx, 123);

    const_idx = chunk.add_constant(3.4);
    chunk.write_chunk(clox::OpCode::OP_CONSTANT, 123);
    chunk.write_chunk(const_idx, 123);

    chunk.write_chunk(clox::OpCode::OP_ADD, 123);

    const_idx = chunk.add_constant(5.6);
    chunk.write_chunk(clox::OpCode::OP_CONSTANT, 123);
    chunk.write_chunk(const_idx, 123);

    chunk.write_chunk(clox::OpCode::OP_DIVIDE, 123);

    chunk.write_chunk(clox::OpCode::OP_NEGATE, 123);
    chunk.write_chunk(clox::OpCode::OP_RETURN, 123);
    clox::debug::disassemble_chunk(chunk, "test chunk");
    clox::vm vm{{chunk}};
    vm.interpret();
    return 0;
}
