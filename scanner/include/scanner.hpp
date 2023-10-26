#pragma once
#include <string>

namespace clox
{

class scanner
{
    const std::string           source_;
    std::string::const_iterator start_;
    std::string::const_iterator current_;
    int                         line_;
    explicit scanner(std::string);
};

}  // namespace clox
