#include "chunk.hpp"
namespace clox
{
template <>
void chunk::write_chunk<>(std::uint8_t code, int line)
{
    code_.push_back(code);
    lines_.push_back(line);
}

template <>
void chunk::write_chunk<>(OpCode code, int line)
{
    write_chunk<>(static_cast<std::uint8_t>(code), line);
}

template <>
void chunk::write_chunk<>(std::byte code, int line)
{
    write_chunk<>(static_cast<std::uint8_t>(code), line);
}

chunk::const_idx_t chunk::add_constant(ValueType val)
{
    constants_.push_back(val);
    // TODO: overflow check?
    return constants_.size() - 1;
}

const std::uint8_t* chunk::get_instruction(int idx) const noexcept(false)
{
    return &code_[idx];
}

ValueType chunk::get_constant(const_idx_t idx) const noexcept(false)
{
    return constants_[idx];
}

std::size_t chunk::size() const { return code_.size(); }

int chunk::line(std::size_t idx) const { return lines_.at(idx); }

}  // namespace clox
