#include "chunk.hpp"

namespace clox
{
template <>
void chunk::write_chunk<std::uint8_t>(std::uint8_t code, int line)
{
    code_.push_back(code);
    lines_.push_back(line);
}

template <>
void chunk::write_chunk<OpCode>(OpCode code, int line)
{
    write_chunk<std::uint8_t>(static_cast<std::uint8_t>(code), line);
}

std::uint8_t chunk::add_constant(ValueType val)
{
    constants_.push_back(val);
    // TODO: overflow check?
    return constants_.size() - 1;
}

OpCode chunk::get_op_code(int idx) const noexcept(false)
{
    return static_cast<OpCode>(code_.at(idx));
}
std::size_t chunk::size() const { return code_.size(); }

}  // namespace clox
