#pragma once

#include <stack>

#include "chunk.hpp"

namespace clox
{

enum class InterpretResult
{
    INTERPRET_OK,
    INTERPRET_COMPILE_ERROR,
    INTERPRET_RUNTIME_ERROR,
};

class vm
{
    std::vector<chunk>                 chunks_;
    std::vector<chunk>::const_iterator current_chunk_;
    const std::uint8_t*                ip_;
    std::vector<ValueType>             stack_;

  public:
    explicit vm(std::vector<chunk> chunks);
    InterpretResult interpret();
    InterpretResult run();

  private:
    ValueType stack_pop();
};

}  // namespace clox
