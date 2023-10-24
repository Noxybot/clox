#pragma once

#include <cstdint>
#include <vector>

namespace clox
{
enum class OpCode
{
    OP_CONSTANT,  // Has one operand - index in 'constants_' array of the chunk.
    OP_RETURN,
};

using ValueType = double;

class chunk
{
    std::vector<std::uint8_t> code_;
    std::vector<ValueType>    constants_;
    std::vector<int>          lines_;

  public:
    template <class T>
    void         write_chunk(T code, int line);
    std::uint8_t add_constant(ValueType val);
    OpCode       get_op_code(int idx) const noexcept(false);
    std::size_t  size() const;

    friend class debug;
};

}  // namespace clox
