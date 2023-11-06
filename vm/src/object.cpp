#include "object.hpp"

#include <iostream>

namespace clox
{

obj_string::obj_string(std::string str)
    : val_(std::make_shared<std::string>(std::move(str)))
{
}

ObjType obj_string::type() const { return ObjType::STRING; }

void obj_string::print() const { std::cout << '"' << *val_ << '"'; }

bool obj_string::operator==(const obj& other) const
{
    if (other.type() != ObjType::STRING)
    {
        return false;
    }
    return *static_cast<const obj_string&>(other).val_ == *val_;
}

std::shared_ptr<obj_string> obj_string::operator+(const obj_string& other) const
{
    return std::make_shared<obj_string>(*val_ + *other.val_);
}

}  // namespace clox
