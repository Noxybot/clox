#pragma once

#include <cstdint>
#include <vector>

namespace clox
{
enum class OpCode : std::uint8_t
{
    OP_CONSTANT,  // Has one operand - index in 'constants_' array of the chunk.
    OP_ADD,
    OP_SUBTRACT,
    OP_MULTIPLY,
    OP_DIVIDE,
    OP_NEGATE,
    OP_RETURN,
};

using ValueType = double;

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
    ValueType           get_constant(const_idx_t idx) const noexcept(false);
    std::size_t         size() const;

    friend class debug;
};

}  // namespace clox
