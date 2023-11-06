#pragma once

#include <memory>
#include <variant>

#include "object.hpp"

namespace clox
{
struct nil
{
    bool operator==(const nil&) const { return true; }
};

using ValueType = std::variant<double, bool, nil, std::shared_ptr<obj>>;

inline bool is_string(const ValueType& val)
{
    return std::holds_alternative<std::shared_ptr<obj>>(val) &&
           std::get<std::shared_ptr<obj>>(val)->type() == ObjType::STRING;
}

}  // namespace clox
