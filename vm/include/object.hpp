#pragma once

#include <memory>
#include <string>

namespace clox
{
enum class ObjType
{
    STRING,
};

class obj
{
  public:
    virtual ~obj() = default;

    virtual ObjType type() const = 0;

    virtual void print() const = 0;

    virtual bool operator==(const obj& other) const = 0;
};

class obj_string : public obj
{
    std::shared_ptr<std::string> val_;

  public:
    explicit obj_string(std::string str);
    ObjType                     type() const override;
    void                        print() const override;
    bool                        operator==(const obj& other) const override;
    std::shared_ptr<obj_string> operator+(const obj_string& other) const;

  private:
};

}  // namespace clox
