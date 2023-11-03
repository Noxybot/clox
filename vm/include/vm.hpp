#pragma once

#include <stack>
#include <string_view>

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
    template <class... Args>
    void runtime_error(std::string_view format, Args&&... args);
};

}  // namespace clox
