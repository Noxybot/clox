#include "debug.hpp"

#include <format>
#include <iostream>
#include <memory>

#include "chunk.hpp"

static int simple_instruction(std::string_view name, int offset)
{
    std::cout << name << std::endl;
    return offset + 1;
}

namespace clox
{
template <class... Ts>
struct overloaded : Ts...
{
    using Ts::operator()...;
};
// explicit deduction guide (not needed as of C++20)
template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

void debug::print_value(const clox::ValueType& val)
{
    std::visit(
        overloaded{
            [](const double val) { std::cout << std::format("'{:g}'", val); },
            [](const bool val) { std::cout << std::format("'{}'", val); },
            [](const nil val) { std::cout << "nil"; },
            [](const std::shared_ptr<obj>& val) { val->print(); }},
        val);
}

int debug::constant_instruction(std::string_view name, const chunk& chunk,
                                int offset)
{
    const auto const_idx = chunk.code_[offset + 1];
    std::cout << std::format("{:<16} {:04} ", name, const_idx);
    print_value(chunk.constants_.at(const_idx));
    std::cout << std::endl;
    return offset + 2;
}

void debug::disassemble_chunk(const chunk& chunk, std::string_view name)
{
    std::cout << std::format("== {} ==\n", name) << std::endl;
    for (int offset = 0; offset < chunk.size();)
    {
        offset = disassemble_instruction(chunk, offset);
    }
    std::cout << std::endl;
}

int debug::disassemble_instruction(const chunk& chunk, int offset)
{
    std::cout << std::format("{:04} ", offset);
    const auto instruction =
        static_cast<OpCode>(*chunk.get_instruction(offset));
    if (offset > 0 && chunk.lines_[offset] == chunk.lines_[offset - 1])
    {
        std::cout << "   | ";
    }
    else
    {
        std::cout << std::format("{:4} ", chunk.lines_[offset]);
    }
    switch (instruction)
    {
        case OpCode::OP_RETURN:
            return simple_instruction("OP_RETURN", offset);
        case OpCode::OP_CONSTANT:
            return constant_instruction("OP_CONSTANT", chunk, offset);
        case OpCode::OP_NIL:
            return simple_instruction("OP_NIL", offset);
        case OpCode::OP_TRUE:
            return simple_instruction("OP_TRUE", offset);
        case OpCode::OP_FALSE:
            return simple_instruction("OP_FALSE", offset);
        case OpCode::OP_EQUAL:
            return simple_instruction("OP_EQUAL", offset);
        case OpCode::OP_GREATER:
            return simple_instruction("OP_GREATER", offset);
        case OpCode::OP_LESS:
            return simple_instruction("OP_LESS", offset);
        case OpCode::OP_ADD:
            return simple_instruction("OP_ADD", offset);
        case OpCode::OP_SUBTRACT:
            return simple_instruction("OP_SUBTRACT", offset);
        case OpCode::OP_MULTIPLY:
            return simple_instruction("OP_MULTIPLY", offset);
        case OpCode::OP_DIVIDE:
            return simple_instruction("OP_DIVIDE", offset);
        case OpCode::OP_NOT:
            return simple_instruction("OP_NOT", offset);
        case OpCode::OP_NEGATE:
            return simple_instruction("OP_NEGATE", offset);
        default:
            std::cout << std::format("Unknown opcode {}",
                                     static_cast<int>(instruction))
                      << std::endl;
            return offset + 1;
    }
}

}  // namespace clox
