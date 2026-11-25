#pragma once

#include <cstring>
#include <iostream>

class String
{
  private:
    char* data_;
    size_t size_ = 0;

  public:
    String();
    String(const char* other);
    String(const size_t& n, const char& c);
    String(const String& other);
    ~String();

    size_t size() const;
    char& operator[](size_t index) const;
    String& operator=(const String& other)
    {
        size_ = other.size_;
        delete[] data_;
        data_ = new char[other.size_ + 1];
        strcpy(data_, other.data_);
        return *this;
    };
    char* Data() const;
};