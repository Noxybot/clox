#pragma once

#include "scanner.hpp"

namespace clox
{
struct parser
{
    token current{};
    token previous{};
    bool  had_error  = false;
    bool  panic_mode = false;

    void error(std::string_view message);
    void error_at_current(std::string_view message);
    void error_at(const token& token, std::string_view message);
};

}  // namespace clox
