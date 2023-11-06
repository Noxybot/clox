#pragma once

#include <cstdint>
#include <vector>

#include "value.hpp"

namespace clox
{
enum class OpCode : std::uint8_t
{
    OP_CONSTANT,  // Has one operand - index in 'constants_' array of the chunk.
    OP_NIL,
    OP_TRUE,
    OP_FALSE,
    OP_EQUAL,
    OP_GREATER,
    OP_LESS,
    OP_ADD,
    OP_SUBTRACT,
    OP_MULTIPLY,
    OP_DIVIDE,
    OP_NOT,
    OP_NEGATE,
    OP_RETURN,
};

class chunk
{
    std::vector<std::uint8_t> code_;
    std::vector<ValueType>    constants_;
    std::vector<int>          lines_;

    using const_idx_t = std::size_t;

  public:
    template <class T>
    void                write_chunk(T code, int line);
    const_idx_t         add_constant(ValueType val);
    const std::uint8_t* get_instruction(int idx) const noexcept(false);
    const ValueType&    get_constant(const_idx_t idx) const noexcept(false);
    std::size_t         size() const;
    int                 line(std::size_t idx) const;

    friend class debug;
};

}  // namespace clox
