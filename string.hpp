#pragma once

#include <iostream>

class String
{
  private:
    char* data_ = nullptr;
    size_t size_ = 0;

  public:
    String() = default;
    String(const char* other);
    String(const size_t& n, const char& c);
    ~String();

    size_t size() const;
    char& operator[](size_t index) const;
};