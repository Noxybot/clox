#include "vm.hpp"

#include <iostream>

#include "chunk.hpp"
#include "debug.hpp"

#define DEBUG_TRACE_EXECUTION

namespace clox
{
vm::vm(std::vector<chunk> chunks) : chunks_(std::move(chunks))
{
    if (!chunks_.empty())
    {
        current_chunk_ = chunks_.begin();
        ip_            = current_chunk_->get_instruction(0);
    }
}
InterpretResult vm::interpret(std::string source) { return run(); }

InterpretResult vm::run()
{
    const auto read_byte  = [this] { return *ip_++; };
    const auto read_const = [this, read_byte]
    { return current_chunk_->get_constant(read_byte()); };

#define BINARY_OP(op)               \
    do                              \
    {                               \
        const auto b = stack_pop(); \
        const auto a = stack_pop(); \
        stack_.push_back(a op b);   \
    } while (false)

    for (;;)
    {
#ifdef DEBUG_TRACE_EXECUTION
        // disassembleInstruction(vm.chunk,
        //                        (int)(ip_ -
        //                        current_chunk_->get_instruction(0)));

        std::cout << "          ";
        for (const auto& val : stack_)
        {
            std::cout << "[ ";
            clox::debug::print_value(val);
            std::cout << " ]";
        }
        std::cout << std::endl;
#endif
        const auto instr = static_cast<OpCode>(read_byte());
        switch (instr)
        {
            case OpCode::OP_RETURN:
            {
                clox::debug::print_value(stack_pop());
                std::cout << std::endl;
                return InterpretResult::INTERPRET_OK;
            }
            case OpCode::OP_CONSTANT:
            {
                const auto constant = read_const();
                clox::debug::print_value(constant);
                stack_.push_back(constant);
                std::cout << std::endl;
                break;
            }
            case OpCode::OP_ADD:
                BINARY_OP(+);
                break;
            case OpCode::OP_SUBTRACT:
                BINARY_OP(-);
                break;
            case OpCode::OP_MULTIPLY:
                BINARY_OP(*);
                break;
            case OpCode::OP_DIVIDE:
                BINARY_OP(/);
                break;
            case OpCode::OP_NEGATE:
                stack_.back() = -stack_.back();
                break;
        }
    }
#undef BINARY_OP
}

ValueType vm::stack_pop()
{
    if (stack_.empty())
    {
        std::cerr << "Stack Overflow!!!" << std::endl;
        throw;
    }
    const auto val = stack_.back();
    stack_.pop_back();
    return val;
}

}  // namespace clox
