#pragma once
#include "chunk.hpp"

namespace clox
{
class debug
{
    static int constant_instruction(std::string_view name, const chunk& chunk,
                                    int offset);

  public:
    static int  disassemble_instruction(const chunk& chunk, int offset);
    static void disassemble_chunk(const chunk& chunk, std::string_view name);
    static void print_value(const clox::ValueType& val);
};

}  // namespace clox
