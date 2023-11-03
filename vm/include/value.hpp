#pragma once

#include <variant>

namespace clox
{
struct nil
{
    bool operator==(const nil&) const { return true; }
};
using ValueType = std::variant<double, bool, nil>;

}  // namespace clox
