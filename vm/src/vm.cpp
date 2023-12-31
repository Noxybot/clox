#include "vm.hpp"

#include <format>
#include <iostream>
#include <memory>
#include <variant>

#include "chunk.hpp"
#include "debug.hpp"
#include "value.hpp"

static bool is_falsey(const clox::ValueType& val)
{
    return std::holds_alternative<clox::nil>(val) ||
           (std::holds_alternative<bool>(val) && std::get<bool>(val) == false);
}

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
InterpretResult vm::interpret() { return run(); }

InterpretResult vm::run()
{
#ifdef DEBUG_TRACE_EXECUTION
    std::cout << "== run == " << std::endl;
#endif

    const auto read_byte  = [this] { return *ip_++; };
    const auto read_const = [this, read_byte]
    { return current_chunk_->get_constant(read_byte()); };
    const auto peek = [this](const auto idx) -> const ValueType&
    { return stack_[stack_.size() - idx - 1]; };

#define BINARY_OP(type, op)                                             \
    do                                                                  \
    {                                                                   \
        if (!std::holds_alternative<type>(stack_[stack_.size() - 1]) || \
            !std::holds_alternative<type>(stack_[stack_.size() - 2]))   \
        {                                                               \
            runtime_error("Operands must be numbers.");                 \
            return InterpretResult::INTERPRET_RUNTIME_ERROR;            \
        }                                                               \
        const auto b = std::get<double>(stack_pop());                   \
        const auto a = std::get<double>(stack_pop());                   \
        stack_.push_back(a op b);                                       \
    } while (false)

    for (;;)
    {
#ifdef DEBUG_TRACE_EXECUTION
        std::cout << "          ";
        for (const auto& val : stack_)
        {
            std::cout << "[ ";
            clox::debug::print_value(val);
            std::cout << " ]";
        }
        std::cout << std::endl;
        debug::disassemble_instruction(
            *current_chunk_,
            static_cast<int>(ip_ - current_chunk_->get_instruction(0)));
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
                stack_.push_back(constant);
                break;
            }
            case OpCode::OP_NIL:
                stack_.push_back(nil{});
                break;
            case OpCode::OP_TRUE:
                stack_.push_back(true);
                break;
            case OpCode::OP_FALSE:
                stack_.push_back(false);
                break;
            case OpCode::OP_EQUAL:
            {
                const auto b      = stack_pop();
                const auto a      = stack_pop();
                const auto is_obj = [](const auto& val)
                { return std::holds_alternative<std::shared_ptr<obj>>(val); };
                if (is_obj(b) && is_obj(a))
                {
                    stack_.push_back(*std::get<std::shared_ptr<obj>>(b) ==
                                     *std::get<std::shared_ptr<obj>>(a));
                }
                else
                {
                    stack_.push_back(a == b);
                }
                break;
            }
            case OpCode::OP_GREATER:
                BINARY_OP(bool, >);
                break;
            case OpCode::OP_LESS:
                BINARY_OP(bool, <);
                break;
            case OpCode::OP_ADD:
                if (is_string(peek(0)) && is_string(peek(1)))
                {
                    const auto b = std::get<std::shared_ptr<obj>>(stack_pop());
                    const auto a = std::get<std::shared_ptr<obj>>(stack_pop());
                    stack_.push_back(static_cast<obj_string&>(*a) +
                                     static_cast<obj_string&>(*b));
                }
                else if (std::holds_alternative<double>(peek(0)) &&
                         std::holds_alternative<double>(peek(1)))
                {
                    const auto b = std::get<double>(stack_pop());
                    const auto a = std::get<double>(stack_pop());
                    stack_.push_back(a + b);
                }
                else
                {
                    runtime_error(
                        "Operands must be two numbers or two strings.");
                    return InterpretResult::INTERPRET_RUNTIME_ERROR;
                }
                break;
            case OpCode::OP_SUBTRACT:
                BINARY_OP(double, -);
                break;
            case OpCode::OP_MULTIPLY:
                BINARY_OP(double, *);
                break;
            case OpCode::OP_DIVIDE:
                BINARY_OP(double, /);
                break;
            case OpCode::OP_NOT:
                stack_.push_back(is_falsey(stack_pop()));
                break;
            case OpCode::OP_NEGATE:
                if (std::holds_alternative<double>(stack_.back()))
                {
                    stack_.back() = -std::get<double>(stack_.back());
                }
                else
                {
                    runtime_error("Operand must be a number.");
                }
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

template <class... Args>
void vm::runtime_error(std::string_view format, Args&&... args)
{
    std::cout << format << std::endl;
    const auto offset =
        static_cast<std::size_t>(ip_ - current_chunk_->get_instruction(0)) - 1;
    const int line = current_chunk_->line(offset);
    std::cerr << std::format("[line {}] in script.", line) << std::endl;
}

}  // namespace clox
